/******************************************************************************
 UndoElementsInsert.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = UndoElementsBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <UndoElementsInsert.h>
#include <UndoElementsCut.h>
#include <RaggedFloatTable.h>
#include <RaggedFloatTableData.h>

#include <jx-af/jcore/JMinMax.h>

#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoElementsInsert::UndoElementsInsert
	(
	RaggedFloatTable*				table,
	const JPoint&						start,
	const JPoint&						end,
	const UndoElementsBase::UndoType	type
	)
	:
	UndoElementsBase(table, start, end, type)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UndoElementsInsert::~UndoElementsInsert()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
UndoElementsInsert::Undo()
{

	// we need to create this before we change the data, because
	// it needs to read the old data first. We can't yet call NewUndo,
	// though, because that will delete us.

	UndoElementsCut* undo =
		jnew UndoElementsCut(GetTable(), GetStartCell(), GetEndCell(), GetType());
	assert(undo != nullptr);

	RaggedFloatTableData* data		= GetData();
	JPoint start					= GetStartCell();
	JPoint end						= GetEndCell();
	UndoElementsBase::UndoType type	= GetType();

	if (type == UndoElementsBase::kRows)
	{
		for (JCoordinate i = start.y; i <= end.y; i++)
		{
			data->RemoveRow(start.y);
		}
	}
	else if (type == UndoElementsBase::kCols)
	{
		for (JCoordinate i = start.x; i <= end.x; i++)
		{
			data->RemoveCol(start.x);
		}
	}
	else if (type == UndoElementsBase::kElements)
	{
		for (JCoordinate i = start.x; i <= end.x; i++)
		{
			for (JCoordinate j = start.y; j <= end.y; j++)
			{
				if (data->CellValid(start.y, i))
				{
					data->RemoveElement(start.y, i);
				}
			}
		}
	}

	NewUndo(undo);
}
