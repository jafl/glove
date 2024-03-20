/******************************************************************************
 PolyFitDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2000 by Glenn Bach.

 ******************************************************************************/

#include "PolyFitDialog.h"
#include "VarList.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>

#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PolyFitDialog::PolyFitDialog()
	:
	JXModalDialogDirector()
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PolyFitDialog::~PolyFitDialog()
{
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
PolyFitDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 330,190, JGetString("WindowTitle::PolyFitDialog::JXLayout"));

	auto* fitNameLabel =
		jnew JXStaticText(JGetString("fitNameLabel::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 70,20);
	fitNameLabel->SetToLabel(false);

	auto* powersLabel =
		jnew JXStaticText(JGetString("powersLabel::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 70,20);
	powersLabel->SetToLabel(false);

	itsCB[0] =
		jnew JXTextCheckbox(JGetString("itsCB[0]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,40, 110,20);

	itsCB[5] =
		jnew JXTextCheckbox(JGetString("itsCB[5]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,40, 110,20);

	itsCB[1] =
		jnew JXTextCheckbox(JGetString("itsCB[1]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,60, 110,20);

	itsCB[6] =
		jnew JXTextCheckbox(JGetString("itsCB[6]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,60, 110,20);

	itsCB[2] =
		jnew JXTextCheckbox(JGetString("itsCB[2]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,80, 110,20);

	itsCB[7] =
		jnew JXTextCheckbox(JGetString("itsCB[7]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,80, 110,20);

	itsCB[3] =
		jnew JXTextCheckbox(JGetString("itsCB[3]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,100, 110,20);

	itsCB[8] =
		jnew JXTextCheckbox(JGetString("itsCB[8]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,100, 110,20);

	itsCB[4] =
		jnew JXTextCheckbox(JGetString("itsCB[4]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,120, 110,20);

	itsCB[9] =
		jnew JXTextCheckbox(JGetString("itsCB[9]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,120, 110,20);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 70,20);
	assert( cancelButton != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,160, 70,20);

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,160, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::PolyFitDialog::JXLayout"));

	itsNameInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,10, 240,20);

// end JXLayout

	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);
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
	JIndex i = 0;
	for (auto* cb : itsCB)
	{
		if (cb->IsChecked())
		{
			powers->AppendItem(i);
		}
		i++;
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
	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
PolyFitDialog::OKToDeactivate()
{
	if (!JXModalDialogDirector::OKToDeactivate())
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

	const bool checked = std::any_of(std::begin(itsCB), std::end(itsCB), [](auto* cb)
	{
		return cb->IsChecked();
	});

	if (!checked)
	{
		JGetUserNotification()->ReportError(JGetString("MissingPower::PolyFitDialog"));
	}

	return checked;
}

/******************************************************************************
 GetFitName

 ******************************************************************************/

const JString&
PolyFitDialog::GetFitName()
	const
{
	return itsNameInput->GetText()->GetText();
}
