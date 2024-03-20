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

#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ColByRangeDialog::ColByRangeDialog
	(
	const JSize count
	)
	:
	JXModalDialogDirector()
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

	auto* window = jnew JXWindow(this, 210,180, JGetString("WindowTitle::ColByRangeDialog::JXLayout"));

	itsDestMenu =
		jnew JXTextMenu(JGetString("itsDestMenu::ColByRangeDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 190,30);

	auto* minLabel =
		jnew JXStaticText(JGetString("minLabel::ColByRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 70,20);
	minLabel->SetToLabel(false);

	auto* maxLabel =
		jnew JXStaticText(JGetString("maxLabel::ColByRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 70,20);
	maxLabel->SetToLabel(false);

	auto* countLabel =
		jnew JXStaticText(JGetString("countLabel::ColByRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 70,20);
	countLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ColByRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,150, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ColByRangeDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 120,150, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::ColByRangeDialog::JXLayout"));

	itsBeginning =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,50, 120,20);

	itsEnd =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,80, 120,20);

	itsCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,110, 120,20);
	itsCount->SetLowerLimit(2);

// end JXLayout

	SetButtons(okButton, cancelButton);

	BuildColumnMenus("Column::global", itsDestCol, itsDestMenu, nullptr);

	itsDestMenu->SetToPopupChoice(true, itsDestCol);
	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDestMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsDestCol = msg.GetIndex();
	}));

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
