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

const JFileVersion	kCurrentPrefsVersion = 1;

const JString kFitDlDirName("fitdlmodule", JString::kNoCopy);

const JUtf8Byte* FitManager::kFitsChanged	= "kFitsChanged::FitManager";

/******************************************************************************
 Constructor

 *****************************************************************************/

FitManager::FitManager()
	:
	JPrefObject(GetPrefsMgr(), kFitManagerID),
	itsIsInitialized(false)
{
	itsFitDescriptions	= jnew JPtrArray<FitDescription>(JPtrArrayT::kDeleteAll);
	assert(itsFitDescriptions != nullptr);

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
	return itsFitDescriptions->GetElementCount();
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
	return *(itsFitDescriptions->GetElement(index));
}

FitDescription&
FitManager::GetFitDescription
	(
	const JIndex index
	)
{
	assert(itsFitDescriptions->IndexValid(index));
	return *(itsFitDescriptions->GetElement(index));
}

/******************************************************************************
 AddFitDescription (public)

 ******************************************************************************/

void
FitManager::AddFitDescription
	(
	const FitDescription& fit
	)
{
//	FitDescription* fd	= jnew FitDescription(fit);
//	assert(fd != nullptr);

//	itsFitDescriptions->InsertSorted(fd);
}

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
 NewFitDescription (public)

 ******************************************************************************/

void
FitManager::NewFitDescription
	(
	const FitDescription::FitType type
	)
{
//	FitDescription* fd	= jnew FitDescription(type);
//	assert(fd != nullptr);

//	itsFitDescriptions->InsertSorted(fd);
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
		itsFitDescriptions->DeleteElement(index);
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
	itsIsInitialized	= true;
	
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
	const JSize count	= itsFitDescriptions->GetElementCount();
	JSize rCount		= 0;
	for (JIndex i = 1; i <= count; i++)
	{
		FitDescription* fd	= itsFitDescriptions->GetElement(i);
		if (fd->GetType() == FitDescription::kPolynomial ||
			fd->GetType() == FitDescription::kNonLinear)
		{
			rCount++;
		}
	}
	output << ' ' << rCount << ' ';
	for (JIndex i = 1; i <= count; i++)
	{
		FitDescription* fd	= itsFitDescriptions->GetElement(i);
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
	BuiltinFitDescription* bd = 
		jnew BuiltinFitDescription(FitDescription::kBLinear);
	assert(bd != nullptr);
	itsFitDescriptions->InsertSorted(bd);

	bd = 
		jnew BuiltinFitDescription(FitDescription::kBExp);
	assert(bd != nullptr);
	itsFitDescriptions->InsertSorted(bd);

	bd = 
		jnew BuiltinFitDescription(FitDescription::kBPower);
	assert(bd != nullptr);
	itsFitDescriptions->InsertSorted(bd);

	const JPtrArray<JString>& paths	= GetApplication()->GetModulePath();
	const JSize count	= paths.GetElementCount();
	for (JIndex i = 1; i <= count; i++)
	{
		const JString& path	= *(paths.GetElement(i));
		JString fitPath	= JCombinePathAndName(path, kFitDlDirName);
		JDirInfo* dir;
		if (JDirInfo::Create(fitPath, &dir))
		{
			dir->SetWildcardFilter(JString("*.so", JString::kNoCopy));
			const JSize count	= dir->GetEntryCount();
			for (JIndex i = 1; i <= count; i++)
			{
				const JDirEntry& entry = dir->GetEntry(i);
				DLFitModule* fit;
				if (!entry.IsDirectory() && DLFitModule::Create(entry.GetFullName(), &fit))
				{
					ModuleFitDescription* md	= 
						jnew ModuleFitDescription(fit);
					assert(md != nullptr);
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
	FitDescription* fd	= itsFitDescriptions->GetElement(index);
	if (fd->GetType() == FitDescription::kPolynomial ||
		fd->GetType() == FitDescription::kNonLinear)
	{
		return true;
	}
	return false;
}
