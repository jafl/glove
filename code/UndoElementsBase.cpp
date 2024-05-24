/******************************************************************************
 UndoElementsBase.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JUndo

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "UndoElementsBase.h"
#include "RaggedFloatTable.h"
#include "RaggedFloatTableData.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoElementsBase::UndoElementsBase
	(
	RaggedFloatTable*	table,
	const JPoint&			start,
	const JPoint&			end,
	const UndoType			type
	)
	:
	UndoBase(table),
	itsStartCell(start),
	itsEndCell(end),
	itsType(type)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UndoElementsBase::~UndoElementsBase()
{
}
