/******************************************************************************
 RaggedFloatTableData.cpp

	Class for storing floats in a table of ragged columns.

	BASE CLASS = JTableData

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "RaggedFloatTableData.h"
#include <jx-af/jcore/jAssert.h>

const JUtf8Byte* RaggedFloatTableData::kItemInserted = "ItemInserted::RaggedFloatTableData";
const JUtf8Byte* RaggedFloatTableData::kItemRemoved  = "ItemRemoved::RaggedFloatTableData";
const JUtf8Byte* RaggedFloatTableData::kDataChanged     = "DataChanged::RaggedFloatTableData";

/******************************************************************************
 Constructor

 ******************************************************************************/

RaggedFloatTableData::RaggedFloatTableData
	(
	const JFloat& defValue
	)
	:
	JTableData(),
	itsDefValue(defValue)
{
	itsCols = jnew JPtrArray< JArray<JFloat> >(JPtrArrayT::kDeleteAll);
	assert( itsCols != nullptr );

	const JSize kInitialColCount = 20;
	const JSize kInitialRowCount = 100;
	ColsAdded(kInitialColCount);
	Broadcast(JTableData::ColsInserted(1, kInitialColCount));

	RowsAdded(kInitialRowCount);
	Broadcast(JTableData::RowsInserted(1, kInitialRowCount));

	itsBroadcast = true;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

RaggedFloatTableData::RaggedFloatTableData
	(
	const RaggedFloatTableData& source
	)
	:
	JTableData(source),
	itsDefValue(source.itsDefValue),
	itsBroadcast(source.itsBroadcast)
{
	itsCols = jnew JPtrArray< JArray<JFloat> >(JPtrArrayT::kDeleteAll);
	assert( itsCols != nullptr );

	const JSize count = (source.itsCols)->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JArray<JFloat>* origColData = (source.itsCols)->GetItem(i);
		JArray<JFloat>* newColData = jnew JArray<JFloat>(*origColData);
		assert( newColData != nullptr );
		itsCols->Append(newColData);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

RaggedFloatTableData::~RaggedFloatTableData()
{
	itsCols->DeleteAll();
	jdelete itsCols;
}

/******************************************************************************
 GetItem

 ******************************************************************************/

bool
RaggedFloatTableData::GetItem
	(
	const JIndex row,
	const JIndex col,
	JFloat* value
	)
	const
{
	if (CellValid(row,col) == false)
	{
		return false;
	}

	const JArray<JFloat>* dataCol = itsCols->GetItem(col);
	*value =  dataCol->GetItem(row);
	return true;
}

/******************************************************************************
 CreateCellIfNeeded (private)

 ******************************************************************************/

inline void
RaggedFloatTableData::CreateCellIfNeeded
	(
	const JPoint cell
	)
{
	CreateCellIfNeeded(cell.y, cell.x);
}

/******************************************************************************
 CreateCellIfNeeded

 ******************************************************************************/

void
RaggedFloatTableData::CreateCellIfNeeded
	(
	const JIndex row,
	const JIndex col
	)
{
	while (col > itsCols->GetItemCount())
	{
		AppendCol();
	}

	const bool origBroadcast = itsBroadcast;
	if (origBroadcast)
	{
		ShouldBroadcast(false);
	}

	const JArray<JFloat>* dataCol = itsCols->GetItem(col);
	while (row > dataCol->GetItemCount())
	{
		AppendItem(col, itsDefValue);
	}

	if (origBroadcast)
	{
		ShouldBroadcast(true);
	}
}

/******************************************************************************
 SetItem

 ******************************************************************************/

void
RaggedFloatTableData::SetItem
	(
	const JIndex	row,
	const JIndex	col,
	const JFloat	data
	)
{
	CreateCellIfNeeded(row, col);
	JArray<JFloat>* dataCol = itsCols->GetItem(col);
	dataCol->SetItem(row, data);
	if (itsBroadcast)
	{
		Broadcast(JTableData::RectChanged(row,col));
	}
}

/******************************************************************************
 GetRow

 ******************************************************************************/

void
RaggedFloatTableData::GetRow
	(
	const JIndex			index,
	JArray<JFloat>*	rowData
	)
	const
{
	rowData->RemoveAll();

	const JSize colCount = GetDataColCount();
	for (JIndex i=1; i<=colCount; i++)
	{
		const JArray<JFloat>* dataCol = itsCols->GetItem(i);
		rowData->AppendItem(dataCol->GetItem(index));
	}
}

/******************************************************************************
 SetRow

 ******************************************************************************/

void
RaggedFloatTableData::SetRow
	(
	const JIndex				index,
	const JArray<JFloat>&	rowData
	)
{
	const JSize colCount = GetDataColCount();
	assert( rowData.GetItemCount() == colCount );

	for (JIndex i=1; i<=colCount; i++)
	{
		JArray<JFloat>* dataCol = itsCols->GetItem(i);
		dataCol->SetItem(index, rowData.GetItem(i));
	}

	Broadcast(JTableData::RectChanged(JRect(index, 1, index+1, colCount+1)));
}

/******************************************************************************
 GetCol

	operator= doesn't work because it could be -any- class derived from JList.

 ******************************************************************************/

void
RaggedFloatTableData::GetCol
	(
	const JIndex	index,
	JList<JFloat>*	colData
	)
	const
{
	colData->RemoveAll();

	const JArray<JFloat>* dataCol = itsCols->GetItem(index);
	const JSize rowCount = dataCol->GetItemCount();

	for (JIndex i=1; i<=rowCount; i++)
	{
		colData->AppendItem(dataCol->GetItem(i));
	}
}

/******************************************************************************
 SetCol

 ******************************************************************************/

void
RaggedFloatTableData::SetCol
	(
	const JIndex			index,
	const JList<JFloat>&	colData
	)
{
	JArray<JFloat>* dataCol = itsCols->GetItem(index);
	const JSize rowCount = colData.GetItemCount();
	CreateCellIfNeeded(rowCount, index);

	JListIterator<JFloat>* iter = colData.NewIterator();
	JFloat v;
	JIndex i = 1;
	while (iter->Next(&v))
	{
		dataCol->SetItem(i, v);
		i++;
	}

	jdelete iter;
	iter = nullptr;

	JRect rect(1, index, rowCount + 1, index + 1);
	if (itsBroadcast)
	{
		Broadcast(JTableData::RectChanged(rect));
	}
}

/******************************************************************************
 InsertRow

 ******************************************************************************/

void
RaggedFloatTableData::InsertRow
	(
	const JIndex			index,
	const JList<JFloat>*	initData
	)
{
	const JSize colCount = itsCols->GetItemCount();
	assert( initData == nullptr || initData->GetItemCount() == colCount );

	JListIterator<JFloat>* iter = nullptr;
	if (initData != nullptr)
	{
		iter = initData->NewIterator();
	}

	for (JIndex i=1; i<=colCount; i++)
	{
		JArray<JFloat>* colData = itsCols->GetItem(i);
		if (index <= colData->GetItemCount())
		{
			JFloat value = 0;
			if (iter != nullptr)
			{
				iter->Next(&value);
			}
			InsertElement(index, i, value);
		}
	}

	jdelete iter;
}

/******************************************************************************
 InsertRows (public)

 ******************************************************************************/

void
RaggedFloatTableData::InsertRows
	(
	const JIndex				index,
	const JSize				count,
	const JList<JFloat>*	initData
	)
{
	for (JIndex i = 0; i < count; i++)
	{
		InsertRow(i + index, initData);
	}
}

/******************************************************************************
 DuplicateRow

 ******************************************************************************/

void
RaggedFloatTableData::DuplicateRow
	(
	const JIndex index
	)
{
	const JSize colCount = itsCols->GetItemCount();
	for (JIndex i=1; i<=colCount; i++)
	{
		JArray<JFloat>* colData = itsCols->GetItem(i);
		const JSize rowCount = colData->GetItemCount();

		if (index <= rowCount)
		{
			const JFloat element = colData->GetItem(index);
			colData->InsertItemAtIndex(index, element);
		}
	}

	RowsAdded(1);
	if (itsBroadcast)
	{
		Broadcast(JTableData::RowDuplicated(index, index));
	}
}

/******************************************************************************
 RemoveRow

 ******************************************************************************/

void
RaggedFloatTableData::RemoveRow
	(
	const JIndex index
	)
{
	assert( index < GetRowCount() );

	const JSize colCount = itsCols->GetItemCount();
	for (JIndex i=1; i<=colCount; i++)
	{
		JArray<JFloat>* colData = itsCols->GetItem(i);
		const JSize rowCount = colData->GetItemCount();

		if (index <= rowCount)
		{
			RemoveItem(index, i);
		}
	}

}

/******************************************************************************
 RemoveAllRows

 ******************************************************************************/

void
RaggedFloatTableData::RemoveAllRows()
{
	const JSize colCount = itsCols->GetItemCount();
	for (JIndex i=1; i<=colCount; i++)
	{
		JArray<JFloat>* colData = itsCols->GetItem(i);
		colData->RemoveAll();
	}
	const JSize count	= GetRowCount();
	SetRowCount(0);
	if (itsBroadcast)
	{
		Broadcast(JTableData::RowsRemoved(1, count));
	}
	RowsAdded(1);
	if (itsBroadcast)
	{
		Broadcast(JTableData::RowsInserted(1, 1));
	}
}

/******************************************************************************
 MoveRow

 ******************************************************************************/

void
RaggedFloatTableData::MoveRow
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	const JSize colCount = itsCols->GetItemCount();
	for (JIndex i=1; i<=colCount; i++)
	{
		JArray<JFloat>* colData = itsCols->GetItem(i);
		const JSize rowCount = colData->GetItemCount();
		if ( (origIndex <= rowCount) && (newIndex <= rowCount) )
		{
			colData->MoveItemToIndex(origIndex, newIndex);
		}
	}

	if (itsBroadcast)
	{
		Broadcast(JTableData::RowMoved(origIndex, newIndex));
	}
}

/******************************************************************************
 InsertCol

 ******************************************************************************/

void
RaggedFloatTableData::InsertCol
	(
	const JIndex			index,
	const JList<JFloat>*	initData
	)
{
	JIndex trueIndex = index;
	const JIndex maxIndex = itsCols->GetItemCount()+1;
	if (trueIndex > maxIndex)
	{
		trueIndex = maxIndex;
	}

	JArray<JFloat>* colData = jnew JArray<JFloat>;
	assert( colData != nullptr );
	itsCols->InsertAtIndex(trueIndex, colData);

	if (initData != nullptr)
	{
		JListIterator<JFloat>* iter = initData->NewIterator();

		JFloat v;
		JIndex i = 1;
		while (iter->Next(&v))
		{
			colData->InsertItemAtIndex(i, v);
		}

		jdelete iter;
	}

	ColsAdded(1);
	if (itsBroadcast)
	{
		Broadcast(JTableData::ColsInserted(trueIndex, 1));
	}
}

/******************************************************************************
 InsertCols

 ******************************************************************************/

void
RaggedFloatTableData::InsertCols
	(
	const JIndex				index,
	const JSize				count,
	const JList<JFloat>*	initData
	)
{
	for (JIndex i = 0; i < count; i++)
	{
		InsertCol(i + index, initData);
	}
}

/******************************************************************************
 DuplicateCol

 ******************************************************************************/

void
RaggedFloatTableData::DuplicateCol
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	JIndex trueIndex = newIndex;
	const JIndex maxIndex = itsCols->GetItemCount()+1;
	if (trueIndex > maxIndex)
	{
		trueIndex = maxIndex;
	}

	JArray<JFloat>* origColData = itsCols->GetItem(origIndex);
	JArray<JFloat>* newColData = jnew JArray<JFloat>(*origColData);
	assert( newColData != nullptr );
	itsCols->InsertAtIndex(trueIndex, newColData);

	ColsAdded(1);
	if (itsBroadcast)
	{
		Broadcast(JTableData::ColDuplicated(origIndex, trueIndex));
	}
}

