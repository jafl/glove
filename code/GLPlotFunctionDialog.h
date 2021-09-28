/******************************************************************************
 GLPlotFunctionDialog.h

	Interface for the GLPlotFunctionDialog class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GLPlotFunctionDialog
#define _H_GLPlotFunctionDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextButton;
class JXInputField;
class GLVarList;
class GLExprDirector;
class JXTextMenu;
class JString;

class GLPlotFunctionDialog : public JXDialogDirector
{
public:

	GLPlotFunctionDialog(JXDirector* supervisor, GLVarList* list);
	virtual ~GLPlotFunctionDialog();
	
	const JString& GetFunctionString();
	
protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual bool	OKToDeactivate() override;

private:

// begin JXLayout

	JXInputField* itsFunctionString;
	JXTextButton* itsEditButton;
	JXTextButton* itsClearButton;
	JXTextMenu*   itsVarMenu;

// end JXLayout

	GLExprDirector* itsEditor;
	GLVarList* 		itsList;

private:

	void	BuildWindow();
};

#endif
