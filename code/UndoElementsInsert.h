/******************************************************************************
 UndoElementsInsert.h

	Interface for the UndoElementsInsert class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_UndoElementsInsert
#define _H_UndoElementsInsert

#include <UndoElementsBase.h>
#include <jx-af/jcore/JPoint.h>
#include <jx-af/jcore/JPtrArray.h>

class RaggedFloatTable;
class RaggedFloatTableData;

class UndoElementsInsert : public UndoElementsBase
{
public:

	UndoElementsInsert(RaggedFloatTable* table, const JPoint& start,
						 const JPoint& end,
						 const UndoElementsBase::UndoType type);

	~UndoElementsInsert() override;

	void	Undo() override;
};

#endif
