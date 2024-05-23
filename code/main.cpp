/******************************************************************************
 main.cpp

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "PlotApp.h"
#include "MDIServer.h"
#include "PrefsMgr.h"
#include "globals.h"
#include <jx-af/jcore/jCommandLine.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jWebUtil.h>
#include <jx-af/jcore/jAssert.h>

void ParseTextOptions(const int argc, char* argv[]);

void PrintHelp();
void PrintVersion();

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	ParseTextOptions(argc, argv);

	if (!MDIServer::WillBeMDIServer(PlotApp::GetAppSignature(), argc, argv))
	{
		return 0;
	}

	bool displayAbout;
	JString prevVersStr;
	auto* app = jnew PlotApp(&argc, argv, &displayAbout, &prevVersStr);

	JXApplication::StartFiber([argc, argv]()
	{
		GetMDIServer()->HandleCmdLineOptions(argc, argv);
	},
	"main->mdi");

	if (displayAbout)
	{
		app->DisplayAbout(true, prevVersStr);
	}
	else
	{
		JCheckForNewerVersion(GetPrefsMgr(), kVersionCheckID);
	}

	app->Run();
	return 0;
}

/******************************************************************************
 ParseTextOptions

	Handle the command line options that don't require opening an X display.

	Options that don't call exit() should use JXApplication::RemoveCmdLineOption()
	so ParseXOptions won't gag.

 ******************************************************************************/

void
ParseTextOptions
	(
	const int	argc,
	char*		argv[]
	)
{
	long index = 1;
	while (index < argc)
	{
		if (JIsVersionRequest(argv[index]))
		{
			PlotApp::InitStrings();
			PrintVersion();
			exit(0);
		}
		else if (JIsHelpRequest(argv[index]))
		{
			PlotApp::InitStrings();
			PrintHelp();
			exit(0);
		}
		index++;
	}
}

/******************************************************************************
 PrintHelp

 ******************************************************************************/

void
PrintHelp()
{
	const JUtf8Byte* map[] =
	{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
	};
	const JString s = JGetString("CommandLineHelp::global", map, sizeof(map));
	std::cout << std::endl << s << std::endl << std::endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	std::cout << GetVersionStr();
	std::cout << std::endl << std::endl;
}
