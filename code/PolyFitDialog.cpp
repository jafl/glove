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

	auto* window = jnew JXWindow(this, 380,220, JString::empty);

	itsCB[0] =
		jnew JXTextCheckbox(JGetString("itsCB[0]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 85,50, 100,20);
	assert( itsCB[0] != nullptr );

	itsCB[1] =
		jnew JXTextCheckbox(JGetString("itsCB[1]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 85,75, 40,20);
	assert( itsCB[1] != nullptr );

	itsCB[2] =
		jnew JXTextCheckbox(JGetString("itsCB[2]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 85,100, 40,20);
	assert( itsCB[2] != nullptr );

	itsCB[3] =
		jnew JXTextCheckbox(JGetString("itsCB[3]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 85,125, 40,20);
	assert( itsCB[3] != nullptr );

	itsCB[4] =
		jnew JXTextCheckbox(JGetString("itsCB[4]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 85,150, 40,20);
	assert( itsCB[4] != nullptr );

	itsCB[5] =
		jnew JXTextCheckbox(JGetString("itsCB[5]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 225,50, 40,20);
	assert( itsCB[5] != nullptr );

	itsCB[6] =
		jnew JXTextCheckbox(JGetString("itsCB[6]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 225,75, 40,20);
	assert( itsCB[6] != nullptr );

	itsCB[7] =
		jnew JXTextCheckbox(JGetString("itsCB[7]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 225,100, 40,20);
	assert( itsCB[7] != nullptr );

	itsCB[8] =
		jnew JXTextCheckbox(JGetString("itsCB[8]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 225,125, 40,20);
	assert( itsCB[8] != nullptr );

	itsCB[9] =
		jnew JXTextCheckbox(JGetString("itsCB[9]::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 225,150, 40,20);
	assert( itsCB[9] != nullptr );

	auto* powersLabel =
		jnew JXStaticText(JGetString("powersLabel::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 65,20);
	assert( powersLabel != nullptr );
	powersLabel->SetToLabel();

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 25,185, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::PolyFitDialog::shortcuts::JXLayout"));

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 155,185, 70,20);
	assert( itsHelpButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 285,185, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::PolyFitDialog::shortcuts::JXLayout"));

	itsNameInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 90,15, 270,20);
	assert( itsNameInput != nullptr );

	auto* fitNameLabel =
		jnew JXStaticText(JGetString("fitNameLabel::PolyFitDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,15, 65,20);
	assert( fitNameLabel != nullptr );
	fitNameLabel->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::PolyFitDialog"));
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
