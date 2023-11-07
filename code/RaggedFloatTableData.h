/******************************************************************************
 RaggedFloatTableData.h

	Interface for RaggedFloatTableData class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GRaggedFloatTableData
#define _H_GRaggedFloatTableData

#include <jx-af/jcore/JTableData.h>
#include <jx-af/jcore/JPtrArray.h>

class RaggedFloatTableData : public JTableData
{
public:

	RaggedFloatTableData(const JFloat& defValue);
	RaggedFloatTableData(const RaggedFloatTableData& source);

	~RaggedFloatTableData() override;

	bool	GetItem(const JIndex row, const JIndex col, JFloat* value) const;
	bool	GetItem(const JPoint& cell, JFloat* value) const;
	void	SetItem(const JIndex row, const JIndex col, const JFloat data);
	void	SetItem(const JPoint& cell, const JFloat data);

	void	GetRow(const JIndex index, JArray<JFloat>* rowData) const;
	void	SetRow(const JIndex index, const JArray<JFloat>& rowData);

	void	GetCol(const JIndex index, JList<JFloat>* colData) const;
	void	SetCol(const JIndex index, const JList<JFloat>& colData);

	const JArray<JFloat>&	GetColPointer(const JIndex index) const;

	void	InsertRow(const JIndex index, const JList<JFloat>* initData = nullptr);
	void	InsertRows(const JIndex index, const JSize count, const JList<JFloat>* initData = nullptr);
	void	PrependRow();
	void	DuplicateRow(const JIndex index);
	void	RemoveRow(const JIndex index);
	void	RemoveAllRows();
	void	MoveRow(const JIndex origIndex, const JIndex newIndex);

	void	InsertCol(const JIndex index, const JList<JFloat>* initData = nullptr);
	void	InsertCols(const JIndex index, const JSize count, const JList<JFloat>* initData = nullptr);
	void	PrependCol(const JList<JFloat>* initData = nullptr);
	void	AppendCol(const JList<JFloat>* initData = nullptr);
	void	DuplicateCol(const JIndex origIndex, const JIndex newIndex);
	void	RemoveCol(const JIndex index);
	void	RemoveAllCols();
	void	MoveCol(const JIndex origIndex, const JIndex newIndex);

	void	InsertElement(const JIndex row, const JIndex col, const JFloat value);
	void	InsertElement(const JPoint& cell, const JFloat value);
	void	PrependItem(const JIndex col, const JFloat value);
	void	AppendItem(const JIndex col, const JFloat value);
	void	DuplicateElement(const JIndex row, const JIndex col);
	void	DuplicateElement(const JPoint& cell);
	void	RemoveItem(const JIndex row, const JIndex col);
	void	RemoveItem(const JPoint& cell);
	void	RemoveAllElements(const JIndex col);
	void	MoveElement(const JIndex origRow, const JIndex origCol,
						const JIndex newRow, const JIndex newCol);
	void	MoveElement(const JPoint& origCell, const JPoint& newCell);

	JSize	GetDataColCount() const;
	bool	ColIndexValid(const JIndex index) const;
	JSize	GetDataRowCount(const JIndex index) const;
	bool	CellValid(const JIndex rowIndex, const JIndex colIndex) const;
	bool	CellValid(const JPoint& cell) const;
	bool	FindColumn(const JArray<JFloat>* array, JIndex *index) const;

	void	ShouldBroadcast(const bool on);

private:

	JPtrArray< JArray<JFloat> >*	itsCols;
	const JFloat					itsDefValue;
	bool							itsBroadcast;

private:

	void	CreateCellIfNeeded(const JIndex row, const JIndex col);
	void	CreateCellIfNeeded(const JPoint cell);
	JSize	GetMaxRowCount();

	// not allowed

	RaggedFloatTableData& operator=(const RaggedFloatTableData&) = delete;

public:

	static const JUtf8Byte* kItemInserted;
	static const JUtf8Byte* kItemRemoved;
	static const JUtf8Byte* kDataChanged;

	class ItemInserted : public JBroadcaster::Message
		{
		public:

			ItemInserted(const JIndex row, const JIndex col)
				:
				JBroadcaster::Message(kItemInserted),
				itsRow(row),
				itsCol(col)
				{ };

			JIndex
			GetRow() const
			{
				return itsRow;
			};

			JIndex
			GetCol() const
			{
				return itsCol;
			};

		private:

			JIndex itsRow;
			JIndex itsCol;
		};

	class ItemRemoved : public JBroadcaster::Message
		{
		public:

			ItemRemoved(const JIndex row, const JIndex col)
				:
				JBroadcaster::Message(kItemRemoved),
				itsRow(row),
				itsCol(col)
				{ };

			JIndex
			GetRow() const
			{
				return itsRow;
			};

			JIndex
			GetCol() const
			{
				return itsCol;
			};

		private:

			JIndex itsRow;
			JIndex itsCol;
		};

	class DataChanged : public JBroadcaster::Message
		{
		public:

			DataChanged()
				:
				JBroadcaster::Message(kDataChanged)
				{ };
		};
};


/******************************************************************************
 PrependRow

 ******************************************************************************/

