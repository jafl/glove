/******************************************************************************
 ColByIncDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "ColByIncDialog.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXFloatInput.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXStaticText.h>

#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ColByIncDialog::ColByIncDialog
	(
	const JSize count
	)
	:
	JXModalDialogDirector()
{
	BuildWindow();

	BuildColumnMenus("Column::global", count, itsDestMenu, nullptr);

	itsDestMenu->SetToPopupChoice(true, count);
	itsDestCol = count;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ColByIncDialog::~ColByIncDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
ColByIncDialog::BuildWindow()
{

// begin JXLayout

	auto* window = jnew JXWindow(this, 210,190, JString::empty);
	assert( window != nullptr );

	itsBeginning =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 95,55, 100,20);
	assert( itsBeginning != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ColByIncDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,160, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::ColByIncDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ColByIncDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::ColByIncDialog::shortcuts::JXLayout"));

	itsInc =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 95,90, 100,20);
	assert( itsInc != nullptr );

	auto* incrLabel =
		jnew JXStaticText(JGetString("incrLabel::ColByIncDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,90, 70,20);
	assert( incrLabel != nullptr );
	incrLabel->SetToLabel();

	auto* minLabel =
		jnew JXStaticText(JGetString("minLabel::ColByIncDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,55, 70,20);
	assert( minLabel != nullptr );
	minLabel->SetToLabel();

	itsCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 95,125, 100,20);
	assert( itsCount != nullptr );

	auto* countLabel =
		jnew JXStaticText(JGetString("countLabel::ColByIncDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,125, 70,20);
	assert( countLabel != nullptr );
	countLabel->SetToLabel();

	itsDestMenu =
		jnew JXTextMenu(JGetString("itsDestMenu::ColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 15,15, 180,30);
	assert( itsDestMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::ColByIncDialog"));
	SetButtons(okButton, cancelButton);

	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDestMenu);

	itsBeginning->SetIsRequired(true);
	itsInc->SetIsRequired(true);
	itsCount->SetIsRequired(true);
	itsCount->SetLowerLimit(2);
	itsCount->SetValue(2);
}

/******************************************************************************
 GetDestination

 ******************************************************************************/

void
ColByIncDialog::GetDestination
	(
	JIndex* dest
	)
{
	*dest = itsDestCol;
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
ColByIncDialog::Receive
	(
	JBroadcaster* sender,
	const Message& message
	)
{
	if (sender == itsDestMenu && message.Is(JXMenu::kItemSelected))
	{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsDestCol = selection->GetIndex();
	}

	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
ColByIncDialog::GetValues
	(
	JFloat* beg,
	JFloat* inc,
	JInteger* count
	)
{
	itsBeginning->GetValue(beg);
	itsInc->GetValue(inc);
	itsCount->GetValue(count);
}

/******************************************************************************
 IsAscending

 ******************************************************************************/

bool
ColByIncDialog::IsAscending()
{
	JFloat inc;
	itsInc->GetValue(&inc);
	if (inc > 0)
	{
		return true;
	}
	return false;
}

/******************************************************************************
 OKToDeactivate

 ******************************************************************************/

bool
ColByIncDialog::OKToDeactivate()
{
	if (Cancelled())
	{
		return true;
	}

	if (itsDestCol == 0)
	{
		JGetUserNotification()->ReportError(JGetString("MissingDestCol::ColByIncDialog"));
		return false;
	}

	JInteger count;

	itsCount->GetValue(&count);

	if (count <= 0)
	{
		JGetUserNotification()->ReportError(JGetString("NoData::ColByIncDialog"));
		return false;
	}

	return true;
}
