/******************************************************************************
 UndoElementsBase.h

	Interface for the UndoElementsBase class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_UndoElementsBase
#define _H_UndoElementsBase

#include <UndoBase.h>
#include <jx-af/jcore/JPoint.h>

class RaggedFloatTable;
class RaggedFloatTableData;

class UndoElementsBase : public UndoBase
{
public:

	enum UndoType
		{
		kRows = 1,
		kCols,
		kElements
		};
	
public:

	UndoElementsBase(RaggedFloatTable* table, const JPoint& start,
					   const JPoint& end, const UndoType type);

	virtual ~UndoElementsBase();

protected:

	JPoint		GetStartCell();
	JPoint		GetEndCell();
	UndoType	GetType();

private:

	JPoint		itsStartCell;
	JPoint		itsEndCell;
	UndoType	itsType;
};

/******************************************************************************
 GetStartCell (protected)

 ******************************************************************************/

inline JPoint
UndoElementsBase::GetStartCell()
{
	return itsStartCell;
}

/******************************************************************************
 GetEndCell (protected)

 ******************************************************************************/

inline JPoint
UndoElementsBase::GetEndCell()
{
	return itsEndCell;
}

/******************************************************************************
 GetType (protected)

 ******************************************************************************/

inline UndoElementsBase::UndoType
UndoElementsBase::GetType()
{
	return itsType;
}

#endif
