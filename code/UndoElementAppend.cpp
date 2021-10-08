/******************************************************************************
 UndoElementAppend.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = UndoElementBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <UndoElementAppend.h>
#include <UndoElementCut.h>
#include <RaggedFloatTable.h>
#include <RaggedFloatTableData.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoElementAppend::UndoElementAppend
	(
	RaggedFloatTable*	table,
	const JPoint&			cell
	)
	:
	UndoElementBase(table, cell)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UndoElementAppend::~UndoElementAppend()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
UndoElementAppend::Undo()
{
	// get current value
	JFloat value = 0;
	GetData()->GetElement(GetCell(), &value);

	// change value to old value
	GetData()->RemoveElement(GetCell());

	// create undo object to change it back
	UndoElementCut* undo =
		jnew UndoElementCut(GetTable(), GetCell(), value);
	assert(undo != nullptr);
	NewUndo(undo);
}
