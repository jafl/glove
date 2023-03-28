/******************************************************************************
 RaggedFloatTable.cpp

	Draws a table of numbers stored in a RaggedFloatTableData object.

	BASE CLASS = JXEditTable

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "RaggedFloatTable.h"
#include "RaggedFloatTableData.h"
#include "ColByIncDialog.h"
#include "ColByRangeDialog.h"
#include "DataDocument.h"
#include "TransformFunctionDialog.h"
#include "VarList.h"
#include "DataModule.h"
#include "CreatePlotDialog.h"
#include "CreateVectorPlotDialog.h"
#include "PlotApp.h"
#include "TextSelection.h"
#include "UndoElementChange.h"
#include "UndoElementAppend.h"
#include "UndoElementCut.h"
#include "UndoElementsChange.h"
#include "UndoElementsInsert.h"
#include "UndoElementsCut.h"
#include "globals.h"

#include <jx-af/image/jx/jx_edit_undo.xpm>
#include <jx-af/image/jx/jx_edit_redo.xpm>
#include <jx-af/image/jx/jx_edit_cut.xpm>
#include <jx-af/image/jx/jx_edit_copy.xpm>
#include <jx-af/image/jx/jx_edit_paste.xpm>
#include <jx-af/image/jx/jx_edit_clear.xpm>
#include "plotdata.xpm"
#include "plotvectordata.xpm"
#include "transform.xpm"

#include <jx-af/jx/JXApplication.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXFloatInput.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXSelectionManager.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXChooseFileDialog.h>

#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JFunctionWithVar.h>
#include <jx-af/jexpr/JFunctionWithArgs.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JPSPrinter.h>
#include <jx-af/jcore/JListUtil.h>

#include <jx-af/jx/jXActionDefs.h>

#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <sstream>

#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jAssert.h>

// Setup information

const JFileVersion	kCurrentTableVersion = 0;

const JCoordinate kDefColWidth  = 100;
const JCoordinate kDefRowWidth	= 20;
const JCoordinate kHMarginWidth = 2;

// Edit menu information

static const JUtf8Byte* kEditMenuStr =
	"    Undo               %k Meta-Z %i " kJXUndoAction
	"  | Redo               %k Meta-Shift-Z %i " kJXRedoAction
	"%l| Cut                %k Meta-X %i " kJXCutAction
	"  | Copy               %k Meta-C %i " kJXCopyAction
	"  | Paste              %k Meta-V %i " kJXPasteAction
	"  | Paste at Selection"
	"%l| Insert             %k Meta-I"
	"  | Duplicate          %k Meta-D"
	"  | Delete             %k Meta-K";

enum
{
	kUndoCmd = 1,
	kRedoCmd,
	kCutCmd,
	kCopyCmd,
	kPasteCmd,
	kSpecialPasteCmd,
	kInsertCmd,
	kDuplicateCmd,
	kDeleteCmd
};

// Data menu information


static const JUtf8Byte* kDataMenuStr =
	"    Plot data... %i Plot::RaggedFloatTable"
	"  | Plot vector field...  %i PlotVector::RaggedFloatTable"
	"%l| Transform... %i Transform::RaggedFloatTable"
	"  | Generate column by range..."
	"  | Generate column by increment..."
	"%l| Data module";

enum
{
	kPlotCmd = 1,
	kPlotVectorCmd,
	kTransCmd,
	kGenerateRangeColCmd,
	kGenerateIncrementColCmd,
	kDataModuleCmd
};

enum
{
	kDataPlot = 1,
	kVectorPlot
};

enum
{
	kNewColByRangeCmd = 1,
	kNewColByIncCmd
};

static const JUtf8Byte* kModuleMenuStr =
	"Reload %l";

enum
{
	kReloadModuleCmd = 1
};

// Selection Targets for Cut/Copy/Paste

static const JUtf8Byte* kGloveTextDataXAtomName = "OVE_TEXT";

/******************************************************************************
 Constructor

 ******************************************************************************/

RaggedFloatTable::RaggedFloatTable
	(
	DataDocument*			dir,
	JXTextButton*			okButton,
	RaggedFloatTableData*	data,
	const int				precision,
	JXMenuBar*				menuBar,
	JXScrollbarSet*			scrollbarSet,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXEditTable(kDefRowWidth,kDefColWidth, scrollbarSet, enclosure,
				hSizing,vSizing, x,y, w,h),
	itsOKButton(okButton)
{
	itsTableDir = dir;
	assert( data != nullptr );

	ListenTo(itsOKButton);
	itsOKButton->Deactivate();

	itsFloatData = data;

	itsFloatInputField = nullptr;

	itsEditMenu = menuBar->AppendTextMenu(JGetString("EditMenuTitle::JXGlobal"));
	itsEditMenu->SetMenuItems(kEditMenuStr);
	itsEditMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsEditMenu);

	itsEditMenu->SetItemImage(kUndoCmd,   jx_edit_undo);
	itsEditMenu->SetItemImage(kRedoCmd,   jx_edit_redo);
	itsEditMenu->SetItemImage(kCutCmd,    jx_edit_cut);
	itsEditMenu->SetItemImage(kCopyCmd,   jx_edit_copy);
	itsEditMenu->SetItemImage(kPasteCmd,  jx_edit_paste);
	itsEditMenu->SetItemImage(kDeleteCmd, jx_edit_clear);

	itsDataMenu = menuBar->AppendTextMenu(JGetString("DataMenuTitle::RaggedFloatTable"));
	itsDataMenu->SetMenuItems(kDataMenuStr);
	itsDataMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDataMenu);

	itsDataMenu->SetItemImage(kPlotCmd, plotdata);
	itsDataMenu->SetItemImage(kPlotVectorCmd, plotvectordata);
	itsDataMenu->SetItemImage(kTransCmd, glv_transform);

	itsModuleMenu = jnew JXTextMenu(itsDataMenu, kDataModuleCmd, menuBar);
	assert( itsModuleMenu != nullptr );
	itsModuleMenu->SetMenuItems(kModuleMenuStr);
	itsModuleMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsModuleMenu);

	itsFirstRedoIndex	= 1;
	itsUndoState		= kIdle;

	itsUndoList = jnew JPtrArray<JUndo>(JPtrArrayT::kDeleteAll);
	assert(itsUndoList != nullptr);

	SetTableData(itsFloatData);

	JTableSelection& selection = GetTableSelection();
	while (selection.GetColCount() < GetColCount())
	{
		selection.InsertCols(1, 1);
	}
	while (selection.GetRowCount() < GetRowCount())
	{
		selection.InsertRows(1, 1);
	}

	UpdateModuleMenu();

	itsGloveTextXAtom = GetDisplay()->RegisterXAtom(kGloveTextDataXAtomName);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RaggedFloatTable::~RaggedFloatTable()
{
	jdelete itsEditMenu;
	jdelete itsDataMenu;

	itsUndoList->DeleteAll();
	jdelete itsUndoList;
}

/******************************************************************************
 LoadDefaultToolButtons (public)

 ******************************************************************************/

void
RaggedFloatTable::LoadDefaultToolButtons
	(
	JXToolBar* toolbar
	)
{
	toolbar->NewGroup();
	toolbar->AppendButton(itsEditMenu, kCutCmd);
	toolbar->AppendButton(itsEditMenu, kCopyCmd);
	toolbar->AppendButton(itsEditMenu, kPasteCmd);
	toolbar->NewGroup();
	toolbar->AppendButton(itsDataMenu, kPlotCmd);
	toolbar->AppendButton(itsDataMenu, kPlotVectorCmd);
	toolbar->NewGroup();
	toolbar->AppendButton(itsDataMenu, kTransCmd);
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
RaggedFloatTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	if ((GetTableSelection()).IsSelected(cell))
	{
		p.SetFilling(true);
		p.SetPenColor(JColorManager::GetDefaultSelectionColor());
		p.Rect(rect);
		p.SetFilling(false);
		p.SetPenColor(JColorManager::GetBlackColor());
	}

	JPoint editCell;
	if (!GetEditedCell(&editCell) || cell != editCell)
	{
		p.SetFont(JFontManager::GetDefaultFont());

/* Original code that used the string buffer */
//		if (itsFloatBufferData->GetElement(cell, &str))
//			{
//			JRect r = rect;
//			r.left += kHMarginWidth;
//			p.String(r, str, JPainter::HAlign::kRight, JPainter::VAlign::kCenter);
//			}
		JFloat value;
		if (itsFloatData->GetElement(cell, &value))
		{
			JRect r = rect;
			r.left += kHMarginWidth;
			JString str(value, 6);
			p.String(r, str, JPainter::HAlign::kRight, JPainter::VAlign::kCenter);
		}
	}
}

