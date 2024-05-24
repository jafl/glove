/******************************************************************************
 UndoElementCut.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = UndoElementBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "UndoElementCut.h"
#include "UndoElementAppend.h"
#include "RaggedFloatTable.h"
#include "RaggedFloatTableData.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoElementCut::UndoElementCut
	(
	RaggedFloatTable*	table,
	const JPoint&		cell,
	const JFloat		value
	)
	:
	UndoElementBase(table, cell),
	itsValue(value)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UndoElementCut::~UndoElementCut()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
UndoElementCut::Undo()
{
	// change value to old value
	GetData()->InsertElement(GetCell(), itsValue);

	// create undo object to change it back
	auto undo = jnew UndoElementAppend(GetTable(), GetCell());
	NewUndo(undo);
}
