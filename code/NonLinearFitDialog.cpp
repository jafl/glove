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
#include <jx-af/jx/JXTimerTask.h>
#include <jx-af/jx/JXVertPartition.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXDeleteObjectTask.h>

#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kDeleteButtonUpdateDelay	= 1000;

/******************************************************************************
 Constructor

 ******************************************************************************/

NonLinearFitDialog::NonLinearFitDialog()
	:
	JXModalDialogDirector(true)
{
	itsVarList	= jnew VarList();
	assert(itsVarList != nullptr);

	itsVarList->AddVariable(JGetString("DefaultVarName::global"), 0);

	BuildWindow();

	itsDelButtonTask	= jnew JXTimerTask(kDeleteButtonUpdateDelay);
	assert(itsDelButtonTask != nullptr);
	itsDelButtonTask->Start();
	ListenTo(itsDelButtonTask);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

NonLinearFitDialog::~NonLinearFitDialog()
{
	JXDeleteObjectTask<VarList>::Delete(itsVarList);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
NonLinearFitDialog::BuildWindow()
{
	const JCoordinate kNameInputHeight	= 20;
	const JCoordinate kExprEditorHeight	= 100;
	const JCoordinate kDerTableHeight	= 100;
	const JCoordinate kVarTableHeight	= 100;

	JArray<JCoordinate> heights(3);
	heights.AppendElement(kExprEditorHeight + kNameInputHeight);
	heights.AppendElement(kDerTableHeight);
	heights.AppendElement(kVarTableHeight);

	const JIndex elasticIndex = 3;

	JArray<JCoordinate> minHeights(3);
	minHeights.AppendElement(80);
	minHeights.AppendElement(80);
	minHeights.AppendElement(80);

// begin JXLayout

	auto* window = jnew JXWindow(this, 400,430, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	itsPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,355);
	assert( itsPartition != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::NonLinearFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,400, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::NonLinearFitDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::NonLinearFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 165,400, 70,20);
	assert( itsHelpButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::NonLinearFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 310,400, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::NonLinearFitDialog::shortcuts::JXLayout"));

// end JXLayout

	JXContainer* container = itsPartition->GetCompartment(1);

// begin functionLayout

	const JRect functionLayout_Aperture = container->GetAperture();
	container->AdjustSize(380 - functionLayout_Aperture.width(), 120 - functionLayout_Aperture.height());

	itsNameInput =
		jnew JXInputField(container,
					JXWidget::kHElastic, JXWidget::kFixedTop, 120,0, 260,20);
	assert( itsNameInput != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(container,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,20, 260,100);
	assert( scrollbarSet != nullptr );

	auto* fitNameLabel =
		jnew JXStaticText(JGetString("fitNameLabel::NonLinearFitDialog::functionLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 110,20);
	assert( fitNameLabel != nullptr );
	fitNameLabel->SetToLabel();

	auto* fitFnLabel =
		jnew JXStaticText(JGetString("fitFnLabel::NonLinearFitDialog::functionLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 110,20);
	assert( fitFnLabel != nullptr );
	fitFnLabel->SetToLabel();

	container->SetSize(functionLayout_Aperture.width(), functionLayout_Aperture.height());

// end functionLayout

	itsFnEditor	=
		jnew JXExprEditor(itsVarList, menuBar,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 100, 100);
	assert(itsFnEditor != nullptr);
	itsFnEditor->FitToEnclosure();

	container = itsPartition->GetCompartment(2);

// begin derivativeLayout

	const JRect derivativeLayout_Aperture = container->GetAperture();
	container->AdjustSize(380 - derivativeLayout_Aperture.width(), 100 - derivativeLayout_Aperture.height());

	auto* derivativeLabel =
		jnew JXStaticText(JGetString("derivativeLabel::NonLinearFitDialog::derivativeLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 100,20);
	assert( derivativeLabel != nullptr );
	derivativeLabel->SetToLabel();

	scrollbarSet =
		jnew JXScrollbarSet(container,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,0, 260,100);
	assert( scrollbarSet != nullptr );

	auto* warningText =
		jnew JXStaticText(JGetString("warningText::NonLinearFitDialog::derivativeLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 90,60);
	assert( warningText != nullptr );
	warningText->SetFontSize(JFontManager::GetDefaultFontSize()-2);

	container->SetSize(derivativeLayout_Aperture.width(), derivativeLayout_Aperture.height());

// end derivativeLayout

	itsDerivativeEditor	=
		jnew JXExprEditor(itsVarList, itsFnEditor,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 100, 100);
	assert(itsDerivativeEditor != nullptr);
	itsDerivativeEditor->FitToEnclosure(true, true);

	container = itsPartition->GetCompartment(3);

// begin variableLayout

	const JRect variableLayout_Aperture = container->GetAperture();
	container->AdjustSize(380 - variableLayout_Aperture.width(), 100 - variableLayout_Aperture.height());

	scrollbarSet =
		jnew JXScrollbarSet(container,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,0, 260,100);
	assert( scrollbarSet != nullptr );

	auto* parmLabel =
		jnew JXStaticText(JGetString("parmLabel::NonLinearFitDialog::variableLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 100,20);
	assert( parmLabel != nullptr );
	parmLabel->SetToLabel();

	itsNewButton =
		jnew JXTextButton(JGetString("itsNewButton::NonLinearFitDialog::variableLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,30, 65,20);
	assert( itsNewButton != nullptr );
	itsNewButton->SetShortcuts(JGetString("itsNewButton::NonLinearFitDialog::shortcuts::variableLayout"));

	itsDeleteButton =
		jnew JXTextButton(JGetString("itsDeleteButton::NonLinearFitDialog::variableLayout"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,60, 65,20);
	assert( itsDeleteButton != nullptr );

	container->SetSize(variableLayout_Aperture.width(), variableLayout_Aperture.height());

// end variableLayout

	window->SetTitle(JGetString("WindowTitle::NonLinearFitDialog"));
	UseModalPlacement(false);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);

	itsVarTable	=
		jnew VarTable(itsVarList,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 100, 100);
	assert(itsVarTable != nullptr);
	itsVarTable->FitToEnclosure(true, true);

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
	else if (sender == itsDelButtonTask && message.Is(JXTimerTask::kTimerWentOff))
	{
		if (OKToDeleteParm())
		{
			itsDeleteButton->Activate();
		}
		else
		{
			itsDeleteButton->Deactivate();
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
	JString name	= itsNameInput->GetText()->GetText();
	name.TrimWhitespace();
	if (name.IsEmpty())
	{
		JGetUserNotification()->ReportError(JGetString("MissingName::NonLinearFitDialog"));
		itsNameInput->Focus();
		return false;
	}
	if (itsFnEditor->ContainsUIF())
	{
		JGetUserNotification()->ReportError(JGetString("MissingFunction::NonLinearFitDialog"));
		itsFnEditor->Focus();
		return false;
	}
	if (!itsDerivativeEditor->EndEditing())
	{
		return false;
	}
	if (!itsVarTable->EndEditing())
	{
		return false;
	}
	return true;
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
