/******************************************************************************
 FitManager.cpp

	BASE CLASS = <NONE>

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include <FitManager.h>
#include "PolyFitDescription.h"
#include "BuiltinFitDescription.h"
#include "NonLinearFitDescription.h"
#include "ModuleFitDescription.h"
#include "DLFitModule.h"
#include "globals.h"
#include "PrefsMgr.h"
#include "PlotApp.h"
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentPrefsVersion = 1;

const JString kFitDlDirName("fitdlmodule");

const JUtf8Byte* FitManager::kFitsChanged = "kFitsChanged::FitManager";

/******************************************************************************
 Constructor

 *****************************************************************************/

FitManager::FitManager()
	:
	JPrefObject(GetPrefsMgr(), kFitManagerID),
	itsIsInitialized(false)
{
	itsFitDescriptions = jnew JPtrArray<FitDescription>(JPtrArrayT::kDeleteAll);
	itsFitDescriptions->SetCompareFunction(FitDescription::CompareFits);

	JPrefObject::ReadPrefs();

	InitializeList();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

FitManager::~FitManager()
{
	JPrefObject::WritePrefs();

	itsFitDescriptions->DeleteAll();
	jdelete itsFitDescriptions;
}

/******************************************************************************
 GetFitCount (public)

 ******************************************************************************/

JSize
FitManager::GetFitCount()
	const
{
	return itsFitDescriptions->GetItemCount();
}

/******************************************************************************
 GetFitDescription (public)

 ******************************************************************************/

const FitDescription&
FitManager::GetFitDescription
	(
	const JIndex index
	)
	const
{
	assert(itsFitDescriptions->IndexValid(index));
	return *(itsFitDescriptions->GetItem(index));
}

FitDescription&
FitManager::GetFitDescription
	(
	const JIndex index
	)
{
	assert(itsFitDescriptions->IndexValid(index));
	return *(itsFitDescriptions->GetItem(index));
}

/******************************************************************************
 AddFitDescription (public)

 ******************************************************************************/

void
FitManager::AddFitDescription
	(
	FitDescription* fit
	)
{
	itsFitDescriptions->InsertSorted(fit);
	Broadcast(FitsChanged());
}

/******************************************************************************
 RemoveFitDescription (public)

 ******************************************************************************/

void
FitManager::RemoveFitDescription
	(
	const JIndex index
	)
{
	assert(itsFitDescriptions->IndexValid(index));
	if (FitIsRemovable(index))
	{
		itsFitDescriptions->DeleteItem(index);
		Broadcast(FitsChanged());
	}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
FitManager::ReadPrefs
	(
	std::istream& input
	)
{
	itsIsInitialized = true;

	JFileVersion version;
	input >> version;

	if (version > kCurrentPrefsVersion)
	{
		return;
	}

	if (version > 0)
	{
		JSize count;
		input >> count;
		for (JIndex i = 1; i <= count; i++)
		{
			FitDescription* fd;
			if (FitDescription::Create(input, &fd))
			{
				itsFitDescriptions->InsertSorted(fd);
			}
		}
	}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
FitManager::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentPrefsVersion << ' ';
	const JSize count	= itsFitDescriptions->GetItemCount();
	JSize rCount		= 0;
	for (JIndex i = 1; i <= count; i++)
	{
		FitDescription* fd	= itsFitDescriptions->GetItem(i);
		if (fd->GetType() == FitDescription::kPolynomial ||
			fd->GetType() == FitDescription::kNonLinear)
		{
			rCount++;
		}
	}
	output << ' ' << rCount << ' ';
	for (JIndex i = 1; i <= count; i++)
	{
		FitDescription* fd	= itsFitDescriptions->GetItem(i);
		if (fd->GetType() == FitDescription::kPolynomial ||
			fd->GetType() == FitDescription::kNonLinear)
		{
			fd->WriteSetup(output);
		}
	}
}

/******************************************************************************
 InitializeList (private)

 ******************************************************************************/

void
FitManager::InitializeList()
{
	auto bd = jnew BuiltinFitDescription(FitDescription::kBLinear);
	itsFitDescriptions->InsertSorted(bd);

	bd = jnew BuiltinFitDescription(FitDescription::kBExp);
	itsFitDescriptions->InsertSorted(bd);

	bd = jnew BuiltinFitDescription(FitDescription::kBPower);
	itsFitDescriptions->InsertSorted(bd);

	for (const auto* path : GetApplication()->GetModulePath())
	{
		JString fitPath	= JCombinePathAndName(*path, kFitDlDirName);
		JDirInfo* dir;
		if (JDirInfo::Create(fitPath, &dir))
		{
			dir->SetWildcardFilter("*.so");
			for (const auto* entry : *dir)
			{
				DLFitModule* fit;
				if (!entry->IsDirectory() && DLFitModule::Create(entry->GetFullName(), &fit))
				{
					auto md = jnew ModuleFitDescription(fit);
					itsFitDescriptions->InsertSorted(md);
				}
			}
			jdelete dir;
		}
	}
}

/******************************************************************************
 FitIsRemovable (public)

 ******************************************************************************/

bool
FitManager::FitIsRemovable
	(
	const JIndex index
	)
{
	assert(itsFitDescriptions->IndexValid(index));
	FitDescription* fd	= itsFitDescriptions->GetItem(index);
	if (fd->GetType() == FitDescription::kPolynomial ||
		fd->GetType() == FitDescription::kNonLinear)
	{
		return true;
	}
	return false;
}