/******************************************************************************
 HandleMouseDown

 ******************************************************************************/

void
RaggedFloatTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button > kJXRightButton)
	{
		ScrollForWheel(button, modifiers);
		return;
	}
	JTableSelection& s = GetTableSelection();

	JPoint cell;
	if (!GetCell(pt, &cell))
	{
		s.ClearSelection();
		TableRefresh();
		return;
	}

	const JPoint newBoat   = cell;
	const JPoint newAnchor = cell;

	const bool extendSelection = modifiers.shift();

	if (button == kJXLeftButton && clickCount == 2)
	{
		s.ClearSelection();
		s.SetBoat(newBoat);
		s.SetAnchor(newAnchor);
		BeginEditing(cell);
	}

	if (clickCount == 1)
	{
		if ((button == kJXLeftButton && extendSelection) || button == kJXRightButton)
		{
			if (s.OKToExtendSelection())
			{
				s.ExtendSelection(newBoat);
			}
		}
		else if (button == kJXLeftButton)
		{
			s.ClearSelection();
			s.SetBoat(newBoat);
			s.SetAnchor(newAnchor);
			s.SelectCell(cell);
		}
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
RaggedFloatTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ScrollForDrag(pt);
	JTableSelection& s = GetTableSelection();

	JPoint cell;
	const bool ok = GetCell(JPinInRect(pt, GetBounds()), &cell);
	assert( ok );
	if (cell == s.GetBoat())
	{
		return;
	}
	const JPoint newBoat   = cell;

	s.ExtendSelection(newBoat);

	GetWindow()->Update();
}

/******************************************************************************
 HandleKeyPress

 ******************************************************************************/

void
RaggedFloatTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJEscapeKey)
	{
		(GetTableSelection()).ClearSelection();
		TableRefresh();
	}
	JPoint cell;
	JTableSelection& selection = GetTableSelection();
	if (!GetEditedCell(&cell) && selection.HasSelection())
	{
		JTableSelectionIterator iter(&selection);
		iter.Next(&cell);

		if (c == kJReturnKey || keySym == XK_KP_Enter)
		{
			selection.ClearSelection();
			BeginEditing(cell);
		}
		else if (c == kJUpArrow)
		{
			if (cell.y > 1)
			{
				if (modifiers.meta())
				{
					cell.y = 1;
				}
				else
				{
					cell.y--;
				}
				selection.ClearSelection();
				selection.SelectCell(cell);
				TableScrollToCell(cell);
			}
		}

		else if (c == kJDownArrow)
		{
			if (cell.y < (JCoordinate)GetRowCount())
			{
				if (modifiers.meta())
				{
					if (itsFloatData->ColIndexValid(cell.x))
					{
						JSize row = itsFloatData->GetDataRowCount(cell.x) + 1;
						if (row > GetRowCount())
						{
							row = GetRowCount();
						}
						cell.y = row;
					}
				}
				else
				{
					cell.y++;
				}
				selection.ClearSelection();
				selection.SelectCell(cell);
				TableScrollToCell(cell);
			}
		}
		else if (c == kJLeftArrow)
		{
			if (cell.x > 1)
			{
				if (modifiers.meta())
				{
					cell.x = 1;
				}
				else
				{
					cell.x--;
				}
				selection.ClearSelection();
				selection.SelectCell(cell);
				TableScrollToCell(cell);
			}
		}
		else if (c == kJRightArrow)
		{
			if (cell.x < (JCoordinate)GetColCount())
			{
				if (modifiers.meta())
				{
					JSize col = itsFloatData->GetDataColCount() + 1;
					if (col > GetColCount())
					{
						col = GetColCount();
					}
					cell.x = col;
				}
				else
				{
					cell.x++;
				}
				selection.ClearSelection();
				selection.SelectCell(cell);
				TableScrollToCell(cell);
			}
		}
		else
		{
			JXEditTable::HandleKeyPress(c, keySym, modifiers);
		}
	}
	else
	{
		JXEditTable::HandleKeyPress(c, keySym, modifiers);
	}

}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
RaggedFloatTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsFloatInputField == nullptr );
	itsOKButton->Activate();

	itsFloatInputField =
		jnew JXFloatInput(this, kFixedLeft, kFixedTop, x,y, w,h);
	assert( itsFloatInputField != nullptr );

	JFloat value;
	if (itsFloatData->GetElement(cell, &value))
	{
		itsFloatInputField->SetValue(value);
	}
	return itsFloatInputField;
}

/******************************************************************************
 ExtractInputData (virtual protected)

	Extract the information from the active input field, check it,
	and delete the input field if successful.

	Returns true if the data is valid and the process succeeded.

 ******************************************************************************/

