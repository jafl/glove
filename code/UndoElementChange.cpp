/******************************************************************************
 UndoElementChange.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "UndoElementChange.h"
#include "RaggedFloatTable.h"
#include "RaggedFloatTableData.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoElementChange::UndoElementChange
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

UndoElementChange::~UndoElementChange()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
UndoElementChange::Undo()
{
	// get current value
	JFloat value = 0;
	GetData()->GetItem(GetCell(), &value);

	// change value to old value
	GetData()->SetItem(GetCell(), itsValue);

	// create undo object to change it back
	auto undo = jnew UndoElementChange(GetTable(), GetCell(), value);
	NewUndo(undo);
}
