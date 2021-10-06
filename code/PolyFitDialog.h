/******************************************************************************
 PolyFitDialog.h

	Interface for the PolyFitDialog class

	Copyright (C) 2000 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_PolyFitDialog
#define _H_PolyFitDialog

#include <jx-af/jx/JXDialogDirector.h>

#include <jx-af/jcore/JArray.h>

class VarList;
class JString;
class JXExprEditor;
class JXInputField;
class JXTextButton;
class JXTextCheckbox;

class PolyFitDialog : public JXDialogDirector
{
public:

	PolyFitDialog(JXDirector* supervisor);
	virtual ~PolyFitDialog();

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
