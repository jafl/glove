/******************************************************************************
 ExprDirector.h

	Interface for the ExprDirector class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_ExprDirector
#define _H_ExprDirector

#include <jx-af/jx/JXDialogDirector.h>

class VarList;
class JXExprEditor;

class ExprDirector : public JXDialogDirector
{
public:

	ExprDirector(JXDirector* supervisor, const VarList* list, const JString& function);
	~ExprDirector() override;

	JString GetString();

private:

	JXExprEditor*	itsEditor;
};

#endif
