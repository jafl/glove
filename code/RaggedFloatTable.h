/******************************************************************************
 RaggedFloatTable.h

	Interface for the RaggedFloatTable class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_RaggedFloatTable
#define _H_RaggedFloatTable

#include <jx-af/jx/JXEditTable.h>
#include <jx-af/jcore/JAuxTableData.h>

class RaggedFloatTableData;
class CreatePlotDialogBase;
class JXFloatInput;
class JTableSelection;
class JXMenuBar;
class JXTextMenu;
class DataDocument;
class VarList;
class JXToolBar;
class JXTextButton;
class JUndoRedoChain;
class JUndo;
class UndoBase;
class DataModule;
class JPSPrinter;

class RaggedFloatTable : public JXEditTable
{
public:

	friend class UndoBase;
	friend class DataModule;

public:

	enum SelectionType
	{
		kRowsSelected = 1,
		kColsSelected,
		kElementsSelected,
		kNoneSelected
	};

public:

	RaggedFloatTable(DataDocument* dir,
					JXTextButton* okButton,
					RaggedFloatTableData* data, const int precision,
					JXMenuBar* menuBar, JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~RaggedFloatTable() override;

	void SelectRow(const JIndex row);
	void AddRowToSelection(const JIndex row);
	void ExtendSelectionToRow(const JIndex row);
	void SelectCol(const JIndex col);
	void AddColToSelection(const JIndex col);
	void ExtendSelectionToCol(const JIndex col);

	void WriteData(std::ostream& os);
	void ReadData(std::istream& is, const JFloat gloveVersion);

	RaggedFloatTableData*				GetFloatData() const;
	RaggedFloatTable::SelectionType	GetSelectionType();

	void	GetSelectionArea(JIndex* rows, JIndex* cols, JIndex* startRow, JIndex* startCol);
	bool	WriteDataCols(std::ostream& os, const int cols);
	void	ExportDataMatrix(std::ostream& os);
	void	ExportData(std::ostream& os);

	void	PrintRealTable(JPSPrinter& p);

	JUndoRedoChain*	GetUndoRedoChain();

	void	LoadDefaultToolButtons(JXToolBar* toolbar);

protected:

	void Receive(JBroadcaster* sender, const Message& message) override;
	void HandleMouseDown(const JPoint& pt, const JXMouseButton button,
						const JSize clickCount,
						const JXButtonStates& buttonStates,
						const JXKeyModifiers& modifiers) override;
	void HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
						const JXKeyModifiers& modifiers) override;
	void HandleKeyPress(const JUtf8Character& c, const int keySym,
								const JXKeyModifiers& modifiers) override;

	void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	JXInputField*	CreateXInputField(const JPoint& cell,
									  const JCoordinate x, const JCoordinate y,
									  const JCoordinate w, const JCoordinate h) override;
	bool			ExtractInputData(const JPoint& cell) override;
	void			PrepareDeleteXInputField() override;

private:

	enum DragType
	{
		kInvalidDrag,
		kSelectRangeDrag
	};

private:

	DataDocument*				itsTableDir;			// owns us
	RaggedFloatTableData*		itsFloatData;			// we don't own this

	JXFloatInput*				itsFloatInputField;		// used during editing

	JXTextMenu*					itsEditMenu;
	JXTextMenu*					itsDataMenu;
	JXTextMenu*					itsModuleMenu;

	Atom						itsGloveTextXAtom;

	JXTextButton*				itsOKButton;
	DragType					itsDragType;

	JUndoRedoChain*				itsUndoChain;

private:

	void SelectCell(const JPoint cell);
	void ExtendSelection(const JPoint cell);

	void HandleEditMenu(const JIndex Index);
	void UpdateEditMenu();
	void HandleDataMenu(const JIndex index);
	void UpdateDataMenu();

	void HandleCutCmd();
	void HandleCopyCmd();
	void HandlePasteCmd();
	void HandleSpecialPasteCmd();
	void HandleInsertion(const bool undo = true);
	void HandleDuplication();
	void HandleDeletion();

	void ChoosePlotColumns(const JIndex type);
	void PlotData(const JIndex type, CreatePlotDialogBase* dlog);

	void GetNewColByRange();
	void GetNewColByInc();

	void ChooseNewTransformFunction();

	void HandleModuleMenu(const JIndex index);
	void UpdateModuleMenu();
};


/******************************************************************************
 GetUndoRedoChain

 ******************************************************************************/

inline JUndoRedoChain*
RaggedFloatTable::GetUndoRedoChain()
{
	return itsUndoChain;
}

/******************************************************************************
 GetFloatData (protected)

 ******************************************************************************/

inline RaggedFloatTableData*
RaggedFloatTable::GetFloatData()
	const
{
	return itsFloatData;
}

#endif
