/******************************************************************************
 NonLinearFitDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2000 by Glenn Bach.

 ******************************************************************************/

#include "NonLinearFitDialog.h"
#include "VarList.h"
#include "VarTable.h"

#include "globals.h"

#include <jx-af/jx/JXApplication.h>
#include <jx-af/jexpr/JXExprEditor.h>
#include <jx-af/jexpr/JXExprEditorSet.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXFunctionTask.h>
#include <jx-af/jx/JXVertPartition.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXDeleteObjectTask.h>

#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kDeleteButtonUpdateDelay = 1000;

/******************************************************************************
 Constructor

 ******************************************************************************/

NonLinearFitDialog::NonLinearFitDialog()
	:
	JXModalDialogDirector(true)
{
	itsVarList	= jnew VarList();

	itsVarList->AddVariable(JGetString("DefaultVarName::global"), 0);

	BuildWindow();

	itsDelButtonTask = jnew JXFunctionTask(kDeleteButtonUpdateDelay, [this]()
	{
		if (OKToDeleteParm())
		{
			itsDeleteButton->Activate();
		}
		else
		{
			itsDeleteButton->Deactivate();
		}
	});
	assert(itsDelButtonTask != nullptr);
	itsDelButtonTask->Start();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

NonLinearFitDialog::~NonLinearFitDialog()
{
	JXDeleteObjectTask<VarList>::Delete(itsVarList);
	jdelete itsDelButtonTask;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
NonLinearFitDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,430, JGetString("WindowTitle::NonLinearFitDialog::JXLayout"));

	JArray<JCoordinate> itsPartition_sizes, itsPartition_minSizes;
	itsPartition_sizes.AppendItem(121);
	itsPartition_minSizes.AppendItem(80);
	itsPartition_sizes.AppendItem(119);
	itsPartition_minSizes.AppendItem(80);
	itsPartition_sizes.AppendItem(110);
	itsPartition_minSizes.AppendItem(100);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	itsPartition =
		jnew JXVertPartition(itsPartition_sizes, 3, itsPartition_minSizes, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,360);

	auto* fitNameLabel =
		jnew JXStaticText(JGetString("fitNameLabel::NonLinearFitDialog::JXLayout"), itsPartition->GetCompartment(1),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 70,20);
	fitNameLabel->SetToLabel(false);

	auto* fitFnLabel =
		jnew JXStaticText(JGetString("fitFnLabel::NonLinearFitDialog::JXLayout"), itsPartition->GetCompartment(1),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 80,20);
	fitFnLabel->SetToLabel(false);

	auto* fnScrollbarSet =
		jnew JXScrollbarSet(itsPartition->GetCompartment(1),
					JXWidget::kHElastic, JXWidget::kVElastic, 100,30, 290,80);
	assert( fnScrollbarSet != nullptr );

	itsFnEditor =
		jnew JXExprEditor(itsVarList, menuBar, fnScrollbarSet, fnScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 290,80);

	auto* derivativeLabel =
		jnew JXStaticText(JGetString("derivativeLabel::NonLinearFitDialog::JXLayout"), itsPartition->GetCompartment(2),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 90,20);
	derivativeLabel->SetToLabel(false);

	auto* derivativeScrollbarSet =
		jnew JXScrollbarSet(itsPartition->GetCompartment(2),
					JXWidget::kHElastic, JXWidget::kVElastic, 100,10, 290,80);
	assert( derivativeScrollbarSet != nullptr );

	itsDerivativeEditor =
		jnew JXExprEditor(itsVarList, itsFnEditor, derivativeScrollbarSet, derivativeScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 290,80);

	auto* warningText =
		jnew JXStaticText(JGetString("warningText::NonLinearFitDialog::JXLayout"), itsPartition->GetCompartment(2),
					JXWidget::kHElastic, JXWidget::kFixedBottom, 10,95, 380,20);
	warningText->SetToLabel(false);

	auto* parmLabel =
		jnew JXStaticText(JGetString("parmLabel::NonLinearFitDialog::JXLayout"), itsPartition->GetCompartment(3),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 75,20);
	parmLabel->SetToLabel(false);

	auto* varScrollbarSet =
		jnew JXScrollbarSet(itsPartition->GetCompartment(3),
					JXWidget::kHElastic, JXWidget::kVElastic, 100,10, 290,90);
	assert( varScrollbarSet != nullptr );

	itsVarTable =
		jnew VarTable(itsVarList, varScrollbarSet, varScrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 290,90);

	itsNewButton =
		jnew JXTextButton(JGetString("itsNewButton::NonLinearFitDialog::JXLayout"), itsPartition->GetCompartment(3),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 60,20);

	itsDeleteButton =
		jnew JXTextButton(JGetString("itsDeleteButton::NonLinearFitDialog::JXLayout"), itsPartition->GetCompartment(3),
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 60,20);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::NonLinearFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 40,400, 70,20);
	assert( cancelButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::NonLinearFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 165,400, 70,20);

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::NonLinearFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 290,400, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::NonLinearFitDialog::JXLayout"));

	itsNameInput =
		jnew JXInputField(itsPartition->GetCompartment(1),
					JXWidget::kHElastic, JXWidget::kFixedTop, 100,10, 290,20);
	itsNameInput->SetIsRequired();

// end JXLayout

	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);

	ListenTo(itsNewButton);
	ListenTo(itsDeleteButton);
	itsDeleteButton->Deactivate();
}

/******************************************************************************
 GetFunctionString

 ******************************************************************************/

JString
NonLinearFitDialog::GetFunctionString()
	const
{
	const JFunction* f = itsFnEditor->GetFunction();
	return f->Print();
}

/******************************************************************************
 GetDerivativeString

 ******************************************************************************/

JString
NonLinearFitDialog::GetDerivativeString()
	const
{
	const JFunction* f = itsDerivativeEditor->GetFunction();
	return f->Print();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
NonLinearFitDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsNewButton && message.Is(JXButton::kPushed))
	{
		itsVarTable->NewConstant();
	}
	else if (sender == itsDeleteButton && message.Is(JXButton::kPushed))
	{
		if (!OKToDeleteParm())
		{
			itsDeleteButton->Deactivate();
		}
		else
		{
			itsVarTable->RemoveSelectedConstant();
		}
	}
	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
NonLinearFitDialog::OKToDeactivate()
{
	if (!JXModalDialogDirector::OKToDeactivate())
	{
		return false;
	}
	if (Cancelled())
	{
		return true;
	}

	if (itsFnEditor->ContainsUIF())
	{
		JGetUserNotification()->ReportError(JGetString("MissingFunction::NonLinearFitDialog"));
		itsFnEditor->Focus();
		return false;
	}

	return itsDerivativeEditor->EndEditing() && itsVarTable->EndEditing();
}

/******************************************************************************
 OKToDeleteParm (private)

 ******************************************************************************/

bool
NonLinearFitDialog::OKToDeleteParm()
{
	JPoint cell;
	const JCoordinate kXOffset	= 1;
	if (itsVarTable->GetEditedCell(&cell) && itsVarList->OKToRemoveVariable(cell.y + kXOffset))
	{
		return true;
	}
	return false;
}

/******************************************************************************
 GetFitName (public)

 ******************************************************************************/

const JString&
NonLinearFitDialog::GetFitName()
	const
{
	return itsNameInput->GetText()->GetText();
}