inline void
RaggedFloatTableData::PrependRow()
{
	InsertRow(1);
}

/******************************************************************************
 Column manipulations

 ******************************************************************************/

inline void
RaggedFloatTableData::PrependCol
	(
	const JList<JFloat>* initData
	)
{
	InsertCol(1, initData);
}

inline void
RaggedFloatTableData::AppendCol
	(
	const JList<JFloat>* initData
	)
{
	InsertCol(itsCols->GetItemCount()+1, initData);
}

/******************************************************************************
 GetColPointer

 ******************************************************************************/

inline const JArray<JFloat>&
RaggedFloatTableData::GetColPointer
	(
	const JIndex index
	)
	const
{
	return *(itsCols->GetItem(index));
}

/******************************************************************************
 PrependItem

 ******************************************************************************/

inline void
RaggedFloatTableData::PrependItem
	(
	const JIndex col,
	const JFloat value
	)
{
	InsertElement(1, col, value);
}

/******************************************************************************
 AppendItem

 ******************************************************************************/

inline void
RaggedFloatTableData::AppendItem
	(
	const JIndex col,
	const JFloat value
	)
{
	JArray<JFloat>* dataCol = itsCols->GetItem(col);
	InsertElement(dataCol->GetItemCount()+1, col, value);
}

/******************************************************************************
 InsertElement

 ******************************************************************************/

inline void
RaggedFloatTableData::InsertElement
	(
	const JPoint&	cell,
	const JFloat	value
	)
{
	InsertElement(cell.y, cell.x, value);
}

/******************************************************************************
 RemoveItem

 ******************************************************************************/

inline void
RaggedFloatTableData::RemoveItem
	(
	const JPoint& cell
	)
{
	RemoveItem(cell.y, cell.x);
}

/******************************************************************************
 MoveElement

 ******************************************************************************/

inline void
RaggedFloatTableData::MoveElement
	(
	const JPoint& origCell,
	const JPoint& newCell
	)
{
	MoveElement(origCell.y, origCell.x, newCell.y, newCell.x);
}

/******************************************************************************
 DuplicateElement

 ******************************************************************************/

inline void
RaggedFloatTableData::DuplicateElement
	(
	const JPoint& cell
	)
{
	DuplicateElement(cell.y, cell.x);
}

/******************************************************************************
 GetItem

 ******************************************************************************/

inline bool
RaggedFloatTableData::GetItem
	(
	const JPoint&	cell,
	JFloat*			value
	)
	const
{
	return GetItem(cell.y, cell.x, value);
}

/******************************************************************************
 SetItem

 ******************************************************************************/

inline void
RaggedFloatTableData::SetItem
	(
	const JPoint&	cell,
	const JFloat	data
	)
{
	SetItem(cell.y, cell.x, data);
}

/******************************************************************************
 ShouldBroadcast

 ******************************************************************************/

inline void
RaggedFloatTableData::ShouldBroadcast
	(
	const bool on
	)
{
	const bool wasOn = itsBroadcast;
	itsBroadcast = on;
	if (!wasOn && itsBroadcast)
		{
		Broadcast(DataChanged());
		}
}

/******************************************************************************
 GetDataColCount

 ******************************************************************************/

inline JSize
RaggedFloatTableData::GetDataColCount()
	const
{
	return itsCols->GetItemCount();
}

/******************************************************************************
 ColIndexValid

 ******************************************************************************/

inline bool
RaggedFloatTableData::ColIndexValid
	(
	const JIndex index
	)
	const
{
	return itsCols->IndexValid(index);
}

/******************************************************************************
 GetDataRowCount

 ******************************************************************************/

inline JSize
RaggedFloatTableData::GetDataRowCount
	(
	const JIndex index
	)
	const
{
	const JArray<JFloat>* dataCol = itsCols->GetItem(index);
	return dataCol->GetItemCount();
}

/******************************************************************************
 CellValid

 ******************************************************************************/

inline bool
RaggedFloatTableData::CellValid
	(
	const JIndex rowIndex,
	const JIndex colIndex
	)
	const
{
	return itsCols->IndexValid(colIndex) &&
				(itsCols->GetItem(colIndex))->IndexValid(rowIndex);
}

inline bool
RaggedFloatTableData::CellValid
	(
	const JPoint& cell
	)
	const
{
	return CellValid(cell.y, cell.x);
}

/******************************************************************************
 FindColumn

 ******************************************************************************/

inline bool
RaggedFloatTableData::FindColumn
	(
	const JArray<JFloat>*	array,
	JIndex*					index
	)
	const
{
	return itsCols->Find(array, index);
}

#endif
