/******************************************************************************
 UndoElementsChange.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "UndoElementsChange.h"
#include "RaggedFloatTable.h"
#include "RaggedFloatTableData.h"

#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoElementsChange::UndoElementsChange
	(
	RaggedFloatTable*					table,
	const JPoint&						start,
	const JPoint&						end,
	const UndoElementsBase::UndoType	type
	)
	:
	UndoElementsBase(table, start, end, type)
{
	itsValues = jnew JPtrArray<JArray<JFloat> >(JPtrArrayT::kDeleteAll);

	RaggedFloatTableData* data = GetData();

	JIndex colstart;
	JIndex colend;
	if (type == UndoElementsBase::kRows)
	{
		colstart	= 1;
		colend		= data->GetDataColCount();
	}
	else
	{
		colstart	= start.x;
		colend		= end.x;
	}

	for (JSize i = colstart; i <= colend; i++)
	{
		auto col = jnew JArray<JFloat>;
		itsValues->Append(col);

		JIndex rowstart;
		JIndex rowend;
		if (type == UndoElementsBase::kCols)
		{
			rowstart	= 1;
			rowend		= data->GetDataRowCount(i);
		}
		else
		{
			rowstart	= start.y;
			rowend		= JMin((JIndex) end.y, data->GetDataRowCount(i));
		}

		for (JSize j = rowstart; j <= rowend; j++)
		{
			JFloat value;
			if (data->GetItem(j, i, &value))
			{
				col->AppendItem(value);
			}
		}
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UndoElementsChange::~UndoElementsChange()
{
	jdelete itsValues;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
UndoElementsChange::Undo()
{

	// we need to create this before we change the data, because
	// it needs to read the old data first. We can't yet call NewUndo,
	// though, because that will delete us.

	auto undo = jnew UndoElementsChange(GetTable(), GetStartCell(), GetEndCell(), GetType());

	RaggedFloatTableData* data		= GetData();
	JPoint start						= GetStartCell();
	JPoint end							= GetEndCell();
	UndoElementsBase::UndoType type	= GetType();

	if (type == UndoElementsBase::kRows)
	{
	}
	else if (type == UndoElementsBase::kCols)
	{
		JSize cols = itsValues->GetItemCount();
		for (JSize i = 1; i <= cols; i++)
		{
			JArray<JFloat>* col = itsValues->GetItem(i);
			JSize rows = col->GetItemCount();
			for (JSize j = 1; j <= rows; j++)
			{
				JFloat value = col->GetItem(j);
				data->SetItem(j, i + start.x - 1, value);
			}
		}
	}
	else if (type == UndoElementsBase::kElements)
	{
		for (JSize i = start.x; i <= (JSize)end.x; i++)
		{
			JArray<JFloat>* col = itsValues->GetItem(i - start.x + 1);
			JSize rows = col->GetItemCount();
			for (JSize j = start.y; j <= start.y + rows -1; j++)
			{
				JFloat value = col->GetItem(j - start.y + 1);
				data->SetItem(j, i, value);
			}
		}
	}

	NewUndo(undo);
}
