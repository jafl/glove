/******************************************************************************
 UndoBase.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JUndo

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include <UndoBase.h>
#include <RaggedFloatTable.h>
#include <RaggedFloatTableData.h>
#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoBase::UndoBase
	(
	RaggedFloatTable*	table
	)
	:
	JUndo(),
	itsTable(table)
{
	itsData = table->itsFloatData;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UndoBase::~UndoBase()
{
}

/******************************************************************************
 NewUndo (protected)

 ******************************************************************************/

void
UndoBase::NewUndo
	(
	JUndo* undo
	)
{
	itsTable->GetUndoRedoChain()->NewUndo(undo);
}