bool
RaggedFloatTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsFloatInputField != nullptr );

	if (itsFloatInputField->InputValid())
	{
		JFloat value;
		const bool valid = itsFloatInputField->GetValue(&value);
		assert( valid );

		// save old value for undo
		JFloat oldvalue;
		bool exists = itsFloatData->GetElement(cell, &oldvalue);

		// set to new value
		itsFloatData->SetElement(cell, value);

		// create and install undo object with old value
		if (exists)
		{
			auto* undo =
				jnew UndoElementChange(this, cell, oldvalue);
			assert(undo != nullptr);
			NewUndo(undo);
		}
		else
		{
			auto* undo =
				jnew UndoElementAppend(this, cell);
			assert(undo != nullptr);
			NewUndo(undo);
		}

		return true;
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
RaggedFloatTable::PrepareDeleteXInputField()
{
	itsFloatInputField = nullptr;
	itsOKButton->Deactivate();
}

/******************************************************************************
 ExtendSelection

 ******************************************************************************/

void
RaggedFloatTable::ExtendSelection
	(
	const JPoint cell
	)
{
	auto* iter =
		jnew JTableSelectionIterator(&(GetTableSelection()));
	assert (iter != nullptr);

	JPoint sCell;
	bool success = iter->Next(&sCell);
	if (!success)
	{
		jdelete iter;
		return;
	}
	JIndex sCol;
	JIndex eCol;
	JIndex sRow;
	JIndex eRow;

	if (cell.x > sCell.x)
	{
		sCol = sCell.x;
		eCol = cell.x;
	}
	else
	{
		eCol = sCell.x;
		sCol = cell.x;
	}

	if (cell.y > sCell.y)
	{
		sRow = sCell.y;
		eRow = cell.y;
	}
	else
	{
		eRow = sCell.y;
		sRow = cell.y;
	}

	(GetTableSelection()).ClearSelection();
	for (JIndex i = sCol; i <= eCol; i++)
	{
		for (JIndex j = sRow; j <= eRow; j++)
		{
			(GetTableSelection()).SelectCell(j,i);
		}
	}

	jdelete iter;
	TableRefresh();
}

/******************************************************************************
 SelectCell

 ******************************************************************************/

void
RaggedFloatTable::SelectCell
	(
	const JPoint cell
	)
{
	(GetTableSelection()).ClearSelection();
	(GetTableSelection()).SelectCell(cell);
	TableRefresh();
}

/******************************************************************************
 SelectRow

 ******************************************************************************/

void
RaggedFloatTable::SelectRow
	(
	const JIndex row
	)
{
	const JSize colCount = GetColCount();
	(GetTableSelection()).ClearSelection();

	for (JSize i = 1; i <= colCount; i++)
	{
		(GetTableSelection()).SelectCell(row, i);
	}
	TableRefresh();
}

/******************************************************************************
 AddRowToSelection

 ******************************************************************************/

void
RaggedFloatTable::AddRowToSelection
	(
	const JIndex row
	)
{

}

/******************************************************************************
 ExtendSelectionToRow

 ******************************************************************************/

void
RaggedFloatTable::ExtendSelectionToRow
	(
	const JIndex row
	)
{
	auto* iter =
		jnew JTableSelectionIterator(&(GetTableSelection()));
	assert (iter != nullptr);

	JPoint sCell;
	bool success = iter->Next(&sCell);
	if (!success)
	{
		jdelete iter;
		return;
	}
	JSize sRow = sCell.y;
	JSize eRow = row;

	if (sRow > eRow)
	{
		sRow = row;
		eRow = sCell.y;
	}

	const JSize colCount = GetColCount();
	(GetTableSelection()).ClearSelection();
	for (JSize rowI = sRow; rowI <= eRow; rowI ++)
	{
		for (JSize col = 1; col <= colCount; col++)
		{
			(GetTableSelection()).SelectCell(rowI, col);
		}
	}

	jdelete iter;
	TableRefresh();
}

/******************************************************************************
 SelectCol

 ******************************************************************************/

void
RaggedFloatTable::SelectCol
	(
	const JIndex col
	)
{
	JTableSelection& selection = GetTableSelection();
	selection.ClearSelection();
	selection.SelectCol(col);

//	for (JSize row = 1; row <= rowCount; row++)
//		{
//		(GetTableSelection()).SelectCell(row, col);
//		}
	TableRefresh();
}

/******************************************************************************
 AddColToSelection

 ******************************************************************************/

void
RaggedFloatTable::AddColToSelection
	(
	const JIndex col
	)
{
	JTableSelection& selection = GetTableSelection();
	selection.SelectCol(col);
}

/******************************************************************************
 ExtendSelectionToCol

 ******************************************************************************/

void
RaggedFloatTable::ExtendSelectionToCol
	(
	const JIndex col
	)
{
	JTableSelection& selection = GetTableSelection();
	JTableSelectionIterator iter(&selection);

	JPoint sCell;
	bool success = iter.Next(&sCell);
	if (!success)
	{
		return;
	}

	JSize sCol = sCell.x;
	JSize eCol = col;

	if (sCol > eCol)
	{
		sCol = col;
		eCol = sCell.x;
	}

	selection.ClearSelection();

	for (JSize colI = sCol; colI <= eCol; colI ++)
	{
		selection.SelectCol(colI);
//		for (JSize row = 1; row <= rowCount; row++)
//			{
//			(GetTableSelection()).SelectCell(row, colI);
//			}
	}

	TableRefresh();
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
RaggedFloatTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleEditMenu(selection->GetIndex());
	}

	else if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateEditMenu();
	}

	else if (sender == itsDataMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleDataMenu(selection->GetIndex());
	}

	else if (sender == itsDataMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateDataMenu();
	}

	else if (sender == itsModuleMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleModuleMenu(selection->GetIndex());
	}

	else if (sender == itsOKButton && message.Is(JXButton::kPushed))
	{
		EndEditing();
	}

	else if (sender == itsFloatData && message.Is(RaggedFloatTableData::kElementRemoved))
	{
		const auto* info =
			dynamic_cast<const RaggedFloatTableData::ElementRemoved*>(&message);
		assert( info != nullptr );
		TableRefreshCol(info->GetCol());
	}
	else if (sender == itsFloatData && message.Is(RaggedFloatTableData::kElementInserted))
	{
		const auto* info =
			dynamic_cast<const RaggedFloatTableData::ElementRemoved*>(&message);
		assert( info != nullptr );
		TableRefreshCol(info->GetCol());
	}
	else if (sender == itsFloatData && message.Is(RaggedFloatTableData::kDataChanged))
	{
		AdjustToTableData();
		Refresh();
	}

	else
	{
		JXEditTable::Receive(sender, message);
	}
}

/******************************************************************************
 HandleEditMenu

 ******************************************************************************/

void
RaggedFloatTable::HandleEditMenu
	(
	const JIndex index
	)
{
	if (index == kUndoCmd)
	{
		Undo();
	}
	else if (index == kRedoCmd)
	{
		Redo();
	}
	else if (index == kCutCmd)
	{
		HandleCutCmd();
	}

	else if (index == kCopyCmd)
	{
		HandleCopyCmd();
	}

	else if (index ==kPasteCmd)
	{
		HandlePasteCmd();
	}

	else if (index == kSpecialPasteCmd)
	{
		HandleSpecialPasteCmd();
	}

	else if (index == kInsertCmd)
	{
		HandleInsertion();
	}

	else if (index == kDuplicateCmd)
	{
		HandleDuplication();
	}

	else if (index == kDeleteCmd)
	{
		HandleDeletion();
	}
}

/******************************************************************************
 UpdateEditMenu

 ******************************************************************************/

void
RaggedFloatTable::UpdateEditMenu()
{
	if (HasUndo())
	{
		itsEditMenu->EnableItem(kUndoCmd);
	}
	else
	{
		itsEditMenu->DisableItem(kUndoCmd);
	}
	if (HasRedo())
	{
		itsEditMenu->EnableItem(kRedoCmd);
	}
	else
	{
		itsEditMenu->DisableItem(kRedoCmd);
	}

	SelectionType type = GetSelectionType();

	if (type == kNoneSelected)
	{
		itsEditMenu->DisableItem(kCutCmd);
		itsEditMenu->DisableItem(kCopyCmd);
		itsEditMenu->DisableItem(kInsertCmd);
		itsEditMenu->DisableItem(kDuplicateCmd);
		itsEditMenu->DisableItem(kDeleteCmd);
	}

	else
	{
		itsEditMenu->EnableItem(kCutCmd);
		itsEditMenu->EnableItem(kCopyCmd);
		itsEditMenu->EnableItem(kInsertCmd);
		itsEditMenu->EnableItem(kDuplicateCmd);
		itsEditMenu->EnableItem(kDeleteCmd);
	}

/*	const JXWindow* window = GetWindow();

	bool hasGloveData = false;
	JXSelectionManager* selManager = GetSelectionManager();

	JArray<Atom> typeList;
	if (selManager->GetAvailableTypes(kJXClipboardName, window, &typeList))
	{
		const JSize typeCount = typeList.GetElementCount();
		for (JIndex i=1; i<=typeCount; i++)
		{
			Atom type = typeList.GetElement(i);
			if (type == itsGloveTextXAtom)
			{
				hasGloveData = true;
				break;
			}
		}
	}

		if (hasGloveData)
		{
			itsEditMenu->EnableItem(kPasteCmd);
		}
		else
		{
			itsEditMenu->DisableItem(kPasteCmd);
		}*/
}

/******************************************************************************
 HandleCutCmd

 ******************************************************************************/

void
RaggedFloatTable::HandleCutCmd()
{
	HandleCopyCmd();
	HandleDeletion();
}

/******************************************************************************
 HandleCopyCmd

 ******************************************************************************/

