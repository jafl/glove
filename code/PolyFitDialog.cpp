/******************************************************************************
 PolyFitDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2000 by Glenn Bach.

 ******************************************************************************/

#include "PolyFitDialog.h"
#include "VarList.h"

#include "globals.h"

#include <jx-af/jexpr/JXExprEditor.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXWindow.h>

#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kDeleteButtonUpdateDelay	= 1000;

/******************************************************************************
 Constructor

 ******************************************************************************/

PolyFitDialog::PolyFitDialog
	(
	JXDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, true)
{
	itsVarList	= jnew VarList();
	assert(itsVarList != nullptr);

	itsVarList->AddVariable(JGetString("DefaultVarName::global"), 0);
	const JSize count	= 10;
	for (JIndex i = 1; i <= count; i++)
	{
		JString parm	= "a" + JString((JUInt64) i - 1);
		itsVarList->AddVariable(parm, 0);
	}

	BuildWindow();

}

/******************************************************************************
 Destructor

 ******************************************************************************/

PolyFitDialog::~PolyFitDialog()
{
	// jdelete	itsVarList;
	// I need to find a way to safely delete this.
}

/******************************************************************************


 ******************************************************************************/

void
PolyFitDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 380,450, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 340,100);
	assert( scrollbarSet != nullptr );

	itsCB[0] =
		jnew JXTextCheckbox(JGetString("itsCB[0]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 185,155, 110,20);
	assert( itsCB[0] != nullptr );

	itsCB[1] =
		jnew JXTextCheckbox(JGetString("itsCB[1]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 185,180, 110,20);
	assert( itsCB[1] != nullptr );

	itsCB[2] =
		jnew JXTextCheckbox(JGetString("itsCB[2]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 185,205, 110,20);
	assert( itsCB[2] != nullptr );

	itsCB[3] =
		jnew JXTextCheckbox(JGetString("itsCB[3]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 185,230, 110,20);
	assert( itsCB[3] != nullptr );

	itsCB[4] =
		jnew JXTextCheckbox(JGetString("itsCB[4]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 185,255, 110,20);
	assert( itsCB[4] != nullptr );

	itsCB[5] =
		jnew JXTextCheckbox(JGetString("itsCB[5]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 185,280, 110,20);
	assert( itsCB[5] != nullptr );

	itsCB[6] =
		jnew JXTextCheckbox(JGetString("itsCB[6]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 185,305, 110,20);
	assert( itsCB[6] != nullptr );

	itsCB[7] =
		jnew JXTextCheckbox(JGetString("itsCB[7]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 185,330, 110,20);
	assert( itsCB[7] != nullptr );

	itsCB[8] =
		jnew JXTextCheckbox(JGetString("itsCB[8]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 185,355, 110,20);
	assert( itsCB[8] != nullptr );

	itsCB[9] =
		jnew JXTextCheckbox(JGetString("itsCB[9]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 185,380, 110,20);
	assert( itsCB[9] != nullptr );

	auto* powersLabel =
		jnew JXStaticText(JGetString("powersLabel::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 90,158, 65,20);
	assert( powersLabel != nullptr );
	powersLabel->SetToLabel();

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 25,415, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::PolyFitDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 155,415, 70,20);
	assert( itsHelpButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 285,415, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::PolyFitDialog::shortcuts::JXLayout"));

	itsNameInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 90,15, 270,20);
	assert( itsNameInput != nullptr );

	auto* fitNameLabel =
		jnew JXStaticText(JGetString("fitNameLabel::PolyFitDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,15, 65,20);
	assert( fitNameLabel != nullptr );
	fitNameLabel->SetToLabel();

// end JXLayout

	itsFn =
		jnew JXExprEditor(itsVarList,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 100, 100);
	assert(itsFn != nullptr);
	itsFn->FitToEnclosure(true, true);
	itsFn->Hide();

	ListenTo(itsHelpButton);

	const JSize count = 10;
	for (JIndex i = 1; i <= count; i++)
	{
		ListenTo(itsCB[i-1]);
	}

	window->SetTitle(JGetString("WindowTitle::PolyFitDialog"));
	UseModalPlacement(false);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);
}

/******************************************************************************
 GetPowers

 ******************************************************************************/

void
PolyFitDialog::GetPowers
	(
	JArray<JIndex>* powers
	)
{
	const JSize count	= 10;
	for (JIndex i = 1; i <= count; i++)
	{
		if (itsCB[i-1]->IsChecked())
		{
			powers->AppendElement(i - 1);
		}
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
PolyFitDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
	{
	}
	else if (message.Is(JXCheckbox::kPushed))
	{
		JString fStr;
		bool started	= false;
		const JSize count	= 10;
		for (JIndex i = 1; i <= count; i++)
		{
			if (itsCB[i-1]->IsChecked())
			{
				if (started)
				{
					fStr += " + ";
				}
				else
				{
					started	= true;
				}
				JString parm	= "a" + JString((JUInt64) i - 1);
				JString xTerm	= " * " + JGetString("DefaultVarName::global");
				if (i > 2)
				{
					xTerm += "^" + JString((JUInt64) i - 1);
				}
				fStr += parm;
				if (i > 1)
				{
					fStr += xTerm;
				}
			}
		}
		if (fStr.IsEmpty())
		{
			itsFn->Hide();
		}
		else
		{
			itsFn->Show();

			JExprParser p(itsVarList);

			JFunction* f;
			if (p.Parse(fStr, &f))
			{
				itsFn->SetFunction(itsVarList, f);
			}
		}
	}
	else
	{
		JXDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
PolyFitDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
	{
		return false;
	}
	if (Cancelled())
	{
		return true;
	}
	JString name = itsNameInput->GetText()->GetText();
	name.TrimWhitespace();
	if (name.IsEmpty())
	{
		JGetUserNotification()->ReportError(JGetString("MissingName::NonLinearFitDialog"));
		itsNameInput->Focus();
		return false;
	}

	bool checked	= false;
	const JSize count	= 10;
	for (JIndex i = 1; i <= count; i++)
	{
		if (itsCB[i-1]->IsChecked())
		{
			checked		= true;
			break;
		}
	}

	if (!checked)
	{
		JGetUserNotification()->ReportError(JGetString("MissingPower::PolyFitDialog"));
	}

	return checked;
}

/******************************************************************************
 GetFitName (public)

 ******************************************************************************/

const JString&
PolyFitDialog::GetFitName()
	const
{
	return itsNameInput->GetText()->GetText();
}
