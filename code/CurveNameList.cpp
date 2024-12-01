/******************************************************************************
 CurveNameList.cpp

	BASE CLASS = public JXEditTable

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include "CurveNameList.h"
#include "PlotDirector.h"
#include <jx-af/j2dplot/J2DPlotWidget.h>

#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXInputField.h>

#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;
const JCoordinate kDefColWidth  = 100;

const JUtf8Byte* CurveNameList::kCurveSelected = "kCurveSelected::CurveNameList";

/******************************************************************************
 Constructor

 *****************************************************************************/

CurveNameList::CurveNameList
	(
	PlotDirector*		dir,
	J2DPlotWidget*		plot,
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
	JXEditTable(1, kDefColWidth, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsInput(nullptr),
	itsPlot(plot),
	itsDir(dir)
{
	itsMinColWidth = 1;

	JFontManager* fontMgr = GetFontManager();
	const JSize rowHeight = 2*kVMarginWidth + JFontManager::GetDefaultFont().GetLineHeight(fontMgr);
	SetDefaultRowHeight(rowHeight);

	const JSize count = plot->GetCurveCount();

	itsNameList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);

	AppendRows(count);
	for (JIndex i=1; i<=count; i++)
	{
		auto str = jnew JString(plot->GetCurveName(i));
		itsNameList->Append(str);

		const JCoordinate width = 2*kHMarginWidth +
			JFontManager::GetDefaultFont().GetStringWidth(fontMgr, *str);
		if (width > itsMinColWidth)
		{
			itsMinColWidth = width;
		}
	}

	AppendCols(1);
	AdjustColWidth();

	SetRowBorderInfo(0, JColorManager::GetBlackColor());
	SetColBorderInfo(0, JColorManager::GetBlackColor());

	ListenTo(itsPlot);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CurveNameList::~CurveNameList()
{
	itsNameList->DeleteAll();
	jdelete itsNameList;		// we don't own the strings
}

/******************************************************************************
 HandleMouseDown (protected)

 ******************************************************************************/

void
CurveNameList::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton	button,
	const JSize			clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button == kJXLeftButton && clickCount == 1 &&
		GetCell(pt, &cell) && !itsDir->CurveIsFit(cell.y))
	{
		if (!GetTableSelection().IsSelected(cell))
		{
			SelectSingleCell(cell);
			Broadcast(CurveSelected(cell.y));
		}
		TableRefresh();
	}
	else
	{
		ScrollForWheel(button, modifiers);
	}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
CurveNameList::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	const JString* curveName = itsNameList->GetItem(cell.y);

	JRect r = rect;
	r.left += kHMarginWidth;

	const JColorID color = itsDir->CurveIsFit(cell.y) ?
							JColorManager::GetBlackColor() :
							JColorManager::GetGrayColor(60);

	p.SetFontStyle(JFontStyle(false, false, 0, false, color));
	p.String(r, *curveName, JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
CurveNameList::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	AdjustColWidth();
}

/******************************************************************************
 AdjustColWidth (private)

 ******************************************************************************/

void
CurveNameList::AdjustColWidth()
{
	SetColWidth(1, JMax(itsMinColWidth, GetApertureWidth()));
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
CurveNameList::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectRow(cell.y);
	Broadcast(CurveSelected(cell.y));

	assert(itsInput == nullptr);
	itsInput = jnew JXInputField(this, kFixedLeft, kFixedTop, x, y, w, h);

	itsInput->GetText()->SetText(*(itsNameList->GetItem(cell.y)));
	itsInput->SetIsRequired();
	return itsInput;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
CurveNameList::PrepareDeleteXInputField()
{
	itsInput = nullptr;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
CurveNameList::ExtractInputData
	(
	const JPoint& cell
	)
{
	const JString& name = itsInput->GetText()->GetText();
	if (!name.IsEmpty())
	{
		*(itsNameList->GetItem(cell.y)) = name;
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 HandleKeyPress (virtual public)

 ******************************************************************************/

void
CurveNameList::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	JTableSelection& s = GetTableSelection();
	const bool ok  = s.GetFirstSelectedCell(&cell);
	assert( ok );

	if (c == kJUpArrow)
	{
		cell.y--;
		if (CellValid(cell))
		{
			BeginEditing(cell);
		}
	}
	else if (c == kJDownArrow)
	{
		cell.y++;
		if (CellValid(cell))
		{
			BeginEditing(cell);
		}
	}

	else
	{
		JXEditTable::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CurveNameList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPlot && message.Is(J2DPlotWidget::kCurveAdded))
	{
		auto& info = dynamic_cast<const J2DPlotWidget::CurveAdded&>(message);
		auto str   = jnew JString(itsPlot->GetCurveName(info.GetIndex()));
		AppendRows(1);
		itsNameList->Append(str);

		const JCoordinate width = 2*kHMarginWidth +
			JFontManager::GetDefaultFont().GetStringWidth(GetFontManager(), *str);
		if (width > itsMinColWidth)
		{
			itsMinColWidth = width;
		}
		AdjustColWidth();
		TableRefresh();
	}
	else if (sender == itsPlot && message.Is(J2DPlotWidget::kCurveRemoved))
	{
		auto& info = dynamic_cast<const J2DPlotWidget::CurveRemoved&>(message);
		RemoveRow(info.GetIndex());
		itsNameList->DeleteItem(info.GetIndex());
		TableRefresh();
	}
}

/******************************************************************************
 GetCurrentCurveIndex

 ******************************************************************************/

bool
CurveNameList::GetCurrentCurveIndex
	(
	JIndex* index
	)
{
	JPoint cell;
	if (GetTableSelection().GetSingleSelectedCell(&cell))
	{
		*index	= cell.y;
		return true;
	}
	return false;
}

/******************************************************************************
 SetCurrentCurveIndex

 ******************************************************************************/

void
CurveNameList::SetCurrentCurveIndex
	(
	const JIndex index
	)
{
	if (GetRowCount() > 0)
	{
		JPoint cell(1, index);
		SelectSingleCell(cell);
		Broadcast(CurveSelected(cell.y));
	}
}