/******************************************************************************
 RemoveCol

 ******************************************************************************/

void
RaggedFloatTableData::RemoveCol
	(
	const JIndex index
	)
{
	assert( ColIndexValid(index) );
	itsCols->DeleteItem(index);
/*	if (itsCols->GetItemCount() != 1)
	{
		itsCols->DeleteItem(index);
	}
	else
	{
		RemoveAllElements(index);
		itsCols->DeleteItem(index);
	}
*/
	ColsDeleted(1);
	if (itsBroadcast)
	{
		Broadcast(JTableData::ColsRemoved(index, 1));
	}
}

/******************************************************************************
 RemoveAllCols

 ******************************************************************************/

void
RaggedFloatTableData::RemoveAllCols()
{
	itsCols->DeleteAll();

	const JSize count	= GetColCount();
	SetColCount(0);
	if (itsBroadcast)
	{
		Broadcast(JTableData::ColsRemoved(1, count));
	}
	ColsAdded(1);
	if (itsBroadcast)
	{
		Broadcast(JTableData::ColsInserted(1, 1));
	}
}

/******************************************************************************
 MoveCol

 ******************************************************************************/

void
RaggedFloatTableData::MoveCol
	(
	const JIndex origIndex,
	const JIndex newIndex
	)
{
	itsCols->MoveItemToIndex(origIndex, newIndex);
	if (itsBroadcast)
	{
		Broadcast(JTableData::ColMoved(origIndex, newIndex));
	}
}