void
RaggedFloatTable::HandleCopyCmd()
{
	SelectionType type = GetSelectionType();

	if (type == kNoneSelected)
	{
		return;
	}

	std::ostringstream os;

	JIndex rows;
	JIndex cols;
	JIndex startRow;
	JIndex startCol;

	GetSelectionArea(&rows, &cols, &startRow, &startCol);

	if (startCol + cols == GetColCount() + 1)
	{
		cols--;
	}

	if (startRow + rows == GetRowCount() + 1)
	{
		rows--;
	}

	for (JIndex row = startRow; row < startRow + rows; row++)
	{
		for (JIndex col = startCol; col < startCol + cols; col++)
		{
			JFloat value;
			if (itsFloatData->GetElement(row, col, &value))
			{
				os << value;
			}
			if (col != startCol + cols - 1)
			{
				os << "\t";
			}
		}
		os << std::endl;
	}

	auto* data = jnew TextSelection(GetDisplay(), os.str());
	assert(data != nullptr);

	std::ostringstream os2;
	os2 << (int)type << " ";
	os2 << cols << " ";
	for (JSize i = startCol; i < startCol + cols; i++)
	{
		JSize rowCount = itsFloatData->GetDataRowCount(i);
		if (rowCount - startRow + 1 < rows)
		{
			os2 << rowCount - startRow + 1 << " ";
		}
		else
		{
			os2 << rows << " ";
		}
		for (JIndex row = startRow; row < startRow + rows; row++)
		{
			JFloat value;
			if (itsFloatData->GetElement(row, i, &value))
			{
				os2 << value << " ";
			}
		}
	}

	data->SetGloveData(os2.str());

	if (!GetSelectionManager()->SetData(kJXClipboardName, data))
	{
		JGetUserNotification()->ReportError(JGetString("CopyFailed::RaggedFloatTable"));
	}
}

/******************************************************************************
 HandlePasteCmd

 ******************************************************************************/

void
RaggedFloatTable::HandlePasteCmd()
{
	bool hasGloveData = false;
	JXSelectionManager* selManager = GetSelectionManager();

	JArray<Atom> typeList;
	if (selManager->GetAvailableTypes(kJXClipboardName,
					CurrentTime, &typeList))
	{

		const JSize typeCount = typeList.GetElementCount();
		for (JIndex i=1; i<=typeCount; i++)
		{
			Atom type = typeList.GetElement(i);
			if (type == itsGloveTextXAtom)
			{
				hasGloveData = true;
				break;
			}
		}

		if (!hasGloveData)
		{
			return;
		}

		unsigned char* data = nullptr;
		JSize dataLength;
		Atom returnType;
		JXSelectionManager::DeleteMethod dMethod;
		if (selManager->GetData(kJXClipboardName,
				CurrentTime, itsGloveTextXAtom,
				&returnType, &data, &dataLength, &dMethod))
		{
			std::string s(reinterpret_cast<char*>(data), dataLength);
			std::istringstream is(s);
			JIndex type;
			JSize rows;
			JSize cols;
			is >> type;
			is >> cols;
			is >> rows;

			JFloat value;
			SelectionType type1 = GetSelectionType();

			JIndex rows1;
			JIndex cols1;
			JIndex startRow;
			JIndex startCol;

			GetSelectionArea(&rows1, &cols1, &startRow, &startCol);

			JSize realrowcount;
			bool ok = false;
			if ((type1 == kElementsSelected) &&
				(cols1 == 1) &&
				((rows1 == 1) || (rows == rows1) ))
			{
				realrowcount = itsFloatData->GetDataRowCount(startCol);
				if (startRow <= realrowcount + 1)
				{
					ok = true;
				}
			}
			if (ok)
			{
				if (rows1 == 1)
				{
					if (rows == 1)
					{
						is >> value;
						JPoint cell(startCol, startRow);
						JFloat oldvalue;
						if (itsFloatData->GetElement(cell, &oldvalue))
						{
							auto* undo =
								jnew UndoElementChange(this, cell, oldvalue);
							assert(undo != nullptr);
							itsFloatData->SetElement(cell, value);
							NewUndo(undo);
						}
						else
						{
							auto* undo =
								jnew UndoElementAppend(this, cell);
							assert(undo != nullptr);
							itsFloatData->InsertElement(cell, value);
							NewUndo(undo);
						}
					}
					else
					{
						auto* undo =
							jnew UndoElementsInsert(this, JPoint(startCol, startRow),
													 JPoint(startCol, startRow + rows - 1),
													 UndoElementsBase::kElements);
						assert(undo != nullptr);
						NewUndo(undo);
						for (JIndex row = startRow; row <= startRow + rows - 1; row++)
						{
							is >> value;
							itsFloatData->InsertElement(row,startCol, value);
						}
					}

				}
				else
				{
					auto* undo =
						jnew UndoElementsChange(this, JPoint(startCol, startRow),
												 JPoint(startCol + cols1 - 1, startRow + rows1 - 1),
												 UndoElementsBase::kElements);
					assert(undo != nullptr);
					NewUndo(undo);
					for (JIndex col = startCol; col < startCol + cols1; col++)
					{
						for (JIndex row = startRow; row <= startRow + rows1 - 1; row++)
						{
							is >> value;
							itsFloatData->SetElement(row,col, value);
						}
					}
				}
			}

			else if (type1 == kNoneSelected ||
				type1 == kRowsSelected ||
				type1 == kElementsSelected)
			{
				const JSize count = itsFloatData->GetDataColCount() + 1;
				auto* undo =
					jnew UndoElementsInsert(this, JPoint(count, 1),
											 JPoint(count + cols - 1, 1),
											 UndoElementsBase::kCols);
				assert(undo != nullptr);
				NewUndo(undo);
				itsFloatData->InsertCols(count, cols);
				for (JSize i = 1; i <= cols; i++)
				{
					for (JSize j = 1; j <= rows; j++)
					{
						is >> value;
						itsFloatData->SetElement(j, i+count-1, value);
					}
				}
			}

			else
			{
				auto* undo =
					jnew UndoElementsInsert(this, JPoint(startCol, 1),
											 JPoint(startCol + cols - 1, 1),
											 UndoElementsBase::kCols);
				assert(undo != nullptr);
				NewUndo(undo);
				itsFloatData->InsertCols(startCol, cols);
				for (JSize i = 1; i <= cols; i++)
				{
					for (JSize j = 1; j <= rows; j++)
					{
						is >> value;
						if (is.good())
						{
							itsFloatData->SetElement(j, i+startCol-1, value);
						}
					}
				}

			}
			TableRefresh();

			selManager->DeleteData(&data, dMethod);
		}
	}
}

/******************************************************************************
 HandleSpecialPasteCmd

 ******************************************************************************/

void
RaggedFloatTable::HandleSpecialPasteCmd()
{
	bool hasGloveData = false;
	JXSelectionManager* selManager = GetSelectionManager();

	JArray<Atom> typeList;
	if (selManager->GetAvailableTypes(kJXClipboardName,
					CurrentTime, &typeList))
	{

		const JSize typeCount = typeList.GetElementCount();
		for (JIndex i=1; i<=typeCount; i++)
		{
			Atom type = typeList.GetElement(i);
			if (type == itsGloveTextXAtom)
			{
				hasGloveData = true;
				break;
			}
		}

		if (!hasGloveData)
		{
			return;
		}

		unsigned char* data = nullptr;
		JSize dataLength;
		Atom returnType;
		JXSelectionManager::DeleteMethod dMethod;
		if (selManager->GetData(kJXClipboardName,
				CurrentTime, itsGloveTextXAtom,
				 &returnType, &data, &dataLength, &dMethod))
		{
			std::string s(reinterpret_cast<char*>(data), dataLength);
			std::istringstream is(s);
			JIndex type;
			JSize rows, cols;
			is >> type;
			is >> cols;

			JFloat value;
			SelectionType type1 = GetSelectionType();

			JIndex rows1;
			JIndex cols1;
			JIndex startRow;
			JIndex startCol;

			GetSelectionArea(&rows1, &cols1, &startRow, &startCol);

			if (type1 == kNoneSelected)
			{
				JGetUserNotification()->ReportError(JGetString("NoSelection::RaggedFloatTable"));
			}

			else if (type1 == kRowsSelected)
			{
				if (cols == GetColCount() - 1)
				{
					for (JSize i = 1; i <= cols; i++)
					{
						is >> rows;
						for (JSize j = 1; j <= rows; j++)
						{
							is >> value;
							itsFloatData->InsertElement(j + startRow - 1, i, value);
						}
					}
				}
				else
				{
					JGetUserNotification()->ReportError(JGetString("SelectionMismatch::RaggedFloatTable"));
				}
			}

			else if (type1 == kElementsSelected)
			{
				if (cols == cols1)
				{
					for (JSize i = 1; i <= cols; i++)
					{
						is >> rows;
						for (JSize j = 1; j <= rows; j++)
						{
							is >> value;
							itsFloatData->InsertElement(j + startRow - 1, i + startCol - 1, value);
						}
					}
				}
				else
				{
					JGetUserNotification()->ReportError(JGetString("SelectionMismatch::RaggedFloatTable"));
				}
			}

			else
			{

				itsFloatData->InsertCols(startCol, cols);
				for (JSize i = 1; i <= cols; i++)
				{
					is >> rows;
					for (JSize j = 1; j <= rows; j++)
					{
						is >> value;
						if (is.good())
						{
							itsFloatData->SetElement(j, i+startCol-1, value);
						}
					}
				}

			}
			TableRefresh();

			selManager->DeleteData(&data, dMethod);
		}
	}
}


