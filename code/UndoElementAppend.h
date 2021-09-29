/******************************************************************************
 UndoElementAppend.h

	Interface for the UndoElementAppend class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_UndoElementAppend
#define _H_UndoElementAppend

#include <UndoElementBase.h>
#include <jx-af/jcore/JPoint.h>

class RaggedFloatTable;
class RaggedFloatTableData;

class UndoElementAppend : public UndoElementBase
{
public:

	UndoElementAppend(RaggedFloatTable* table, const JPoint& cell);

	virtual ~UndoElementAppend();

	virtual void	Undo();
};

#endif