/******************************************************************************
 InsertElement

 ******************************************************************************/

void
RaggedFloatTableData::InsertElement
	(
	const JIndex row,
	const JIndex col,
	const JFloat value
	)
{
	JArray<JFloat>* dataCol = itsCols->GetItem(col);
	dataCol->InsertItemAtIndex(row, value);
	const JSize rowCount = dataCol->GetItemCount();
	if (itsBroadcast)
	{
		Broadcast(RaggedFloatTableData::ItemInserted(row, col));
	}

	if (rowCount == GetRowCount())
	{
		RowsAdded(1);
		if (itsBroadcast)
		{
			Broadcast(JTableData::RowsInserted(rowCount+1, 1));
		}
	}
}

/******************************************************************************
 DuplicateElement

 ******************************************************************************/

void
RaggedFloatTableData::DuplicateElement
	(
	const JIndex row,
	const JIndex col
	)
{
	JFloat value;
	if (GetItem(row,col, &value))
	{
		InsertElement(row, col, value);
	}
}

/******************************************************************************
 RemoveItem

 ******************************************************************************/

void
RaggedFloatTableData::RemoveItem
	(
	const JIndex row,
	const JIndex col
	)
{
	JArray<JFloat>* dataCol = itsCols->GetItem(col);
	const JSize rowCount = dataCol->GetItemCount();
	if (row <= rowCount)
	{
		dataCol->RemoveItem(row);
	}
	if (itsBroadcast)
	{
		Broadcast(RaggedFloatTableData::ItemRemoved(row, col));
	}

	if (GetMaxRowCount() == GetRowCount() - 2)
	{
		if (itsBroadcast)
		{
			Broadcast(JTableData::RowsRemoved(GetRowCount(), 1));
		}
		RowsDeleted(1);
	}
}

