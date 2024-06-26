/******************************************************************************
 FitParmsTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "FitParmsTable.h"
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JFontStyle.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kDefColWidth  = 50;
const JCoordinate kHMarginWidth = 5;

/******************************************************************************
 Constructor

 ******************************************************************************/

FitParmsTable::FitParmsTable
	(
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
	JXTable(1,kDefColWidth, scrollbarSet, enclosure,
				hSizing,vSizing, x,y, w,h)
{
	AppendCols(2, kDefColWidth);
	itsCol1 = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsCol2 = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	SetColBorderInfo(0, JColorManager::GetBlackColor());
	SetRowBorderInfo(0, JColorManager::GetBlackColor());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FitParmsTable::~FitParmsTable()
{
	itsCol1->DeleteAll();
	jdelete itsCol1;
	itsCol2->DeleteAll();
	jdelete itsCol2;
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
FitParmsTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JString* str;
	p.SetFont(JFontManager::GetDefaultFont());
	JRect r = rect;
	if (cell.x == 1)
	{
		r.right -= kHMarginWidth;
		str = itsCol1->GetItem(cell.y);
		p.String(r, *str, JPainter::HAlign::kRight, JPainter::VAlign::kCenter);
	}
	else
	{
		r.left += kHMarginWidth;
		str = itsCol2->GetItem(cell.y);
		p.String(r, *str, JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
	}
}

/******************************************************************************
 Clear

 ******************************************************************************/

void
FitParmsTable::Clear()
{
	itsCol1->DeleteAll();
	itsCol2->DeleteAll();
	RemoveAllRows();
	SetColWidth(1, kDefColWidth);
	SetColWidth(2, kDefColWidth);
	TableRefresh();
}


/******************************************************************************
 Append

 ******************************************************************************/

void
FitParmsTable::Append
	(
	const JString& col1,
	const JString& col2
	)
{

	JFontManager* fm = GetFontManager();
	JSize lineHeight = JFontManager::GetDefaultFont().GetLineHeight(fm);
	AppendRows(1, lineHeight + 2);
	auto* str = jnew JString(col1);
	itsCol1->Append(str);
	JSize col1Width = GetColWidth(1);
	JSize strWidth = JFontManager::GetDefaultFont().GetStringWidth(fm, *str);
	if (strWidth + 10 > col1Width)
	{
		SetColWidth(1, strWidth + 10);
	}
	str = jnew JString(col2);
	itsCol2->Append(str);
	JSize col2Width = GetColWidth(2);
	strWidth = JFontManager::GetDefaultFont().GetStringWidth(fm, *str);
	if (strWidth + 10 > col2Width)
	{
		SetColWidth(2, strWidth + 10);
	}
	TableRefresh();
}
