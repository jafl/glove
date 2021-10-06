/******************************************************************************
 NonLinearFitDialog.h

	Interface for the NonLinearFitDialog class

	Copyright (C) 2000 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_NonLinearFitDialog
#define _H_NonLinearFitDialog

#include <jx-af/jx/JXDialogDirector.h>

#include <jx-af/jcore/JString.h>

class VarList;
class VarTable;
class JFunction;
class JXExprEditor;
class JXInputField;
class JXTextButton;
class JXTimerTask;
class JXVertPartition;

class NonLinearFitDialog : public JXDialogDirector
{
public:

	NonLinearFitDialog(JXDirector* supervisor);
	virtual ~NonLinearFitDialog();

	JString				GetFunctionString() const;
	JString				GetDerivativeString() const;
	const VarList&	GetVarList() const;
	const JString&		GetFitName() const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	bool	OKToDeactivate() override;

private:

	JXExprEditor*	itsFnEditor;
	JXExprEditor*	itsDerivativeEditor;
	VarList*		itsVarList;
	VarTable*		itsVarTable;
	JXTimerTask*	itsDelButtonTask;

// begin JXLayout

	JXVertPartition* itsPartition;
	JXTextButton*    itsHelpButton;

// end JXLayout
// begin functionLayout

	JXInputField* itsNameInput;

// end functionLayout

// begin derivativeLayout


// end derivativeLayout

// begin variableLayout

	JXTextButton* itsNewButton;
	JXTextButton* itsDeleteButton;

// end variableLayout

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
