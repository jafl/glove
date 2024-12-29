/******************************************************************************
 NonLinearFitDialog.h

	Interface for the NonLinearFitDialog class

	Copyright (C) 2000 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_NonLinearFitDialog
#define _H_NonLinearFitDialog

#include <jx-af/jx/JXModalDialogDirector.h>

#include <jx-af/jcore/JString.h>

class VarList;
class VarTable;
class JFunction;
class JXExprEditor;
class JXInputField;
class JXTextButton;
class JXIdleTask;
class JXVertPartition;

class NonLinearFitDialog : public JXModalDialogDirector
{
public:

	NonLinearFitDialog();
	~NonLinearFitDialog() override;

	JString			GetFunctionString() const;
	JString			GetDerivativeString() const;
	const VarList&	GetVarList() const;
	const JString&	GetFitName() const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	bool	OKToDeactivate() override;

private:

	VarList*	itsVarList;
	JXIdleTask*	itsDelButtonTask;

// begin JXLayout

	JXVertPartition* itsPartition;
	JXExprEditor*    itsFnEditor;
	JXExprEditor*    itsDerivativeEditor;
	VarTable*        itsVarTable;
	JXTextButton*    itsNewButton;
	JXTextButton*    itsDeleteButton;
	JXTextButton*    itsHelpButton;
	JXInputField*    itsNameInput;

// end JXLayout

private:

	void	BuildWindow();
	bool	OKToDeleteParm();
};

/******************************************************************************
 GetVarList (public)

 ******************************************************************************/

inline const VarList&
NonLinearFitDialog::GetVarList()
	const
{
	return *itsVarList;
}


#endif
