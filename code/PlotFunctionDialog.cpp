/******************************************************************************
 PlotFunctionDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "PlotFunctionDialog.h"
#include "VarList.h"
#include "EditExprDialog.h"

#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXWindow.h>

#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PlotFunctionDialog::PlotFunctionDialog
	(
	VarList* list
	)
	:
	JXModalDialogDirector(),
	itsList(list)
{
	BuildWindow();

	for (JIndex i = 1; i <= list->GetItemCount(); i++)
	{
		itsVarMenu->AppendItem(list->GetVariableName(i));
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PlotFunctionDialog::~PlotFunctionDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
PlotFunctionDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 480,80, JGetString("WindowTitle::PlotFunctionDialog::JXLayout"));

	auto* fnLabel =
		jnew JXStaticText(JGetString("fnLabel::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 60,20);
	fnLabel->SetToLabel(false);

	itsEditButton =
		jnew JXTextButton(JGetString("itsEditButton::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,10, 60,20);
	itsEditButton->SetShortcuts(JGetString("itsEditButton::shortcuts::PlotFunctionDialog::JXLayout"));

	itsVarMenu =
		jnew JXTextMenu(JGetString("itsVarMenu::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 370,10, 100,20);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,50, 70,20);
	assert( cancelButton != nullptr );

	itsClearButton =
		jnew JXTextButton(JGetString("itsClearButton::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 210,50, 70,20);

	okButton =
		jnew JXTextButton(JGetString("okButton::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 330,50, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::PlotFunctionDialog::JXLayout"));

	itsFunctionString =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,10, 220,20);
	itsFunctionString->SetIsRequired();

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsVarMenu->SetUpdateAction(JXMenu::kDisableNone);

	ListenTo(itsClearButton, std::function([this](const JXButton::Pushed&)
	{
		itsFunctionString->GetText()->SetText(JString::empty);
	}));

	ListenTo(itsEditButton, std::function([this](const JXButton::Pushed&)
	{
		auto* dlog = jnew EditExprDialog(itsList, itsFunctionString->GetText()->GetText());
		if (dlog->DoDialog())
		{
			itsFunctionString->GetText()->SetText(dlog->GetString());
		}
	}));

	ListenTo(itsVarMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsFunctionString->Paste(itsVarMenu->GetItemText(msg.GetIndex()));
	}));
}

/******************************************************************************
 GetFunctionString

 ******************************************************************************/

const JString&
PlotFunctionDialog::GetFunctionString()
{
	return itsFunctionString->GetText()->GetText();
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
PlotFunctionDialog::OKToDeactivate()
{
	if (Cancelled())
	{
		return true;
	}

	JExprParser p(itsList);

	JFunction* f;
	if (p.Parse(itsFunctionString->GetText()->GetText(), &f))
	{
		jdelete f;
		return true;
	}
	return false;
}
