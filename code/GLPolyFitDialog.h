/******************************************************************************
 GLPolyFitDialog.h

	Interface for the GLPolyFitDialog class

	Copyright (C) 2000 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_GLPolyFitDialog
#define _H_GLPolyFitDialog

#include <jx-af/jx/JXDialogDirector.h>

#include <jx-af/jcore/JArray.h>

class GLVarList;
class JString;
class JXExprEditor;
class JXInputField;
class JXTextButton;
class JXTextCheckbox;

class GLPolyFitDialog : public JXDialogDirector
{
public:

	GLPolyFitDialog(JXDirector* supervisor);
	virtual ~GLPolyFitDialog();

	void			GetPowers(JArray<JIndex>* powers);
	const JString&	GetFitName() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual bool	OKToDeactivate() override;

private:

	JXExprEditor*		itsFn;
	GLVarList*			itsVarList;
	JXTextCheckbox*		itsCB[10];

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXInputField* itsNameInput;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
