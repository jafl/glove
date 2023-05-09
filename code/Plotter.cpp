/******************************************************************************
 Plotter.cpp

	BASE CLASS = JX2DPlotWidget

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "Plotter.h"
#include "HistoryDir.h"
#include "gloveModule.h"
#include "PlotApp.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXWindowPainter.h>

#include <jx-af/jcore/JProcess.h>
#include <jx-af/jcore/JOutPipeStream.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kModuleMenuStr = "Reload modules %l";

enum
{
	kReloadCursorModuleCmd = 1
};

/******************************************************************************
 Constructor

 ******************************************************************************/

Plotter::Plotter
	(
	HistoryDir*	sessionDir,
	JXMenuBar*			menuBar,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JX2DPlotWidget(menuBar, enclosure, hSizing, vSizing, x, y, w, h)
{
	itsSessionDir = sessionDir;
	JXTextMenu* cursorMenu = GetCursorMenu();
	cursorMenu->ShowSeparatorAfter(cursorMenu->GetItemCount());
	cursorMenu->AppendItem(JGetString("ModuleMenuTitle::Plotter"));

	itsModuleMenu = jnew JXTextMenu(cursorMenu, cursorMenu->GetItemCount(), menuBar);
	assert( itsModuleMenu != nullptr );
	itsModuleMenu->SetMenuItems(kModuleMenuStr);
	itsModuleMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsModuleMenu->AttachHandler(this, &Plotter::HandleModuleMenu);

	itsIsProcessingCursor = false;
	ListenTo(this);
	UpdateModuleMenu();
	itsLink = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Plotter::~Plotter()
{
	if (itsIsProcessingCursor)
	{
		jdelete itsCursorProcess;
		delete itsLink;
	}
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
Plotter::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (message.Is(JProcess::kFinished))
	{
		jdelete itsCursorProcess;
		delete itsLink;
		itsLink = nullptr;
		itsIsProcessingCursor = false;
	}

	else if (sender == itsLink && message.Is(JMessageProtocolT::kMessageReady))
	{
		if (itsLink->HasMessages())
		{
			JSize count = itsLink->GetMessageCount();
			for (JSize i = 1; i <= count; i++)
			{
				JString str;
				if (itsLink->GetNextMessage(&str))
				{
					if (itsCursorFirstPass)
					{
						JUtf8Byte c = str.GetRawBytes()[0];
						int val = c - '0';
						if (val == kGloveFail)
						{
							JStringIterator iter(&str);
							iter.SkipNext(2);
							iter.RemoveAllPrev();
							iter.Invalidate();

							str.Prepend(JGetString("ModuleError::Plotter"));
							JGetUserNotification()->ReportError(str);
						}
						itsCursorFirstPass = false;
					}
					else
					{
						itsSessionDir->AppendText(str);
					}
				}
			}
		}
	}

	else
	{
		JX2DPlotWidget::Receive(sender, message);
	}
}

/*******************************************************************************
 UpdateModuleMenu

 ******************************************************************************/

void
Plotter::UpdateModuleMenu()
{
	const JSize mCount = itsModuleMenu->GetItemCount();
	for (JIndex i = 2; i <= mCount; i++)
	{
		itsModuleMenu->RemoveItem(2);
	}

	GetApplication()->ReloadCursorModules();
	for (auto* name : *GetApplication()->GetCursorModules())
	{
		itsModuleMenu->AppendItem(*name);
	}
}

/*******************************************************************************
 HandleModuleMenu

 ******************************************************************************/

void
Plotter::HandleModuleMenu
	(
	const JIndex index
	)
{
	if (index == 1)
	{
		UpdateModuleMenu();
	}

	else
	{
		JString modName;
		GetApplication()->GetCursorModulePath(index - 1, &modName);
		int inFD;
		int outFD;
		JError err =
			JProcess::Create(&itsCursorProcess, modName,
							kJCreatePipe, &outFD,
							kJCreatePipe, &inFD,
							kJIgnoreConnection, nullptr);
		assert(err.OK());
		auto* op = jnew JOutPipeStream(outFD, true);
		assert( op != nullptr );
		assert( op->good() );

		itsLink = new ProcessLink(inFD);
		assert(itsLink != nullptr);
		ListenTo(itsLink);
		ListenTo(itsCursorProcess);

		itsIsProcessingCursor = true;
		*op << XCursorVisible();
		*op << YCursorVisible();
		*op << DualCursorsVisible() << " ";
		if (XCursorVisible())
		{
//			*op << itsXCursorVal1 << " ";
			if (DualCursorsVisible())
			{
//				*op << itsXCursorVal2 << " ";
			}
		}
		if (YCursorVisible())
		{
//			*op << itsYCursorVal1 << " ";
			if (DualCursorsVisible())
			{
//				*op << itsYCursorVal2 << " ";
			}
		}
		jdelete op;
		itsCursorFirstPass = true;
	}
}
