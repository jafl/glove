/******************************************************************************
 MDIServer.cpp

	BASE CLASS = public JXMDIServer

	Copyright (C) 1999 by Glenn W. Bach.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <MDIServer.h>
#include <globals.h>
#include <PrefsMgr.h>
#include <PlotApp.h>

#include <jx-af/jx/JXStandAlonePG.h>
#include <jx-af/jx/jXglobals.h>

#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

MDIServer::MDIServer
	(
	PlotApp* app
	)
	:
	JXMDIServer()
{
	itsApp = app;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDIServer::~MDIServer()
{
}

/******************************************************************************
 HandleMDIRequest

 *****************************************************************************/

void
MDIServer::HandleMDIRequest
	(
	const JString& dir,
	const JPtrArray<JString>& argList
	)
{
	const JSize argCount = argList.GetElementCount();
	if (argCount == 1)
	{
		itsApp->NewFile();
		return;
	}

	const JString origDir = JGetCurrentDirectory();
	if (!JChangeDirectory(dir).OK())
	{
		const JUtf8Byte* map[] =
		{
			"name", dir.GetBytes()
		};
		const JString msg = JGetString("DirectoryAccessDenied::MDIServer", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		return;
	}

	bool hasFile = false;

	JXStandAlonePG pg;
	pg.RaiseWhenUpdate();
	if (argCount > 4)
	{
		pg.FixedLengthProcessBeginning(argCount-1, JGetString("OpenFilesProgress::MDIServer"), true, true);
	}

	for (JIndex i=2; i<=argCount; i++)
	{
		const JString& fileName = *(argList.GetElement(i));
		if (fileName.IsEmpty())
		{
			continue;
		}

		bool shouldOpen = true;

		const bool isFile = JFileExists(fileName);
		if (!isFile && JNameUsed(fileName))
		{
			const JUtf8Byte* map[] =
			{
				"name", fileName.GetBytes()
			};
			const JString msg = JGetString("NotAFile::MDIServer", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
			shouldOpen = false;
		}
		else if (!isFile)
		{
			shouldOpen = false;
/*			JString msg = "\"";
			msg += fileName;
			msg += "\" does not exist.  Do you want to create it?";
			if (!JGetUserNotification()->AskUserYes(msg))
			{
				shouldOpen = false;
			}
			else
			{
				std::ofstream temp(fileName);
				if (!temp.good())
				{
					JGetUserNotification()->ReportError("Unable to create it.  "
						"Please check that the directory is writable.");
					shouldOpen = false;
				}
			}*/
		}

		if (shouldOpen)
		{
			hasFile = itsApp->OpenFile(fileName) || hasFile;
		}

		if (pg.ProcessRunning() && !pg.IncrementProgress())
		{
			break;
		}
	}

	if (pg.ProcessRunning())
	{
		pg.ProcessFinished();
	}

	const JError err = JChangeDirectory(origDir);
	assert_ok( err );

	if (!hasFile)
	{
		itsApp->NewFile();
	}
}
