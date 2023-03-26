/******************************************************************************
 VarTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 2000 by Glenn Bach.

 ******************************************************************************/

#include "VarTable.h"
#include "VarList.h"
#include <jx-af/jexpr/JXExprInput.h>
#include <jx-af/jx/jXConstants.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

const JIndex kUserParmsOffset	= 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

VarTable::VarTable
	(
	VarList*			varList,
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
	JXEditTable(1,1, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	const JSize rowHeight = 2*kVMarginWidth + JFontManager::GetDefaultFont().GetLineHeight(GetFontManager());
	SetDefaultRowHeight(rowHeight);

	itsVarList   = varList;
	itsTextInput = nullptr;

	itsOrigText = jnew JString;
	assert( itsOrigText != nullptr );

	AppendCols(1);

	const JSize count = itsVarList->GetVariableCount() - kUserParmsOffset;
	AppendRows(count);

	FitToEnclosure();	// make sure SetColWidth() won't fail
	ListenTo(this);		// adjust fn col width

	SetColWidth(kNameColumn, GetApertureWidth());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

VarTable::~VarTable()
{
	jdelete itsOrigText;
}

/******************************************************************************
 NewConstant

 ******************************************************************************/

void
VarTable::NewConstant()
{
	if (!EndEditing())
	{
		return;
	}
	JIndex parmIndex	= GetRowCount() + 1;
	JString parm;
	JIndex index;
	do
	{
		parm	= "a" + JString((JUInt64) parmIndex);
		parmIndex ++;
	}
	while (itsVarList->ParseVariableName(parm, &index));

	if (itsVarList->AddVariable(parm, 0))
	{
		AppendRows(1);
		BeginEditing(JPoint(1,GetRowCount()));
	}
}

/******************************************************************************
 RemoveSelectedConstant

 ******************************************************************************/

void
VarTable::RemoveSelectedConstant()
{
	JPoint cell;
	if (GetEditedCell(&cell))
	{
		CancelEditing();
		RemoveRow(cell.y);
		itsVarList->RemoveVariable(cell.y + kUserParmsOffset);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
VarTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button == kJXLeftButton && GetCell(pt, &cell))
	{
		BeginEditing(cell);
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
VarTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && cell == editCell)
	{
		return;
	}

	JRect r = rect;
	r.left += kHMarginWidth;

	const JIndex varIndex = cell.y + kUserParmsOffset;
	p.String(r, itsVarList->GetVariableName(varIndex),
				 JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
VarTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsTextInput == nullptr );

	itsTextInput = jnew JXExprInput(this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsTextInput != nullptr );

	const JIndex varIndex = cell.y + kUserParmsOffset;
	itsTextInput->SetVarName(itsVarList->GetVariableName(varIndex));

	*itsOrigText = itsTextInput->GetVarName();

	itsTextInput->SetIsRequired();
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
VarTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsTextInput != nullptr );

	const JString s = itsTextInput->GetVarName();
	if (s == *itsOrigText)
	{
		return true;
	}
	else if (itsTextInput->InputValid())
	{
		const JIndex varIndex = cell.y + kUserParmsOffset;
		bool ok = itsVarList->SetVariableName(varIndex, s);
		return ok;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
VarTable::PrepareDeleteXInputField()
{
	itsTextInput = nullptr;
}

/******************************************************************************
 Receive (virtual protected)

	We don't have to listen for changes to itsVarList because we are
	the only ones who change it.

 ******************************************************************************/

void
VarTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(kColWidthChanged))
	{
		// do it regardless of which column changed so they can't shrink fn column
		AdjustColWidths();
	}

	JXEditTable::Receive(sender, message);
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
VarTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	AdjustColWidths();
}

/******************************************************************************
 AdjustColWidths (private)

 ******************************************************************************/

void
VarTable::AdjustColWidths()
{
	const JSize apWidth = GetApertureWidth();
	SetColWidth(kNameColumn, apWidth);
}
