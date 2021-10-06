/******************************************************************************
 PlotFunctionDialog.h

	Interface for the PlotFunctionDialog class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_PlotFunctionDialog
#define _H_PlotFunctionDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextButton;
class JXInputField;
class VarList;
class ExprDirector;
class JXTextMenu;
class JString;

class PlotFunctionDialog : public JXDialogDirector
{
public:

	PlotFunctionDialog(JXDirector* supervisor, VarList* list);
	virtual ~PlotFunctionDialog();
	
	const JString& GetFunctionString();
	
protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	bool	OKToDeactivate() override;

private:

// begin JXLayout

	JXInputField* itsFunctionString;
	JXTextButton* itsEditButton;
	JXTextButton* itsClearButton;
	JXTextMenu*   itsVarMenu;

// end JXLayout

	ExprDirector* itsEditor;
	VarList* 		itsList;

private:

	void	BuildWindow();
};

#endif
