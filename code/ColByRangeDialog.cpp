/******************************************************************************
 ColByRangeDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "ColByRangeDialog.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXFloatInput.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXStaticText.h>

#include <jx-af/jcore/JUserNotification.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ColByRangeDialog::ColByRangeDialog
	(
	JXWindowDirector* supervisor,
	const JSize count
	)
	:
	JXModalDialogDirector(supervisor, true)
{
	itsDestCol = count;

	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ColByRangeDialog::~ColByRangeDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
ColByRangeDialog::BuildWindow()
{

// begin JXLayout

	auto* window = jnew JXWindow(this, 210,190, JString::empty);
	assert( window != nullptr );

	itsBeginning =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,55, 100,20);
	assert( itsBeginning != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,160, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::ColByRangeDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,160, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::ColByRangeDialog::shortcuts::JXLayout"));

	itsEnd =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,90, 100,20);
	assert( itsEnd != nullptr );

	auto* maxLabel =
		jnew JXStaticText(JGetString("maxLabel::ColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,90, 70,20);
	assert( maxLabel != nullptr );
	maxLabel->SetToLabel();

	auto* minLabel =
		jnew JXStaticText(JGetString("minLabel::ColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,55, 70,20);
	assert( minLabel != nullptr );
	minLabel->SetToLabel();

	itsCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 95,125, 100,20);
	assert( itsCount != nullptr );

	auto* countLabel =
		jnew JXStaticText(JGetString("countLabel::ColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,125, 70,20);
	assert( countLabel != nullptr );
	countLabel->SetToLabel();

	itsDestMenu =
		jnew JXTextMenu(JGetString("itsDestMenu::ColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 15,15, 180,30);
	assert( itsDestMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::ColByRangeDialog"));
	SetButtons(okButton, cancelButton);

	BuildColumnMenus("Column::global", itsDestCol, itsDestMenu, nullptr);

	itsDestMenu->SetToPopupChoice(true, itsDestCol);
//	itsDestMenu->SetPopupChoice(itsDestCol);
	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDestMenu);

	itsBeginning->SetIsRequired(true);
	itsEnd->SetIsRequired(true);
	itsCount->SetIsRequired(true);
	itsCount->SetLowerLimit(2);
	itsCount->SetValue(2);
}

/******************************************************************************
 GetDestination

 ******************************************************************************/

void
ColByRangeDialog::GetDestination
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
ColByRangeDialog::Receive
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
ColByRangeDialog::GetValues
	(
	JFloat* beg,
	JFloat* end,
	JInteger* count
	)
{
	itsBeginning->GetValue(beg);
	itsEnd->GetValue(end);
	itsCount->GetValue(count);
}

/******************************************************************************
 IsAscending

 ******************************************************************************/

bool
ColByRangeDialog::IsAscending()
{
	return itsIsAscending;
}

/******************************************************************************
 OKToDeactivate

 ******************************************************************************/

bool
ColByRangeDialog::OKToDeactivate()
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

	JFloat beg;
	JFloat end;
	JInteger count;

	itsBeginning->GetValue(&beg);
	itsEnd->GetValue(&end);

	itsIsAscending	= beg < end;
	itsCount->GetValue(&count);

	if (count <= 0)
	{
		JGetUserNotification()->ReportError(JGetString("NoData::ColByIncDialog"));
		return false;
	}

	return true;
}