/******************************************************************************
 HandleInsertion

 ******************************************************************************/

void
RaggedFloatTable::HandleInsertion
	(
	const bool undo
	)
{
	SelectionType type = GetSelectionType();

	if (type == kNoneSelected)
	{
		return;
	}

	JIndex rows;
	JIndex cols;
	JIndex startRow;
	JIndex startCol;

	GetSelectionArea(&rows, &cols, &startRow, &startCol);

	if (type == kRowsSelected)
	{
		if (undo)
		{
			auto* undo1 =
				jnew UndoElementsInsert(this, JPoint(1, startRow),
										 JPoint(itsFloatData->GetDataColCount(), startRow + rows - 1),
										 UndoElementsBase::kRows);
			assert(undo1 != nullptr);
			NewUndo(undo1);
		}
		itsFloatData->InsertRows(startRow, rows);
	}

	else if (type == kColsSelected)
	{
		if (undo)
		{
			auto* undo1 =
				jnew UndoElementsInsert(this, JPoint(startCol, 1),
										 JPoint(startCol + cols - 1, GetRowCount()),
										 UndoElementsBase::kCols);
			assert(undo1 != nullptr);
			NewUndo(undo1);
		}
		itsFloatData->InsertCols(startCol, cols);
	}

	else if (type == kElementsSelected)
	{
		if (cols == 1 && rows == 1 && undo)
		{
			JPoint cell(startCol, startRow);
			auto* undo1 =
				jnew UndoElementAppend(this, cell);
			assert(undo1 != nullptr);
			NewUndo(undo1);
		}
		else if (undo)
		{
			auto* undo1 =
				jnew UndoElementsInsert(this, JPoint(startCol, startRow),
										 JPoint(startCol + cols - 1, startRow + rows - 1),
										 UndoElementsBase::kElements);
			assert(undo1 != nullptr);
			NewUndo(undo1);
		}

		for (JIndex col = startCol; col < startCol + cols; col++)
		{
			JIndex end = JMin(startRow + rows - 1, itsFloatData->GetDataRowCount(col));
			for (JIndex row = startRow; row <= end; row++)
			{
				itsFloatData->InsertElement(row,col, 0);
			}
		}
	}
	TableRefresh();
}

/******************************************************************************
 HandleDuplication

 ******************************************************************************/

void
RaggedFloatTable::HandleDuplication()
{
	SelectionType type = GetSelectionType();

	if (type == kNoneSelected)
	{
		return;
	}

	JIndex rows;
	JIndex cols;
	JIndex startRow;
	JIndex startCol;

	GetSelectionArea(&rows, &cols, &startRow, &startCol);
	HandleInsertion();

	if ( (type == kRowsSelected) || (type == kElementsSelected) )
	{
//		if ((cols == 1) && (rows == 1))
//			{
//			JPoint cell(startCol, startRow + 1);
//			UndoElementAppend* undo =
//				jnew UndoElementAppend(this, cell);
//			assert(undo != nullptr);
//			NewUndo(undo);
//			}
		for (JIndex col = startCol; col < startCol + cols; col++)
		{
			for (JIndex row = startRow; row < startRow + rows; row++)
			{
				JFloat value;
				if (itsFloatData->GetElement(row+rows, col, &value))
				{
					itsFloatData->SetElement(row,col, value);
				}
			}
		}
	}

	else if (type == kColsSelected)
	{
		for (JIndex col = startCol; col < startCol + cols; col++)
		{
			for (JIndex row = startRow; row < startRow + rows; row++)
			{
				JFloat value;
				if (itsFloatData->GetElement(row, col+cols, &value))
				{
					itsFloatData->SetElement(row,col, value);
				}
			}
		}
	}
	TableRefresh();
}

/******************************************************************************
 HandleDeletion

 ******************************************************************************/

void
RaggedFloatTable::HandleDeletion()
{
	SelectionType type = GetSelectionType();

	if (type == kNoneSelected)
	{
		return;
	}

	JIndex rows;
	JIndex cols;
	JIndex startRow;
	JIndex startCol;

	GetSelectionArea(&rows, &cols, &startRow, &startCol);

	if (startCol + cols == GetColCount() + 1)
	{
		cols--;
	}

	if (startRow + rows == GetRowCount() + 1)
	{
		rows--;
	}

	if (type == kRowsSelected)
	{
		auto* undo =
			jnew UndoElementsCut(this, JPoint(1, startRow),
								  JPoint(itsFloatData->GetDataColCount(), startRow + rows - 1),
								  UndoElementsBase::kRows);
		assert(undo != nullptr);
		NewUndo(undo);
		for (JIndex i = startRow; i < startRow + rows; i++)
		{
			itsFloatData->RemoveRow(startRow);
		}
	}

	else if (type == kColsSelected)
	{
		auto* undo =
			jnew UndoElementsCut(this, JPoint(startCol, 1),
								  JPoint(startCol + cols - 1, GetRowCount()),
								  UndoElementsBase::kCols);
		assert(undo != nullptr);
		NewUndo(undo);
		for (JIndex i = startCol; i < startCol + cols; i++)
		{
			itsFloatData->RemoveCol(startCol);
		}
	}

	else if (type == kElementsSelected)
	{
		// special case if just one cell - use special undo
		if ((cols == 1) && (rows == 1))
		{
			JPoint cell(startCol, startRow);
			JFloat value;
			if (itsFloatData->GetElement(cell, &value))
			{
				auto* undo =
					jnew UndoElementCut(this, cell, value);
				assert(undo != nullptr);
				NewUndo(undo);
			}
		}
		else
		{
			auto* undo =
				jnew UndoElementsCut(this, JPoint(startCol, startRow),
									  JPoint(startCol + cols - 1, startRow + rows - 1),
									  UndoElementsBase::kElements);
			assert(undo != nullptr);
			NewUndo(undo);
		}
		for (JIndex col = startCol; col < startCol + cols; col++)
		{
			for (JIndex row = startRow; row < startRow + rows; row++)
			{
				if (itsFloatData->CellValid(startRow,col))
				{
					itsFloatData->RemoveElement(startRow,col);
				}
			}
		}
	}

	(GetTableSelection()).ClearSelection();
	TableRefresh();
}

/******************************************************************************
 GetSelectionType

 ******************************************************************************/

