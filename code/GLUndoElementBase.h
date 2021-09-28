/******************************************************************************
 GLUndoElementBase.h

	Interface for the GLUndoElementBase class.

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLUndoElementBase
#define _H_GLUndoElementBase

#include <GLUndoBase.h>
#include <jx-af/jcore/JPoint.h>

class GLRaggedFloatTable;
class GLRaggedFloatTableData;

class GLUndoElementBase : public GLUndoBase
{
public:

	GLUndoElementBase(GLRaggedFloatTable* table, const JPoint& cell);

	virtual ~GLUndoElementBase();

protected:

	JPoint	GetCell();

private:

	JPoint	itsCell;
};

/******************************************************************************
 GetCell (protected)

 ******************************************************************************/

inline JPoint
GLUndoElementBase::GetCell()
{
	return itsCell;
}

#endif
