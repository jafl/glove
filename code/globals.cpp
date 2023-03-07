/******************************************************************************
 globals.h

	Copyright (C) 1997-2000 by Glenn Bach.

 ******************************************************************************/

#include "globals.h"
#include "FitManager.h"
#include "PrefsMgr.h"
#include "MDIServer.h"

#include <jx-af/jx/JXPTPrinter.h>
#include <jx-af/jx/JXTextMenu.h>

#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/jDirUtil.h>
#include <cstdarg>
#include <jx-af/jcore/jAssert.h>

static FitManager*	itsFitManager	= nullptr;
static PrefsMgr*	itsPrefsMgr	= nullptr;
static PlotApp*		itsApplication	= nullptr;
static JXPTPrinter*	itsPrinter		= nullptr;
static MDIServer*	itsMDIServer	= nullptr;

/******************************************************************************
 InitGlobals

 ******************************************************************************/

bool
InitGlobals
	(
	PlotApp* app
	)
{
	itsApplication = app;

	bool isNew;
	itsPrefsMgr = jnew PrefsMgr(&isNew);
	assert(itsPrefsMgr != nullptr);

	JXInitHelp();

	itsPrinter = jnew JXPTPrinter();
	assert(itsPrinter != nullptr);

	itsPrefsMgr->ReadPrinterSetup(itsPrinter);

	itsFitManager = jnew FitManager();
	assert(itsFitManager != nullptr);

	itsMDIServer	= jnew MDIServer(app);
	assert(itsMDIServer != nullptr);

	return isNew;
}

/******************************************************************************
 DeleteGlobals

 ******************************************************************************/

void
DeleteGlobals()
{
	itsPrefsMgr->WritePrinterSetup(itsPrinter);
	jdelete itsPrinter;
	jdelete itsFitManager;
	jdelete itsPrefsMgr;
}

/******************************************************************************
 GetPrefsMgr

 ******************************************************************************/

PrefsMgr*
GetPrefsMgr()
{
	assert(itsPrefsMgr != nullptr);
	return itsPrefsMgr;
}

/******************************************************************************
 ForgetPrefsMgr

	Called when license is not accepted, to avoid writing prefs file.

 ******************************************************************************/

void
ForgetPrefsMgr()
{
	itsPrefsMgr = nullptr;
}

/******************************************************************************
 GetApplication

 ******************************************************************************/

PlotApp*
GetApplication()
{
	assert(itsApplication != nullptr);
	return itsApplication;
}

/******************************************************************************
 GNBGetPTPrinter

 ******************************************************************************/

JXPTPrinter*
GetPTPrinter()
{
	return itsPrinter;
}

/******************************************************************************
 GetFitManager

 ******************************************************************************/

FitManager*
GetFitManager()
{
	assert(itsFitManager != nullptr);
	return itsFitManager;
}

/******************************************************************************
 GetMDIServer

 ******************************************************************************/

MDIServer*
GetMDIServer()
{
	assert(itsMDIServer != nullptr);
	return itsMDIServer;
}

/******************************************************************************
 BuildColumnMenus

 ******************************************************************************/

void
BuildColumnMenus
	(
	const JUtf8Byte*	key,
	const JSize			count,
	...
	)
{
	std::va_list args;
	va_start(args, count);

	for (JUInt64 i=1; i<=count; i++)
	{
		JString str(i);
		const JUtf8Byte* map[] =
		{
			"i", str.GetBytes()
		};
		str = JGetString(key, map, sizeof("map"));

		std::va_list args2;
		va_copy(args2, args);

		while (true)
		{
			JXTextMenu* menu = va_arg(args2, JXTextMenu*);
			if (menu == nullptr)
			{
				break;
			}
			menu->AppendItem(str);
		}

		va_end(args2);
	}

	va_end(args);
}
