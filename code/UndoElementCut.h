/******************************************************************************
 UndoElementCut.h

	Interface for the UndoElementCut class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_UndoElementCut
#define _H_UndoElementCut

#include "UndoElementBase.h"
#include <jx-af/jcore/JPoint.h>

class RaggedFloatTable;
class RaggedFloatTableData;

class UndoElementCut : public UndoElementBase
{
public:

	UndoElementCut(RaggedFloatTable* table, const JPoint& cell,
					 const JFloat value);

	~UndoElementCut() override;

	void	Undo() override;

private:

	JFloat	itsValue;
};

#endif