RaggedFloatTable::SelectionType
RaggedFloatTable::GetSelectionType()
{
	JTableSelection& selection = GetTableSelection();
	JTableSelectionIterator iter(&selection);

	JPoint cell;
	if (selection.GetFirstSelectedCell(&cell))
	{
		if (cell.x > (JCoordinate)(itsFloatData->GetDataColCount()))
		{
			return kNoneSelected;
		}
	}
	else
	{
		return kNoneSelected;
	}

	bool found = iter.Next(&cell);
	if (found)
	{
		if (cell.y == 1)
		{
			const JSize rowCount = GetRowCount();
			bool colSelected = true;
			JIndex row = 2;
			while (row <= rowCount && colSelected)
			{
				if (!((GetTableSelection()).IsSelected(row,cell.x)))
				{
					colSelected = false;
				}
				row++;
			}

			if (colSelected)
			{
				return kColsSelected;
			}

			if (cell.x != 1)
			{
				return kElementsSelected;
			}

			const JSize colCount = GetColCount();
			bool rowSelected = true;
			JIndex col = 2;
			while (col <= colCount && rowSelected)
			{
				if (!((GetTableSelection()).IsSelected(cell.y,col)))
				{
					rowSelected = false;
				}
				col++;
			}

			if (rowSelected)
			{
				return kRowsSelected;
			}

			else
			{
				return kElementsSelected;
			}
		}

		else if (cell.x == 1)
		{
			const JSize colCount = GetColCount();
			bool rowSelected = true;
			JIndex col = 2;
			while (col <= colCount && rowSelected)
			{
				if (!((GetTableSelection()).IsSelected(cell.y,col)))
				{
					rowSelected = false;
				}
				col++;
			}

			if (rowSelected)
			{
				return kRowsSelected;
			}

			else
			{
				return kElementsSelected;
			}

		}

		else
		{
			return kElementsSelected;
		}
	}

	return kNoneSelected;
}

/******************************************************************************
 GetSelectionArea

 ******************************************************************************/

void
RaggedFloatTable::GetSelectionArea
	(
	JIndex* rows,
	JIndex* cols,
	JIndex* startRow,
	JIndex* startCol
	)
{
	JTableSelection& selection = GetTableSelection();
	JTableSelectionIterator iter(&selection);

	JPoint cell;
	const bool found = iter.Next(&cell);

	if (!found)
	{
		*rows = 1;
		*cols = 1;
		*startRow = 1;
		*startCol = 1;
		return;
	}

	*startRow = cell.y;
	*startCol = cell.x;

	JPoint cell2;

	iter.MoveTo(kJIteratorStartAtEnd,0,0);
	iter.Prev(&cell2);

	*rows = cell2.y - cell.y + 1;
	*cols = cell2.x - cell.x + 1;
}

/******************************************************************************
 UpdateDataMenu

 ******************************************************************************/

void
RaggedFloatTable::UpdateDataMenu()
{
	if (itsFloatData->GetDataColCount() == 0)
	{
		itsDataMenu->DisableItem(kPlotCmd);
		itsDataMenu->DisableItem(kTransCmd);
	}
	else
	{
		itsDataMenu->EnableItem(kPlotCmd);
		itsDataMenu->EnableItem(kTransCmd);
	}
}

/******************************************************************************
 HandleDataMenu

 ******************************************************************************/

void
RaggedFloatTable::HandleDataMenu
	(
	const JIndex index
	)
{
	if (index == kPlotCmd)
	{
		ChoosePlotColumns(kPlotCmd);
	}

	else if (index == kPlotVectorCmd)
	{
		ChoosePlotColumns(kPlotVectorCmd);
	}

	else if (index == kTransCmd)
	{
		ChooseNewTransformFunction();
	}

	else if (index == kGenerateRangeColCmd)
	{
		GetNewColByRange();
	}

	else if (index == kGenerateIncrementColCmd)
	{
		GetNewColByInc();
	}

	else if (index == kDataModuleCmd)
	{
		auto* dlog = JXChooseFileDialog::Create();
		if (dlog->DoDialog())
		{
			DataModule* dm;
			DataModule::Create(&dm, this, itsFloatData, dlog->GetFullName());
		}
	}
}

/******************************************************************************
 HandleModuleMenu

 ******************************************************************************/

void
RaggedFloatTable::HandleModuleMenu
	(
	const JIndex index
	)
{
	if (index == kReloadModuleCmd)
	{
		UpdateModuleMenu();
	}
	else
	{
		JString modName;
		GetApplication()->GetDataModulePath(index - 1, &modName);
		DataModule* dm;
		DataModule::Create(&dm, this, itsFloatData, modName);
	}
}

/******************************************************************************
 UpdateModuleMenu

 ******************************************************************************/

void
RaggedFloatTable::UpdateModuleMenu()
{
	const JSize mCount = itsModuleMenu->GetItemCount();
	JSize i;
	for (i = 2; i <= mCount; i++)
	{
		itsModuleMenu->RemoveItem(2);
	}

	GetApplication()->ReloadDataModules();
	JPtrArray<JString>* names = GetApplication()->GetDataModules();
	for (i = 1; i <= names->GetElementCount(); i++)
	{
		itsModuleMenu->AppendItem(*(names->GetElement(i)));
	}
}

/******************************************************************************
 ChoosePlotColumns

 ******************************************************************************/

void
RaggedFloatTable::ChoosePlotColumns
	(
	const JIndex type
	)
{
	if (itsFloatData->GetDataColCount() == 0)
	{
		JGetUserNotification()->ReportError(JGetString("NoDataToPlot::RaggedFloatTable"));
		return;
	}

	SelectionType selType = GetSelectionType();

	JIndex xCol;
	JIndex x2Col;
	JIndex yCol;
	JIndex y2Col;

	xCol = 0;
	x2Col = 0;
	yCol = 0;
	y2Col = 0;

	if (selType != kNoneSelected)
	{
		JIndex rows;
		JIndex cols;
		JIndex startRow;
		JIndex startCol;

		GetSelectionArea(&rows, &cols, &startRow, &startCol);

		xCol = startCol;
		if (cols > 1)
		{
			yCol = startCol+1;
		}
		if (cols > 2)
		{
			y2Col = startCol + 2;
		}
		if (cols > 3)
		{
			x2Col = startCol + 3;
		}
	}

	if (type == kPlotCmd)
	{
		auto* dlog = jnew CreatePlotDialog(itsTableDir, itsFloatData, xCol,yCol, x2Col,y2Col);
		assert (dlog != nullptr);
		if (dlog->DoDialog())
		{
			PlotData(kDataPlot, dlog);
		}
	}
	else if (type == kPlotVectorCmd)
	{
		auto* dlog = jnew CreateVectorPlotDialog(itsTableDir, itsFloatData, xCol,yCol, x2Col,y2Col);
		assert (dlog != nullptr);
		if (dlog->DoDialog())
		{
			PlotData(kVectorPlot, dlog);
		}
	}
}

/******************************************************************************
 PlotData (private)

 ******************************************************************************/

void
RaggedFloatTable::PlotData
	(
	const JIndex			type,
	CreatePlotDialogBase*	dlog
	)
{
	JIndex xCol, x2Col, yCol, y2Col;
	dlog->GetColumns(&xCol, &x2Col, &yCol, &y2Col);

	const JArray<JFloat>& xData = itsFloatData->GetColPointer(xCol);
	const JArray<JFloat>& yData = itsFloatData->GetColPointer(yCol);

	const JArray<JFloat>* xErr = nullptr;
	if (x2Col != 0)
	{
		xErr = &itsFloatData->GetColPointer(x2Col);
	}

	const JArray<JFloat>* yErr = nullptr;
	if (y2Col != 0)
	{
		yErr = &itsFloatData->GetColPointer(y2Col);
	}

	const JString& label = dlog->GetLabel();

	JIndex index;
	if (dlog->GetPlotIndex(&index))
	{
		itsTableDir->AddToPlot(index, type, xData, xErr, yData, yErr, true, label);
	}
	else
	{
		itsTableDir->CreateNewPlot(type, xData, xErr, yData, yErr, true, label);
	}
}

/******************************************************************************
 GetNewColByRange (private)

 ******************************************************************************/

