/******************************************************************************
 HistoryDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "HistoryDir.h"
#include "History.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXApplication.h>
#include <jx-af/jx/jXglobals.h>

#include <jx-af/jcore/JChooseSaveFile.h>
#include <jx-af/image/jx/jx_plain_file_small.xpm>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kFileMenuStr =
	"    Save session             %k Meta-S"
	"  | Save session as..."
	"%l| Save session window size"
	"%l| Print                    %k Meta-P"
	"%l| Close                    %k Meta-W"
	"  | Quit                     %k Meta-Q";

enum
{
	kSaveCmd = 1,
	kSaveAsCmd,
	kSaveWindowSizeCmd,
	kPrintCmd,
	kCloseCmd,
	kQuitCmd
};

const JUtf8Byte* HistoryDir::kSessionChanged = "kSessionChanged::HistoryDir";

/******************************************************************************
 Constructor

 ******************************************************************************/

HistoryDir::HistoryDir
	(
	JXDirector* supervisor
	)
	:
	JXDocument(supervisor)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

HistoryDir::~HistoryDir()
{
}

/******************************************************************************
 BuildWindow

	This is a convenient and organized way of putting all of the initial
	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
HistoryDir::BuildWindow()
{
	JCoordinate w = 485;
	JCoordinate h = 320;
	JPoint dtl;
//	bool foundWindowPref = gjdbApp->GetCmdWindowSize(&dtl, &w, &h);
	auto* window = jnew JXWindow(this, w,h, "Glove session");
	assert( window != nullptr );
	window->SetMinSize(300,200);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
//	if (foundWindowPref)
//		{
//		window->Place(dtl.x, dtl.y);
//		}

	itsMenuBar =
		jnew JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
						0,0, w,kJXDefaultMenuBarHeight);
	assert( itsMenuBar != nullptr );

	itsFileMenu = itsMenuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,kJXDefaultMenuBarHeight,
			w,h - kJXDefaultMenuBarHeight);
	assert( scrollbarSet != nullptr );

	itsHistory =
		jnew History(itsMenuBar,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( itsHistory != nullptr );

	itsHistory->FitToEnclosure(true, true);

	ListenTo(itsHistory);

}

/******************************************************************************
 Receive (virtual protected)


 ******************************************************************************/

void
HistoryDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
	}
	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateFileMenu();
	}
	else if (sender == itsHistory && message.Is(JStyledText::kTextChanged))
	{
		Broadcast(SessionChanged());
	}
	else
	{
		JXWindowDirector::Receive(sender,message);
	}
}

/******************************************************************************
 UpdateFileMenu


 ******************************************************************************/

void
HistoryDir::UpdateFileMenu()
{
}

/******************************************************************************
 HandleFileMenu


 ******************************************************************************/

void
HistoryDir::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kSaveWindowSizeCmd)
	{
//		gjdbApp->SaveCmdWindowSize(GetWindow());
	}
	else if (index == kCloseCmd)
	{
//		GetWindow()->Iconify();
		Deactivate();
	}
	else if (index == kQuitCmd)
	{
		JXGetApplication()->Quit();
	}
}

/******************************************************************************
 AppendText


 ******************************************************************************/

void
HistoryDir::AppendText
	(
	const JString&	text,
	const bool	show
	)
{
	const JString& history = itsHistory->GetText()->GetText();
	if (!history.IsEmpty())
	{
		itsHistory->SetCaretLocation(itsHistory->GetText()->GetBeyondEnd().charIndex);
		if (history.GetLastCharacter() != '\n')
		{
			itsHistory->Paste(JString::newline);
		}
	}
	itsHistory->Paste(text);
	if (!IsActive() && show)
	{
		Activate();
	}
}

/******************************************************************************
 Print


 ******************************************************************************/

void
HistoryDir::Print()
{
	itsHistory->PrintPT();
}

/******************************************************************************
 WriteData


 ******************************************************************************/

void
HistoryDir::WriteData
	(
	std::ostream& os
	)
{
	os << itsHistory->GetText()->GetText() << ' ';
}

/******************************************************************************
 ReadData


 ******************************************************************************/

void
HistoryDir::ReadData
	(
	std::istream& is
	)
{
	JString text;
	is >> text;
	itsHistory->Paste(text);
}

/******************************************************************************
 OKToClose

 ******************************************************************************/

bool
HistoryDir::OKToClose()
{
	return true;
}

/******************************************************************************
 OKToRevert

 ******************************************************************************/

bool
HistoryDir::OKToRevert()
{
	return true;
}

/******************************************************************************
 CanRevert

 ******************************************************************************/

bool
HistoryDir::CanRevert()
{
	return true;
}

/******************************************************************************
 NeedsSave

 ******************************************************************************/

bool
HistoryDir::NeedsSave()
	const
{
	return false;
}

/******************************************************************************
 SafetySave

 ******************************************************************************/

void
HistoryDir::SafetySave
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{

}

/******************************************************************************
 DiscardChanges

 ******************************************************************************/

void
HistoryDir::DiscardChanges()
{

}

/******************************************************************************
 GetMenuIcon (virtual)

	Override of JXDocument::GetMenuIcon().

 ******************************************************************************/

JXPM
HistoryDir::GetMenuIcon()
	const
{
	return JXPM(jx_plain_file_small);
}
