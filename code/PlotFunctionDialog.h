/******************************************************************************
 PlotFunctionDialog.h

	Interface for the PlotFunctionDialog class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_PlotFunctionDialog
#define _H_PlotFunctionDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXTextMenu;
class JXTextButton;
class JXInputField;
class VarList;

class PlotFunctionDialog : public JXModalDialogDirector
{
public:

	PlotFunctionDialog(VarList* list);

	~PlotFunctionDialog() override;

	const JString& GetFunctionString();

protected:

	bool	OKToDeactivate() override;

private:

// begin JXLayout

	JXInputField* itsFunctionString;
	JXTextButton* itsEditButton;
	JXTextButton* itsClearButton;
	JXTextMenu*   itsVarMenu;

// end JXLayout

	VarList*	itsList;

private:

	void	BuildWindow();
};

#endif
