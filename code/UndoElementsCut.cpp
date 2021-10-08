/******************************************************************************
 UndoElementsCut.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <UndoElementsCut.h>
#include <UndoElementsInsert.h>
#include <RaggedFloatTable.h>
#include <RaggedFloatTableData.h>

#include <jx-af/jcore/JMinMax.h>

#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoElementsCut::UndoElementsCut
	(
	RaggedFloatTable*				table,
	const JPoint&						start,
	const JPoint&						end,
	const UndoElementsBase::UndoType	type
	)
	:
	UndoElementsBase(table, start, end, type)
{
	itsValues = jnew JPtrArray<JArray<JFloat> >(JPtrArrayT::kDeleteAll);
	assert(itsValues != nullptr);

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
		JArray<JFloat>* col = jnew JArray<JFloat>;
		assert(col != nullptr);
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
			rowend		= JMin((JSize)end.y, data->GetDataRowCount(i));
		}

		for (JSize j = rowstart; j <= rowend; j++)
		{
			JFloat value;
			if (data->GetElement(j, i, &value))
			{
				col->AppendElement(value);
			}
		}
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UndoElementsCut::~UndoElementsCut()
{
	jdelete itsValues;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
UndoElementsCut::Undo()
{

	// we need to create this before we change the data, because
	// it needs to read the old data first. We can't yet call NewUndo,
	// though, because that will delete us.

	UndoElementsInsert* undo =
		jnew UndoElementsInsert(GetTable(), GetStartCell(), GetEndCell(), GetType());
	assert(undo != nullptr);

	RaggedFloatTableData* data		= GetData();
	JPoint start						= GetStartCell();
	UndoElementsBase::UndoType type	= GetType();

	if (type == UndoElementsBase::kCols)
	{
		JSize cols = itsValues->GetElementCount();
		for (JSize i = 1; i <= cols; i++)
		{
			JArray<JFloat>* col = itsValues->GetElement(i);
			data->InsertCol(i + start.x - 1, col);
		}
	}
	else
	{
		JSize cols = itsValues->GetElementCount();
		for (JSize i = 1; i <= cols; i++)
		{
			JArray<JFloat>* col = itsValues->GetElement(i);
			JSize rows = col->GetElementCount();
			for (JSize j = 1; j <= rows; j++)
			{
				JFloat value = col->GetElement(j);
				data->InsertElement(j + start.y - 1 , i + start.x - 1, value);
			}
		}
	}

	NewUndo(undo);
}