void
RaggedFloatTable::GetNewColByRange()
{
	auto* dlog = jnew ColByRangeDialog(itsFloatData->GetDataColCount() + 1);
	assert (dlog != nullptr);
	if (dlog->DoDialog())
	{
		JIndex dest;
		dlog->GetDestination(&dest);
		JFloat beg;
		JFloat end;
		JInteger count;
		dlog->GetValues(&beg, &end, &count);

		bool replace = false;
		const JSize colCount = itsFloatData->GetDataColCount();
		if (dest <= colCount)
		{
			replace = JGetUserNotification()->AskUserYes(JGetString("ReplaceColWarning::RaggedFloatTable"));
		}

		if (!replace)
		{
			auto* undo =
				jnew UndoElementsInsert(this, JPoint(dest, 1),
										 JPoint(dest, 1),
										 UndoElementsBase::kCols);
			assert(undo != nullptr);
			NewUndo(undo);
			itsFloatData->InsertCols(dest, 1);
		}
		else if (dest <= colCount)
		{
			auto* undo =
				jnew UndoElementsChange(this, JPoint(dest, 1),
										 JPoint(dest, itsFloatData->GetDataRowCount(dest)),
										 UndoElementsBase::kCols);
			assert(undo != nullptr);
			NewUndo(undo);
			itsFloatData->RemoveAllElements(dest);
		}

		// this takes care of whether or not it was ascending.
		const JFloat inc = (end - beg)/(JFloat)( count - 1 );

		for (JInteger i = 1; i <= count; i++)
		{
			itsFloatData->SetElement(i, dest, beg + inc*(i-1));
		}
	}
}

/******************************************************************************
 GetNewColByInc (private)

 ******************************************************************************/

void
RaggedFloatTable::GetNewColByInc()
{
	auto* dlog = jnew ColByIncDialog(itsFloatData->GetDataColCount() + 1);
	assert (dlog != nullptr);
	if (dlog->DoDialog())
	{
		JIndex dest;
		dlog->GetDestination(&dest);
		JFloat beg;
		JFloat inc;
		JInteger count;
		dlog->GetValues(&beg, &inc, &count);

		bool replace = false;
		const JSize colCount = itsFloatData->GetDataColCount();
		if (dest <= colCount)
		{
			replace = JGetUserNotification()->AskUserYes(JGetString("ReplaceColWarning::RaggedFloatTable"));
		}

		if (!replace)
		{
			auto* undo =
				jnew UndoElementsInsert(this, JPoint(dest, 1),
										 JPoint(dest, 1),
										 UndoElementsBase::kCols);
			assert(undo != nullptr);
			NewUndo(undo);
			itsFloatData->InsertCols(dest, 1);
		}
		else if (dest <= colCount)
		{
			auto* undo =
				jnew UndoElementsChange(this, JPoint(dest, 1),
										 JPoint(dest, itsFloatData->GetDataRowCount(dest)),
										 UndoElementsBase::kCols);
			assert(undo != nullptr);
			NewUndo(undo);
			itsFloatData->RemoveAllElements(dest);
		}

		for (JInteger i = 1; i <= count; i++)
		{
			itsFloatData->SetElement(i, dest, beg + inc*(i-1));
		}
	}
}

/******************************************************************************
 WriteData

 ******************************************************************************/

void
RaggedFloatTable::WriteData
	(
	std::ostream& os
	)
{
	os << kCurrentTableVersion << ' ';
	const JSize colCount = itsFloatData->GetDataColCount();
	os << colCount << ' ';
	JString temp("column x");
	for (JSize i = 1; i <= colCount; i++)
	{
		os << temp << ' ';
		const JSize rowCount = itsFloatData->GetDataRowCount(i);
		os << rowCount << ' ';
		for (JSize j = 1; j <= rowCount; j++)
		{
			JFloat value;
			itsFloatData->GetElement(j, i, &value);
			os << value << ' ';
		}
	}
}

/******************************************************************************
 ReadData

 ******************************************************************************/

void
RaggedFloatTable::ReadData
	(
	std::istream&		is,
	const JFloat	gloveVersion
	)
{
	JFileVersion vers = 0;
	if (gloveVersion > 0.5)
	{
		is >> vers;
		assert(vers <= kCurrentTableVersion);
	}

	itsFloatData->ShouldBroadcast(false);

	JSize colCount;
	is >> colCount;
	JString temp;
	JSize rowCount;
	for (JSize i= 1; i <= colCount; i++)
	{
		is >> temp;
		is >> rowCount;
		for (JSize j = 1; j <= rowCount; j++)
		{
			JFloat value;
			is >> value;
			itsFloatData->SetElement(j, i, value);
		}
	}

	itsFloatData->ShouldBroadcast(true);
}

/******************************************************************************
 ChooseNewTransformFunction

 ******************************************************************************/

void
jCollectColumnIndexes
	(
	const JFunction*	root,
	JArray<JIndex>*		inds
	)
{
	const auto* fwv = dynamic_cast<const JFunctionWithVar*>(root);
	if (fwv != nullptr)
	{
		const JFunction* ai = fwv->GetArrayIndex();
		if (ai != nullptr)
		{
			JFloat x;
			const bool ok = ai->Evaluate(&x);
			assert( ok );
			const JIndex i = JRound(x);
			JIndex tmp;
			if (!inds->SearchSorted(i, JListT::kAnyMatch, &tmp))
			{
				inds->InsertSorted(i, false);
			}
		}
		return;
	}

	const auto* fwa = dynamic_cast<const JFunctionWithArgs*>(root);
	if (fwa != nullptr)
	{
		const JSize argCount = fwa->GetArgCount();
		for (JIndex i=1; i<=argCount; i++)
		{
			jCollectColumnIndexes(fwa->GetArg(i), inds);
		}
	}
}

void
RaggedFloatTable::ChooseNewTransformFunction()
{
	const JSize count = itsFloatData->GetDataColCount() + 1;
	if (count == 1)
	{
		JGetUserNotification()->ReportError(JGetString("NoDataToTransform::RaggedFloatTable"));
		return;
	}

	VarList xformVarList;
	JArray<JFloat>* ar = jnew JArray<JFloat>;
	for (JSize i = 1; i < count; i++)
	{
		ar->AppendElement(0);
	}
	xformVarList.AddArray(JString("col", JString::kNoCopy), *ar);

	auto* dlog = jnew TransformFunctionDialog(&xformVarList, count);
	assert (dlog != nullptr);

	if (dlog->DoDialog())
	{
		const JIndex dest = dlog->GetDestination();
		const JString& fnStr(dlog->GetFunctionString());

		const JSize count = itsFloatData->GetDataColCount();
		bool replace = false;
		if (dest <= count)
		{
			replace = JGetUserNotification()->AskUserYes(JGetString("ReplaceColWarning::RaggedFloatTable"));
		}

		JArray<JFloat> newArray;

		JExprParser p(&xformVarList);

		JFunction* f;
		if (!p.Parse(fnStr, &f))
		{
			return;
		}

		JArray<JIndex> inds;
		inds.SetCompareFunction(JCompareIndices);
		jCollectColumnIndexes(f, &inds);

		const JSize indCount = inds.GetElementCount();
		if (indCount == 0)
		{
			JGetUserNotification()->ReportError(JGetString("GenerateIfNoTransform::RaggedFloatTable"));
			jdelete f;
			return;
		}

		JSize minRowCount = itsFloatData->GetDataRowCount(inds.GetElement(1));
		for (JIndex i = 2; i <= indCount; i++)
		{
			const JSize rowCount = itsFloatData->GetDataRowCount(inds.GetElement(i));
			if (rowCount < minRowCount)
			{
				minRowCount = rowCount;
			}
		}

		for (JSize r = 1; r <= minRowCount; r++)
		{
			for (JIndex i = 1; i <= indCount; i++)
			{
				JFloat value;
				itsFloatData->GetElement(r, inds.GetElement(i), &value);
				xformVarList.SetNumericValue(1, inds.GetElement(i), value);
			}
			JFloat value;
			f->Evaluate(&value);
			newArray.AppendElement(value);
		}

		if (replace)
		{
			auto* undo =
				jnew UndoElementsChange(this, JPoint(dest, 1),
										 JPoint(dest, itsFloatData->GetDataRowCount(dest)),
										 UndoElementsBase::kCols);
			assert(undo != nullptr);
			NewUndo(undo);
			itsFloatData->RemoveAllElements(dest);
			itsFloatData->SetCol(dest, newArray);
		}
		else
		{
			auto* undo =
				jnew UndoElementsInsert(this, JPoint(dest, 1),
										 JPoint(dest, 1),
										 UndoElementsBase::kCols);
			assert(undo != nullptr);
			NewUndo(undo);
			itsFloatData->InsertCols(dest, 1);
			itsFloatData->SetCol(dest, newArray);
		}

		jdelete f;
	}
}

