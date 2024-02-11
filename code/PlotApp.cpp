/******************************************************************************
 PlotApp.cpp

	PlotApp class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "PlotApp.h"
#include "DataDocument.h"
#include "globals.h"
#include "PrefsMgr.h"
#include "AboutDialog.h"
#include "stringData.h"

#include <jx-af/jx/JXDocumentManager.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXHelpManager.h>

#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/JPtrArray.h>
#include <jx-af/jcore/JPoint.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jWebUtil.h>
#include <stdlib.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kAppSignature = "glove";

static const JString kCursorSubPath("/cursormodule/");
static const JString kDataSubPath("/datamodule/");
static const JString kImportSubPath("/importmodule/");
static const JString kExportSubPath("/exportmodule/");
static const JString kFitSubPath("/fitmodule/");

/******************************************************************************
 Constructor

 ******************************************************************************/

PlotApp::PlotApp
	(
	int*		argc,
	char*		argv[],
	bool*		displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kDefaultStringData)
{
// Assumption - person has home dir, or no fileimpprogs

	itsModulePath = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsModulePath != nullptr);

	JString homeDir;

	if (!JGetHomeDirectory(&homeDir))
	{
		homeDir = JGetRootDirectory();
	}

	JAppendDirSeparator(&homeDir);

	JString dmhome = homeDir + ".glove";
	auto* str = jnew JString(dmhome);
	itsModulePath->Append(str);
	str = jnew JString("/usr/local/lib/glove");
	itsModulePath->Append(str);

	*displayAbout = InitGlobals(this);

	if (!*displayAbout)
	{
		*prevVersStr = GetPrefsMgr()->GetGloveVersionStr();
		if (*prevVersStr == JGetString("VERSION"))
		{
			prevVersStr->Clear();
		}
		else
		{
			*displayAbout = true;
		}
	}
	else
	{
		prevVersStr->Clear();
	}

//	itsFileImpProgs = jnew JPtrArray<JString>();


	JXCreateDefaultDocumentManager(true);

/*
	JString fihome = homeDir + ".glove/fileimport/";
	foundFileImpDir = false;
	if (JDirInfo::Create(fihome, &itsFileImpDir))
	{
		foundFileImpDir = true;
		UpdateFileImpProgs();
	}
*/
	itsDirNumber = 1;


	itsDataModules = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsCursorModules = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsImportModules = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsExportModules = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsFitModules = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);

	itsDataPathIndex = jnew JArray<JIndex>;
	itsCursorPathIndex = jnew JArray<JIndex>;
	itsImportPathIndex = jnew JArray<JIndex>;
	itsExportPathIndex = jnew JArray<JIndex>;
	itsFitPathIndex = jnew JArray<JIndex>;

	ReloadDataModules();
	ReloadExportModules();
	ReloadCursorModules();
	ReloadFitModules();
	ReloadImportModules();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PlotApp::~PlotApp()
{
/*
	itsFileImpProgs->DeleteAll();
	jdelete itsFileImpProgs;

	if (foundFileImpDir)
	{
		jdelete itsFileImpDir;
	}
*/
	itsModulePath->DeleteAll();
	jdelete itsModulePath;

	itsDataModules->DeleteAll();
	jdelete itsDataModules;
	itsCursorModules->DeleteAll();
	jdelete itsCursorModules;
	itsImportModules->DeleteAll();
	jdelete itsImportModules;
	itsExportModules->DeleteAll();
	jdelete itsExportModules;
	itsFitModules->DeleteAll();
	jdelete itsFitModules;
	jdelete itsDataPathIndex;
	jdelete itsCursorPathIndex;
	jdelete itsImportPathIndex;
	jdelete itsExportPathIndex;
	jdelete itsFitPathIndex;
	DeleteGlobals();
}

/******************************************************************************
 ReloadImportModules

 ******************************************************************************/

