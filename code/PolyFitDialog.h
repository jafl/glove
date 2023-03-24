/******************************************************************************
 PolyFitDialog.h

	Interface for the PolyFitDialog class

	Copyright (C) 2000 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_PolyFitDialog
#define _H_PolyFitDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class VarList;
class JXExprEditor;
class JXInputField;
class JXTextButton;
class JXTextCheckbox;

class PolyFitDialog : public JXModalDialogDirector
{
public:

	PolyFitDialog();

	~PolyFitDialog() override;

	void			GetPowers(JArray<JIndex>* powers);
	const JString&	GetFitName() const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	bool	OKToDeactivate() override;

private:

	JXExprEditor*		itsFn;
	VarList*			itsVarList;
	JXTextCheckbox*		itsCB[10];

// begin JXLayout

	JXTextButton* itsHelpButton;
	JXInputField* itsNameInput;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
