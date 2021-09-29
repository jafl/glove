/******************************************************************************
 ParmColHeaderWidget.cpp

	BASE CLASS = public JXColHeaderWidget

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include <ParmColHeaderWidget.h>
#include <jx-af/jcore/JPagePrinter.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ParmColHeaderWidget::ParmColHeaderWidget
	(
	JXTable*			table,
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
}

/******************************************************************************
 Destructor

 *****************************************************************************/

ParmColHeaderWidget::~ParmColHeaderWidget()
{
}

/******************************************************************************
 PrintOnPage (public)

 ******************************************************************************/

void
ParmColHeaderWidget::PrintOnPage
	(
	JPagePrinter& p
	)
{
	TableDraw(p, GetBounds());
}
