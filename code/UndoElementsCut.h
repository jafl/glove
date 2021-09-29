/******************************************************************************
 UndoElementsCut.h

	Interface for the UndoElementsCut class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_UndoElementsCut
#define _H_UndoElementsCut

#include <UndoElementsBase.h>
#include <jx-af/jcore/JPoint.h>
#include <jx-af/jcore/JPtrArray.h>

class RaggedFloatTable;
class RaggedFloatTableData;

class UndoElementsCut : public UndoElementsBase
{
public:

	UndoElementsCut(RaggedFloatTable* table, const JPoint& start,
						 const JPoint& end, 
						 const UndoElementsBase::UndoType type);

	virtual ~UndoElementsCut();

	virtual void	Undo();

private:

	JPtrArray<JArray<JFloat> >*	itsValues;
};

#endif
