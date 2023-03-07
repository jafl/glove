/******************************************************************************
 RowHeaderWidget.cpp

	Maintains a column to match the rows of a JTable.

	BASE CLASS = JXEditTable

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "RowHeaderWidget.h"
#include "RaggedFloatTable.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXScrollbar.h>
#include <jx-af/jx/JXDragPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jx/jXConstants.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jglobals.h>
#include <jx-af/jcore/jAssert.h>

const JSize kCellFrameWidth = kJXDefaultBorderWidth;

/******************************************************************************
 Constructor

 ******************************************************************************/

RowHeaderWidget::RowHeaderWidget
	(
	RaggedFloatTable*	table,
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
	JXRowHeaderWidget(table, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsTable = table;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RowHeaderWidget::~RowHeaderWidget()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

	We provide a default implementation, for convenience.

 ******************************************************************************/

void
RowHeaderWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JXDrawUpFrame(p, rect, kCellFrameWidth);

	const JFont font = JFontManager::GetFont(
		JFontManager::GetDefaultFontName(), 10,
		JFontStyle(true, false, 0, false, JColorManager::GetBlackColor()));
	p.SetFont(font);
	JString str(cell.y, 0);
	p.String(rect, str, JPainter::HAlign::kCenter, JPainter::VAlign::kCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
RowHeaderWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint itsDragCell;
	const bool inDragRegion = InDragRegion(pt, &itsDragCell);

	if (inDragRegion && button == kJXLeftButton)
	{
		JXRowHeaderWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
	}

	else if (modifiers.shift() || button == kJXRightButton)
	{
		itsTable->ExtendSelectionToRow(itsDragCell.y);
	}

	else if (button == kJXLeftButton)
	{
		itsTable->SelectRow(itsDragCell.y);
	}

}
