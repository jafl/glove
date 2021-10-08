/******************************************************************************
 globals.h

	Copyright (C) 1997-2000 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_globals
#define _H_globals

#include <jx-af/jx/jXglobals.h>

class FitManager;
class PrefsMgr;
class PlotApp;
class JXPTPrinter;
class MDIServer;

FitManager*		GetFitManager();
PrefsMgr*		GetPrefsMgr();
PlotApp*		GetApplication();
JXPTPrinter*	GetPTPrinter();
MDIServer*		GetMDIServer();

bool		InitGlobals(PlotApp* app);
void		DeleteGlobals();

void	BuildColumnMenus(const JUtf8Byte* key, const JSize count, ...);

#endif
