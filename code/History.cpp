/******************************************************************************
 History.cpp

	BASE CLASS = JXTEBase

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "History.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXXFontMenu.h>
#include <jx-af/jx/JXFontSizeMenu.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXScrollbar.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXStyledText.h>

#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JPagePrinter.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <stdlib.h>
#include <stdio.h>
#include <jx-af/jcore/jAssert.h>

const JSize kDefTabCharCount = 4;

const JCoordinate kHeaderStartX	= 30;

// Font menu

static const JString kFontRegex("^[0-9]x[0-9]*$");
static const JString kDefaultFontName("6x13");

// JBroadcaster message types

const JUtf8Byte* History::kFontWillChange = "History::FontWillChange";

/******************************************************************************
 Constructor

 ******************************************************************************/

History::History
	(
	JXMenuBar*			menuBar,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kFullEditor,
			 jnew JXStyledText(false, false, enclosure->GetFontManager()), true,
			 false, scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h)
{
	WantInput(true, false);

	scrollbarSet->GetVScrollbar()->SetScrollDelay(0);

	itsTabCharCount = kDefTabCharCount;

	// adjust the edit menu

	AppendEditMenu(menuBar);
	AppendSearchReplaceMenu(menuBar);

	// add the font or size menu

	JRegex regex(kFontRegex);
	if (JXXFontMenu::Create(regex, CompareFontNames, JGetString("FontMenuTitle::JXGlobal"), menuBar,
							kFixedLeft, kFixedTop, 0,0, 10,10, &itsFontMenu))
	{
		itsSizeMenu = nullptr;
		itsFontMenu->SetFontName(kDefaultFontName);
		GetText()->SetDefaultFont(JFontManager::GetDefaultFont());
		menuBar->AppendMenu(itsFontMenu);
		ListenTo(itsFontMenu);
	}
	else
	{
		itsFontMenu = nullptr;
		GetText()->SetDefaultFont(JFontManager::GetDefaultMonospaceFont());

		itsSizeMenu = jnew JXFontSizeMenu(JFontManager::GetDefaultMonospaceFontName(), JGetString("SizeMenuTitle::JXGlobal"), menuBar,
										 kFixedLeft, kFixedTop, 0,0, 10,10);
		menuBar->AppendMenu(itsSizeMenu);
		itsSizeMenu->SetFontSize(JFontManager::GetDefaultFontSize());
		ListenTo(itsSizeMenu);
	}

	AdjustTabWidth();

	SetPTPrinter(GetPTPrinter());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

History::~History()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
History::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if ((sender == itsFontMenu && message.Is(JXFontNameMenu::kNameChanged)) ||
		(sender == itsSizeMenu && message.Is(JXFontSizeMenu::kSizeChanged)))
	{
		AdjustFont();
	}

	else
	{
		JXTEBase::Receive(sender, message);
	}
}

/******************************************************************************
 AdjustFont (private)

 ******************************************************************************/

void
History::AdjustFont()
{
	Broadcast(FontWillChange());	// notify others so save status won't change

	// save selection or caret location

	JCharacterRange selRange;
	JIndex caretIndex;
	const bool hasSelection = HasSelection();
	bool ok;
	if (hasSelection)
	{
		ok = GetSelection(&selRange);
	}
	else
	{
		ok = GetCaretLocation(&caretIndex);
	}
	assert( ok );

	// set font or size for entire text

	SelectAll();

	if (itsFontMenu != nullptr)
	{
		JFont font = JFontManager::GetFont(itsFontMenu->GetFontName());
		SetCurrentFont(font);
		GetText()->SetDefaultFont(font);
	}
	else
	{
		assert( itsSizeMenu != nullptr );
		const JSize size = itsSizeMenu->GetFontSize();
		SetCurrentFontSize(size);
		GetText()->SetDefaultFontSize(size);
	}

	// restore selection or caret location

	if (hasSelection)
	{
		SetSelection(selRange);
	}
	else
	{
		SetCaretLocation(caretIndex);
	}

	// update the tab width

	AdjustTabWidth();
}

/******************************************************************************
 AdjustTabWidth (private)

 ******************************************************************************/

void
History::AdjustTabWidth()
{
	JCoordinate charWidth;

	JFontManager* fontMgr = GetFontManager();
	if (itsFontMenu != nullptr)
	{
		const JFont font = JFontManager::GetFont(itsFontMenu->GetFontName());
		charWidth = font.GetCharWidth(fontMgr, JUtf8Character(' '));
	}
	else
	{
		assert( itsSizeMenu != nullptr );
		const JFont font = JFontManager::GetFont(JFontManager::GetDefaultMonospaceFontName(), itsSizeMenu->GetFontSize());
		charWidth = font.GetCharWidth(fontMgr, JUtf8Character(' '));
	}

	SetDefaultTabWidth(itsTabCharCount * charWidth);
}


/******************************************************************************
 CompareFontNames (static private)

	We sort by the first number, then by the second number: #x##

 ******************************************************************************/

std::weak_ordering
History::CompareFontNames
	(
	JString* const & s1,
	JString* const & s2
	)
{
	const int c1 = s1->GetRawBytes()[0] - '0';
	const int c2 = s2->GetRawBytes()[0] - '0';

	if (c1 < c2)
	{
		return std::weak_ordering::less;
	}
	else if (c1 > c2)
	{
		return std::weak_ordering::greater;
	}

	JUInt x1, x2;
	const bool ok1 = JString::ConvertToUInt(s1->GetBytes() + 2, &x1);
	assert( ok1 );
	const bool ok2 = JString::ConvertToUInt(s2->GetBytes() + 2, &x2);
	assert( ok2 );

	return JIntToWeakOrdering(x1 - x2);
}

/******************************************************************************
 GetPrintHeaderHeight (protected)

 ******************************************************************************/

JCoordinate
History::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return p.GetLineHeight() * 2;
}

/******************************************************************************
 DrawPrintHeader (protected)

 ******************************************************************************/

void
History::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
	JPoint topLeft;
	topLeft.y	= (headerHeight - p.GetLineHeight())/2;
	topLeft.x	= kHeaderStartX;
	p.String(topLeft, JGetString("PrintHeader::History"));
}

/******************************************************************************
 LoadDefaultToolBar (public)

 ******************************************************************************/

void
History::LoadDefaultToolBar
	(
	JXToolBar*		toolBar
	)
{
	JXTextMenu* editMenu;
	GetEditMenu(&editMenu);

	JIndex mIndex;
	bool ok = EditMenuCmdToIndex(kCutCmd, &mIndex);
	assert(ok);

	toolBar->NewGroup();
	toolBar->AppendButton(editMenu, mIndex);

	ok = EditMenuCmdToIndex(kCopyCmd, &mIndex);
	assert(ok);

	toolBar->AppendButton(editMenu, mIndex);

	ok = EditMenuCmdToIndex(kPasteCmd, &mIndex);
	assert(ok);

	toolBar->AppendButton(editMenu, mIndex);
}
