/******************************************************************************
 UndoBase.h

	Interface for the UndoBase class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_UndoBase
#define _H_UndoBase

#include <jx-af/jcore/JUndo.h>
#include <jx-af/jcore/JPoint.h>

class RaggedFloatTable;
class RaggedFloatTableData;

class UndoBase : public JUndo
{
public:

	UndoBase(RaggedFloatTable* table);

	virtual ~UndoBase();

protected:

	RaggedFloatTableData*	GetData();
	RaggedFloatTable*		GetTable();
	void					NewUndo(JUndo* undo);

private:

	RaggedFloatTableData*	itsData;	// we don't own this
	RaggedFloatTable*		itsTable;	// we don't own this
};

/******************************************************************************
 GetData (protected)

 ******************************************************************************/

inline RaggedFloatTableData*
UndoBase::GetData()
{
	return itsData;
}

/******************************************************************************
 GetTable (protected)

 ******************************************************************************/

inline RaggedFloatTable*
UndoBase::GetTable()
{
	return itsTable;
}

#endif
