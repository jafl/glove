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

	auto* window = jnew JXWindow(this, 210,180, JGetString("WindowTitle::ColByIncDialog::JXLayout"));

	itsDestMenu =
		jnew JXTextMenu(JGetString("itsDestMenu::ColByIncDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 190,30);

	auto* minLabel =
		jnew JXStaticText(JGetString("minLabel::ColByIncDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 70,20);
	minLabel->SetToLabel(false);

	auto* incLabel =
		jnew JXStaticText(JGetString("incLabel::ColByIncDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 70,20);
	incLabel->SetToLabel(false);

	auto* countLabel =
		jnew JXStaticText(JGetString("countLabel::ColByIncDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 70,20);
	countLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ColByIncDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,150, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::ColByIncDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 120,150, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::ColByIncDialog::JXLayout"));

	itsBeginning =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,50, 120,20);

	itsInc =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,80, 120,20);

	itsCount =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 80,110, 120,20);
	itsCount->SetLowerLimit(2);

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDestMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsDestCol = msg.GetIndex();
	}));

	itsInc->SetValue(1);
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
