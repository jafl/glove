/******************************************************************************
 TransformFunctionDialog.h

	Interface for the TransformFunctionDialog class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_TransformFunctionDialog
#define _H_TransformFunctionDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXTextButton;
class JXInputField;
class VarList;
class JXTextMenu;
class JXStaticText;

class TransformFunctionDialog : public JXModalDialogDirector
{
public:

	TransformFunctionDialog(VarList* list, const JSize colCount);

	~TransformFunctionDialog() override;

	JIndex	GetDestination();
	const JString& GetFunctionString();

protected:

	bool	OKToDeactivate() override;

private:

// begin JXLayout

	JXTextMenu*   itsDestMenu;
	JXStaticText* itsColNumber;
	JXTextButton* itsEditButton;
	JXTextMenu*   itsVarMenu;
	JXTextButton* itsCloseButton;
	JXTextButton* itsClearButton;
	JXTextButton* itsTransformButton;
	JXInputField* itsFunctionString;

// end JXLayout

	VarList*	itsList;
	JIndex		itsDestCol;

private:

	void	BuildWindow();
};

#endif
