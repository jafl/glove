/******************************************************************************
 UndoElementBase.h

	Interface for the UndoElementBase class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_UndoElementBase
#define _H_UndoElementBase

#include <UndoBase.h>
#include <jx-af/jcore/JPoint.h>

class RaggedFloatTable;
class RaggedFloatTableData;

class UndoElementBase : public UndoBase
{
public:

	UndoElementBase(RaggedFloatTable* table, const JPoint& cell);

	virtual ~UndoElementBase();

protected:

	JPoint	GetCell();

private:

	JPoint	itsCell;
};

/******************************************************************************
 GetCell (protected)

 ******************************************************************************/

inline JPoint
UndoElementBase::GetCell()
{
	return itsCell;
}

#endif
