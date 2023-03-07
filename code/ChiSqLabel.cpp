/******************************************************************************
 ChiSqLabel.cpp

	Draws a box with an up frame, useful for decorative purposes.

	BASE CLASS = JXDecorRect

	Copyright (C) 2000 by Glenn W. Bach.

 ******************************************************************************/

#include <ChiSqLabel.h>

#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jx/jXConstants.h>

#include <jx-af/jcore/jglobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ChiSqLabel::ChiSqLabel
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXDecorRect(enclosure, hSizing, vSizing, x,y, w,h)
{
	SetBorderWidth(kJXDefaultBorderWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ChiSqLabel::~ChiSqLabel()
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
ChiSqLabel::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawDownFrame(p, frame, GetBorderWidth());
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
ChiSqLabel::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXDrawUpFrame(p, GetBounds(), GetBorderWidth());
	p.JPainter::String(0, 0, JGetString("Label::ChiSqLabel"), GetBoundsWidth(), JPainter::HAlign::kCenter, GetBoundsHeight(), JPainter::VAlign::kCenter);
}
