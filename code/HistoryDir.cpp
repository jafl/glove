/******************************************************************************
 HistoryDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "HistoryDir.h"
#include "History.h"

#include <jx-af/jx/JXApplication.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXMenu.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/jXGlobals.h>

#include <jx-af/jcore/jAssert.h>

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

#include "HistoryDir-File.h"

void
HistoryDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 480,320, JGetString("WindowTitle::HIstoryDir::JXLayout"));
	window->SetMinSize(300, 200);

	itsMenuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 480,30);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 480,290);
	assert( scrollbarSet != nullptr );

	itsHistory =
		jnew History(itsMenuBar, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 465,275);

// end JXLayout

	window->SetCloseAction(JXWindow::kDeactivateDirector);

	ListenTo(itsHistory, std::function([this](const JStyledText::TextChanged&)
	{
		Broadcast(SessionChanged());
	}));

	itsFileMenu = itsMenuBar->AppendTextMenu(JGetString("MenuTitle::HistoryDir_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&HistoryDir::UpdateFileMenu,
		&HistoryDir::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);
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
