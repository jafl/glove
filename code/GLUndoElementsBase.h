/******************************************************************************
 GLUndoElementsBase.h

	Interface for the GLUndoElementsBase class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementsBase
#define _H_GLUndoElementsBase

#include <GLUndoBase.h>
#include <jx-af/jcore/JPoint.h>

class GLRaggedFloatTable;
class GLRaggedFloatTableData;

class GLUndoElementsBase : public GLUndoBase
{
public:

	enum UndoType
		{
		kRows = 1,
		kCols,
		kElements
		};
	
public:

	GLUndoElementsBase(GLRaggedFloatTable* table, const JPoint& start,
					   const JPoint& end, const UndoType type);

	virtual ~GLUndoElementsBase();

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
GLUndoElementsBase::GetStartCell()
{
	return itsStartCell;
}

/******************************************************************************
 GetEndCell (protected)

 ******************************************************************************/

inline JPoint
GLUndoElementsBase::GetEndCell()
{
	return itsEndCell;
}

/******************************************************************************
 GetType (protected)

 ******************************************************************************/

inline GLUndoElementsBase::UndoType
GLUndoElementsBase::GetType()
{
	return itsType;
}

#endif
