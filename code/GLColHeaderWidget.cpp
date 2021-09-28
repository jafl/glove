/******************************************************************************
 GLColHeaderWidget.cpp

	Maintains a row to match the columns of a JTable.

	BASE CLASS = JXEditTable

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "GLColHeaderWidget.h"
#include "GLRaggedFloatTable.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXScrollbar.h>
#include <jx-af/jx/JXDragPainter.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jx/jXConstants.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JSize kCellFrameWidth = kJXDefaultBorderWidth;

/******************************************************************************
 Constructor

 ******************************************************************************/

GLColHeaderWidget::GLColHeaderWidget
	(
	GLRaggedFloatTable*	table,
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
	JXColHeaderWidget(table, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsTable = table;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLColHeaderWidget::~GLColHeaderWidget()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

	We provide a default implementation, for convenience.

 ******************************************************************************/

void
GLColHeaderWidget::TableDrawCell
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
	JString str(cell.x, 0);
	p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GLColHeaderWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	
	if (!GetCell(pt,&cell))
	{
		return;
	}
		
	JPoint itsDragCell;

	const bool inDragRegion = InDragRegion(pt, &itsDragCell);

	if (inDragRegion && button == kJXLeftButton)
	{
		JXColHeaderWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
	}
		
	else if (modifiers.shift() || button == kJXRightButton)
	{
		itsTable->ExtendSelectionToCol(itsDragCell.x);
	}
	
	else if (button == kJXLeftButton)
	{
		itsTable->SelectCol(itsDragCell.x);
	}
}