void
PlotApp::ReloadImportModules()
{
	itsImportModules->DeleteAll();
	itsImportPathIndex->RemoveAll();
	JDirInfo* info;

	const JSize count = itsModulePath->GetItemCount();
	for (JSize i = 1; i <= count; i++)
	{
		JString path = *(itsModulePath->GetItem(i)) + kImportSubPath;
		if (JDirInfo::Create(path, &info))
		{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
			{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
				{
					auto* str = jnew JString(entry.GetName());
					itsImportModules->Append(str);
					itsImportPathIndex->AppendItem(i);
				}
			}
			jdelete info;
		}
	}
}

/******************************************************************************
 GetImportModulePath

 ******************************************************************************/

bool
PlotApp::GetImportModulePath
	(
	const JIndex index,
	JString* path
	)
{
	assert (itsImportModules->IndexValid(index));

	JIndex mIndex = itsImportPathIndex->GetItem(index);
	*path = *(itsModulePath->GetItem(mIndex)) + kImportSubPath +
				*(itsImportModules->GetItem(index));
	return true;
/*
	if (foundFileImpDir)
	{
		assert( itsFileImpProgs->IndexValid(index) );

		JString str = itsFileImpDir->GetCWD();
		JAppendDirSeparator(&str);
		str += *(itsFileImpProgs->GetItem(index));
		*path = str;
		return true;
	}

	return false;
*/
}

/******************************************************************************
 GetImportModules

 ******************************************************************************/

JPtrArray<JString>*
PlotApp::GetImportModules()
{
//	if (foundFileImpDir)
//		{
//		itsFileImpDir->UpdateList();
//		}
	return itsImportModules;
}

/******************************************************************************
 Receive


void
PlotApp::Receive
	(
	JBroadcaster* sender,
	const Message& message
	)
{
	if (sender == itsFileImpDir &&
		(message.Is(JDirInfo::kContentsUpdated) ||
		message.Is(JDirInfo::kPermissionsChanged) ))
	{
		UpdateFileImpProgs();
	}

}
 ******************************************************************************/


/******************************************************************************
 DirectorClosed

 ******************************************************************************/

void
PlotApp::DirectorClosed
	(
	JXDirector* theDirector
	)
{

}

/******************************************************************************
 UpdateFileImpProgs

 ******************************************************************************/
/*
void
PlotApp::UpdateFileImpProgs()
{
	for (JSize i = 1; i <= itsFileImpDir->GetEntryCount(); i++)
	{
		const JDirEntry& entry = itsFileImpDir->GetEntry(i);
		if ( entry.IsExecutable() && !(entry.IsDirectory()) )
		{
			JString* str = jnew JString(entry.GetName());
			itsFileImpProgs->Append(str);
		}
	}
}
*/

/******************************************************************************
 NewFile

 ******************************************************************************/

void
PlotApp::NewFile()
{
	JString str = "Untitled " + JString(itsDirNumber);
	itsDirNumber++;
	auto* tableDir = jnew DataDocument(this, str, false);
	tableDir->Activate();
}

/******************************************************************************
 OpenFile

 ******************************************************************************/

bool
PlotApp::OpenFile
	(
	const JString& fileName
	)
{
	JXFileDocument* doc;
	if (JXGetDocumentManager()->FileDocumentIsOpen(fileName, &doc))
	{
		doc->Activate();
		return true;
	}
	else if (!JFileExists(fileName))
	{
		JString msg = fileName;
		msg += " does not exist.";
		JGetUserNotification()->ReportError(msg);
		return false;
	}
	else if (!JFileReadable(fileName))
	{
		JString msg = fileName;
		msg += " is not readable.";
		JGetUserNotification()->ReportError(msg);
		return false;
	}
	else
	{
		auto* tableDir = jnew DataDocument(this, fileName, true);
		tableDir->Activate();
		return true;
	}
}

/******************************************************************************
 ReloadDataModules

 ******************************************************************************/

