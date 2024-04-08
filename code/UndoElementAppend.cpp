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
	const JPoint&		cell
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
	GetData()->GetItem(GetCell(), &value);

	// change value to old value
	GetData()->RemoveItem(GetCell());

	// create undo object to change it back
	auto undo = jnew UndoElementCut(GetTable(), GetCell(), value);
	NewUndo(undo);
}