/******************************************************************************
 RemoveAllElements

 ******************************************************************************/

void
RaggedFloatTableData::RemoveAllElements
	(
	const JIndex col
	)
{
	JArray<JFloat>* dataCol = itsCols->GetItem(col);
	const JSize rowCount = dataCol->GetItemCount();
	for (JSize i = 1; i <= rowCount; i++)
	{
		RemoveItem(1, col);
	}
}

/******************************************************************************
 MoveElement

 ******************************************************************************/

void
RaggedFloatTableData::MoveElement
	(
	const JIndex origRow,
	const JIndex origCol,
	const JIndex newRow,
	const JIndex newCol
	)
{
	JFloat value;
	if (GetItem(origRow, origCol, &value))
	{
		InsertElement(newRow, newCol, value);
		RemoveItem(origRow, origCol);
	}
}

/******************************************************************************
 GetMaxRowCount

 ******************************************************************************/

JSize
RaggedFloatTableData::GetMaxRowCount()
{
	JSize max = 0;
	const JSize colCount = itsCols->GetItemCount();
	for (JSize i = 1; i <= colCount; i++)
	{
		JArray<JFloat>* dataCol = itsCols->GetItem(i);
		if (dataCol->GetItemCount() > max)
		{
			max = dataCol->GetItemCount();
		}
	}
	return max;
}
