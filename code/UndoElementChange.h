/******************************************************************************
 UndoElementChange.h

	Interface for the UndoElementChange class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_UndoElementChange
#define _H_UndoElementChange

#include <UndoElementBase.h>
#include <jx-af/jcore/JPoint.h>

class RaggedFloatTable;
class RaggedFloatTableData;

class UndoElementChange : public UndoElementBase
{
public:

	UndoElementChange(RaggedFloatTable* table, const JPoint& cell,
						const JFloat value);

	virtual ~UndoElementChange();

	virtual void	Undo();

private:

	JFloat	itsValue;
};

#endif