/******************************************************************************
 WriteDataCols

 ******************************************************************************/

bool
RaggedFloatTable::WriteDataCols
	(
	std::ostream& os,
	const int cols
	)
{
	SelectionType type = GetSelectionType();
	if (type != kColsSelected)
	{
		JString str((JUInt64) cols);
		const JUtf8Byte* map[] =
		{
			"i", str.GetBytes()
		};
		JGetUserNotification()->ReportError(
			JGetString("MustSelectColumns::RaggedFloatTable", map, sizeof(map)));
		return false;
	}
	JIndex nrows;
	JIndex ncols;
	JIndex startRow;
	JIndex startCol;

	GetSelectionArea(&nrows, &ncols, &startRow, &startCol);
	if (startCol + ncols == GetColCount() + 1)
	{
		ncols--;
	}

	if (startRow + nrows == GetRowCount() + 1)
	{
		nrows--;
	}

	if (cols > (int) ncols)
	{
		JString str((JUInt64) cols);
		const JUtf8Byte* map[] =
		{
			"i", str.GetBytes()
		};
		JGetUserNotification()->ReportError(
			JGetString("MustSelectColumns::RaggedFloatTable", map, sizeof(map)));
		return false;
	}
	os << nrows << std::endl;
	for (JIndex row = startRow; row < startRow + nrows; row++)
	{
		for (JIndex col = startCol; col < startCol + ncols; col++)
		{
			JFloat value;
			if (itsFloatData->GetElement(row, col, &value))
			{
				os << value;
			}
			if (col != startCol + ncols - 1)
			{
				os << "\t";
			}
		}
		os << std::endl;
	}
	return true;
}

/******************************************************************************
 ExportDataMatrix

 ******************************************************************************/

void
RaggedFloatTable::ExportDataMatrix
	(
	std::ostream& os
	)
{
	JSize rowCount = GetRowCount() - 1;
	JSize colCount = GetColCount() - 1;
	os << colCount << " ";
	os << rowCount << " ";
	for (JSize i = 1; i <= rowCount; i++)
	{
		for (JSize j = 1; j <= colCount; j++)
		{
			JFloat value;
			if (itsFloatData->GetElement(i, j, &value))
			{
				os << value << " ";
			}
			else
			{
				os << "0" << " ";
			}
		}
	}
}

/******************************************************************************
 ExportData

 ******************************************************************************/

void
RaggedFloatTable::ExportData
	(
	std::ostream& os
	)
{
	JSize colCount = GetColCount() - 1;
	os << colCount << " ";
	for (JSize i = 1; i <= colCount; i++)
	{
		JSize rowCount = itsFloatData->GetDataRowCount(i);
		os << rowCount << " ";
		for (JSize j = 1; j <= rowCount; j++)
		{
			JFloat value;
			if (itsFloatData->GetElement(j, i, &value))
			{
				os << value << " ";
			}
			else
			{
				os << "0" << " ";
			}
		}
	}
}

/******************************************************************************
 PrintRealTable (public)

 ******************************************************************************/

void
RaggedFloatTable::PrintRealTable
	(
	JPSPrinter& p
	)
{
	JSize cols = itsFloatData->GetDataColCount();
	JSize maxrows = 0;
	for (JSize i = 1; i <= cols; i++)
	{
		JSize rows = itsFloatData->GetDataRowCount(i);
		if (rows > maxrows)
		{
			maxrows = rows;
		}
	}
	JSize tableRowCount = GetRowCount();
	JSize tableColCount = GetColCount();

	while (GetColCount() > cols)
	{
		RemoveCol(GetColCount());
	}

	while (GetRowCount() > maxrows)
	{
		RemoveRow(GetRowCount());
	}

	const JColorID gray50Color = JColorManager::GetGrayColor(50);
	SetRowBorderInfo(0, gray50Color);
	SetColBorderInfo(0, gray50Color);

	Print(p, false, false);

	while (GetColCount() < tableColCount)
	{
		AppendCols(kDefColWidth, 1);
	}
	while(GetRowCount() < tableRowCount)
	{
		AppendRows(kDefRowWidth, 1);
	}
	SetRowBorderInfo(1, gray50Color);	// calls TableSetScrollSteps()
	SetColBorderInfo(1, gray50Color);
}

/******************************************************************************
 Undo (public)

	This is the function that is called when the user asks to undo.

 ******************************************************************************/

void
RaggedFloatTable::Undo()
{
	// This can't be called while Undo/Redo is being called.
	assert( itsUndoState == kIdle );

	// See if we have an undo object available.
	JUndo* undo;
	const bool hasUndo = GetCurrentUndo(&undo);

	// Perform the undo.
	if (hasUndo)
	{
		itsUndoState = kUndo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
	}
}

/******************************************************************************
 Redo (public)

	This is the function that is called when the user asks to redo.

 ******************************************************************************/

void
RaggedFloatTable::Redo()
{
	// This can't be called while Undo/Redo is being called.
	assert( itsUndoState == kIdle );

	// See if we have an redo object available.
	JUndo* undo;
	const bool hasUndo = GetCurrentRedo(&undo);

	// Perform the redo.
	if (hasUndo)
	{
		itsUndoState = kRedo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
	}
}

/******************************************************************************
 GetCurrentUndo (private)

 ******************************************************************************/

bool
RaggedFloatTable::GetCurrentUndo
	(
	JUndo** undo
	)
	const
{
	if (HasUndo())
	{
		*undo = itsUndoList->GetElement(itsFirstRedoIndex - 1);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 GetCurrentRedo (private)

 ******************************************************************************/

bool
RaggedFloatTable::GetCurrentRedo
	(
	JUndo** redo
	)
	const
{
	if (HasRedo())
	{
		*redo = itsUndoList->GetElement(itsFirstRedoIndex);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 HasUndo (public)

 ******************************************************************************/

bool
RaggedFloatTable::HasUndo()
	const
{
	return itsFirstRedoIndex > 1;
}

/******************************************************************************
 HasRedo (public)

 ******************************************************************************/

bool
RaggedFloatTable::HasRedo()
	const
{
	return itsFirstRedoIndex <= itsUndoList->GetElementCount();
}

/******************************************************************************
 NewUndo (private)

 ******************************************************************************/

void
RaggedFloatTable::NewUndo
	(
	JUndo* undo
	)
{
	if (itsUndoList != nullptr && itsUndoState == kIdle)
	{
		// clear redo objects

		const JSize undoCount = itsUndoList->GetElementCount();
		for (JIndex i=undoCount; i>=itsFirstRedoIndex; i--)
		{
			itsUndoList->DeleteElement(i);
		}

		// save the new object

		itsUndoList->Append(undo);
		itsFirstRedoIndex++;

		assert( !itsUndoList->IsEmpty() );
	}

	else if (itsUndoList != nullptr && itsUndoState == kUndo)
	{
		assert( itsFirstRedoIndex > 1 );

		itsFirstRedoIndex--;
//		JUndo* oldUndo = itsUndoList->GetElement(itsFirstRedoIndex);
//		jdelete oldUndo;
		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);

		undo->SetRedo(true);
		undo->Deactivate();
	}

	else if (itsUndoList != nullptr && itsUndoState == kRedo)
	{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() );

//		JUndo* oldRedo = itsUndoList->GetElement(itsFirstRedoIndex);
//		jdelete oldRedo;
		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);
		itsFirstRedoIndex++;

		undo->SetRedo(false);
		undo->Deactivate();
	}

}