void
PlotApp::ReloadDataModules()
{
	itsDataModules->DeleteAll();
	itsDataPathIndex->RemoveAll();
	JDirInfo* info;

	const JSize count = itsModulePath->GetItemCount();
	for (JSize i = 1; i <= count; i++)
	{
		JString path = *(itsModulePath->GetItem(i)) + kDataSubPath;
		if (JDirInfo::Create(path, &info))
		{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
			{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
				{
					auto* str = jnew JString(entry.GetName());
					itsDataModules->Append(str);
					itsDataPathIndex->AppendItem(i);
				}
			}
			jdelete info;
		}
	}
}

/******************************************************************************
 GetDataModulePath

 ******************************************************************************/

bool
PlotApp::GetDataModulePath
	(
	const JIndex index,
	JString* path
	)
{
	assert (itsDataModules->IndexValid(index));

	JIndex mIndex = itsDataPathIndex->GetItem(index);
	*path = *(itsModulePath->GetItem(mIndex)) + kDataSubPath +
				*(itsDataModules->GetItem(index));
	return true;

}

/******************************************************************************
 GetDataModules

 ******************************************************************************/

JPtrArray<JString>*
PlotApp::GetDataModules()
{
	return itsDataModules;
}

/******************************************************************************
 ReloadCursorModules

 ******************************************************************************/

void
PlotApp::ReloadCursorModules()
{
	itsCursorModules->DeleteAll();
	itsCursorPathIndex->RemoveAll();
	JDirInfo* info;

	const JSize count = itsModulePath->GetItemCount();
	for (JSize i = 1; i <= count; i++)
	{
		JString path = *(itsModulePath->GetItem(i)) + kCursorSubPath;
		if (JDirInfo::Create(path, &info))
		{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
			{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
				{
					auto* str = jnew JString(entry.GetName());
					itsCursorModules->Append(str);
					itsCursorPathIndex->AppendItem(i);
				}
			}
			jdelete info;
		}
	}
}

/******************************************************************************
 GetCursorModulePath

 ******************************************************************************/

bool
PlotApp::GetCursorModulePath
	(
	const JIndex index,
	JString* path
	)
{
	assert (itsCursorModules->IndexValid(index));

	JIndex mIndex = itsCursorPathIndex->GetItem(index);
	*path = *(itsModulePath->GetItem(mIndex)) + kCursorSubPath +
				*(itsCursorModules->GetItem(index));
	return true;

}

/******************************************************************************
 GetCursorModules

 ******************************************************************************/

JPtrArray<JString>*
PlotApp::GetCursorModules()
{
	return itsCursorModules;
}

/******************************************************************************
 ReloadExportModules

 ******************************************************************************/

void
PlotApp::ReloadExportModules()
{
	itsExportModules->DeleteAll();
	itsExportPathIndex->RemoveAll();
	JDirInfo* info;

	const JSize count = itsModulePath->GetItemCount();
	for (JSize i = 1; i <= count; i++)
	{
		JString path = *(itsModulePath->GetItem(i)) + kExportSubPath;
		if (JDirInfo::Create(path, &info))
		{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
			{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
				{
					auto* str = jnew JString(entry.GetName());
					itsExportModules->Append(str);
					itsExportPathIndex->AppendItem(i);
				}
			}
			jdelete info;
		}
	}
}

/******************************************************************************
 GetExportModulePath

 ******************************************************************************/

bool
PlotApp::GetExportModulePath
	(
	const JIndex index,
	JString* path
	)
{
	assert (itsExportModules->IndexValid(index));

	JIndex mIndex = itsExportPathIndex->GetItem(index);
	*path = *(itsModulePath->GetItem(mIndex)) + kExportSubPath +
				*(itsExportModules->GetItem(index));
	return true;

}

/******************************************************************************
 GetExportModules

 ******************************************************************************/

JPtrArray<JString>*
PlotApp::GetExportModules()
{
	return itsExportModules;
}

/******************************************************************************
 ReloadFitModules

 ******************************************************************************/

