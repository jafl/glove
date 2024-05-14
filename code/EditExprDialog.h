/******************************************************************************
 EditExprDialog.h

	Interface for the EditExprDialog class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_ExprDirector
#define _H_ExprDirector

#include <jx-af/jx/JXModalDialogDirector.h>

class VarList;
class JXExprEditor;

class EditExprDialog : public JXModalDialogDirector
{
public:

	EditExprDialog(const VarList* list, const JString& function);

	~EditExprDialog() override;

	JString GetString();

private:

	JXExprEditor*	itsEditor;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const VarList* list, const JString& function);
};

#endif
