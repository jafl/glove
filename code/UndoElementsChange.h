/******************************************************************************
 UndoElementsChange.h

	Interface for the UndoElementsChange class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_UndoElementsChange
#define _H_UndoElementsChange

#include <UndoElementsBase.h>
#include <jx-af/jcore/JPoint.h>
#include <jx-af/jcore/JPtrArray.h>

class RaggedFloatTable;
class RaggedFloatTableData;

class UndoElementsChange : public UndoElementsBase
{
public:

	UndoElementsChange(RaggedFloatTable* table, const JPoint& start,
						 const JPoint& end, 
						 const UndoElementsBase::UndoType type);

	virtual ~UndoElementsChange();

	virtual void	Undo();

private:

	JPtrArray<JArray<JFloat> >*	itsValues;
};

#endif