void
PlotApp::ReloadFitModules()
{
	itsFitModules->DeleteAll();
	itsFitPathIndex->RemoveAll();
	JDirInfo* info;

	const JSize count = itsModulePath->GetItemCount();
	for (JSize i = 1; i <= count; i++)
	{
		JString path = *(itsModulePath->GetItem(i)) + kFitSubPath;
		if (JDirInfo::Create(path, &info))
		{
			for (JSize j = 1; j <= info->GetEntryCount(); j++)
			{
				const JDirEntry& entry = info->GetEntry(j);
				if ( entry.IsExecutable() && !(entry.IsDirectory()) )
				{
					auto* str = jnew JString(entry.GetName());
					itsFitModules->Append(str);
					itsFitPathIndex->AppendItem(i);
				}
			}
			jdelete info;
		}
	}
}

/******************************************************************************
 GetFitModulePath

 ******************************************************************************/

bool
PlotApp::GetFitModulePath
	(
	const JIndex index,
	JString* path
	)
{
	assert (itsFitModules->IndexValid(index));

	JIndex mIndex = itsFitPathIndex->GetItem(index);
	*path = *(itsModulePath->GetItem(mIndex)) + kFitSubPath +
				*(itsFitModules->GetItem(index));
	return true;

}

/******************************************************************************
 GetFitModules

 ******************************************************************************/

JPtrArray<JString>*
PlotApp::GetFitModules()
{
	return itsFitModules;
}

/******************************************************************************
 DisplayAbout

	prevVersStr can be nullptr.

 ******************************************************************************/

void
PlotApp::DisplayAbout
	(
	const bool		showLicense,
	const JString&	prevVersStr
	)
{
	StartFiber([showLicense, prevVersStr]()
	{
		if (!showLicense || JGetUserNotification()->AcceptLicense())
		{
			auto* dlog = jnew AboutDialog(prevVersStr);
			dlog->DoDialog();

			JCheckForNewerVersion(GetPrefsMgr(), kVersionCheckID);
		}
		else
		{
			ForgetPrefsMgr();
			JXGetApplication()->Quit();
		}
	});
}

/******************************************************************************
 CreateHelpMenu

 ******************************************************************************/

#include "PlotApp-Help.h"

JXTextMenu*
PlotApp::CreateHelpMenu
	(
	JXMenuBar*			menuBar,
	const JUtf8Byte*	sectionName
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(JGetString("MenuTitle::PlotApp_Help"));
	menu->SetMenuItems(kHelpMenuStr);
	menu->SetUpdateAction(JXMenu::kDisableNone);
	ConfigureHelpMenu(menu);

	ListenTo(menu, std::function([this, sectionName](const JXMenu::ItemSelected& msg)
	{
		HandleHelpMenu(sectionName, msg.GetIndex());
	}));

	return menu;
}

/******************************************************************************
 AppendHelpMenuToToolBar

 ******************************************************************************/

void
PlotApp::AppendHelpMenuToToolBar
	(
	JXToolBar*	toolBar,
	JXTextMenu* menu
	)
{
	toolBar->NewGroup();
	toolBar->AppendButton(menu, kHelpTOCCmd);
	toolBar->AppendButton(menu, kHelpWindowCmd);
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
PlotApp::HandleHelpMenu
	(
	const JUtf8Byte*	windowSectionName,
	const JIndex		index
	)
{
	if (index == kHelpAboutCmd)
	{
		DisplayAbout();
	}

	else if (index == kHelpTOCCmd)
	{
		JXGetHelpManager()->ShowTOC();
	}
	else if (index == kHelpWindowCmd)
	{
		JXGetHelpManager()->ShowSection(windowSectionName);
	}

	else if (index == kHelpChangeLogCmd)
	{
		JXGetHelpManager()->ShowChangeLog();
	}
	else if (index == kHelpCreditsCmd)
	{
		JXGetHelpManager()->ShowCredits();
	}
}

/******************************************************************************
 GetModulePath (public)

 ******************************************************************************/

const JPtrArray<JString>&
PlotApp::GetModulePath()
	const
{
	return *itsModulePath;
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JUtf8Byte*
PlotApp::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static public)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
PlotApp::InitStrings()
{
	JGetStringManager()->Register(kAppSignature, kDefaultStringData);
}
