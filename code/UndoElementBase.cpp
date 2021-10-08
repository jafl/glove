/******************************************************************************
 UndoElementBase.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JUndo

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <UndoElementBase.h>
#include <RaggedFloatTable.h>
#include <RaggedFloatTableData.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoElementBase::UndoElementBase
	(
	RaggedFloatTable*	table,
	const JPoint&			cell
	)
	:
	UndoBase(table),
	itsCell(cell)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UndoElementBase::~UndoElementBase()
{
}
