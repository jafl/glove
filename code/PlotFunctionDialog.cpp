/******************************************************************************
 PlotFunctionDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "PlotFunctionDialog.h"
#include "VarList.h"
#include "ExprDirector.h"

#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXWindow.h>

#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/jglobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PlotFunctionDialog::PlotFunctionDialog
	(
	JXDirector* supervisor,
	VarList* list
	)
	:
	JXDialogDirector(supervisor, true)
{
	itsEditor = nullptr;
	BuildWindow();
	itsList = list;
	
	for (JIndex i = 1; i <= list->GetElementCount(); i++)
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

	auto* window = jnew JXWindow(this, 500,80, JString::empty);
	assert( window != nullptr );

	itsFunctionString =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 80,10, 220,20);
	assert( itsFunctionString != nullptr );

	auto* fnLabel =
		jnew JXStaticText(JGetString("fnLabel::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 70,20);
	assert( fnLabel != nullptr );
	fnLabel->SetToLabel();

	itsEditButton =
		jnew JXTextButton(JGetString("itsEditButton::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 310,10, 60,20);
	assert( itsEditButton != nullptr );
	itsEditButton->SetShortcuts(JGetString("itsEditButton::PlotFunctionDialog::shortcuts::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 315,50, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::PlotFunctionDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 115,50, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::PlotFunctionDialog::shortcuts::JXLayout"));

	itsClearButton =
		jnew JXTextButton(JGetString("itsClearButton::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 215,50, 70,20);
	assert( itsClearButton != nullptr );

	itsVarMenu =
		jnew JXTextMenu(JGetString("itsVarMenu::PlotFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 380,10, 110,20);
	assert( itsVarMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::PlotFunctionDialog"));
	SetButtons(okButton, cancelButton);
	itsVarMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsClearButton);
	ListenTo(itsEditButton);
	ListenTo(itsVarMenu);
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
PlotFunctionDialog::Receive
	(
	JBroadcaster* sender, 
	const Message& message
	)
{
	if (sender == itsEditButton && message.Is(JXButton::kPushed))
	{
		assert (itsEditor == nullptr);
		itsEditor = jnew ExprDirector(this, itsList, itsFunctionString->GetText()->GetText());
		assert(itsEditor != nullptr);
		ListenTo(itsEditor);
		itsEditor->BeginDialog();
	}
	else if (sender == itsClearButton && message.Is(JXButton::kPushed))
	{
		itsFunctionString->GetText()->SetText(JString::empty);
	}
	else if (sender == itsEditor && message.Is(JXDialogDirector::kDeactivated))
	{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			itsFunctionString->GetText()->SetText(itsEditor->GetString());
		}
		itsEditor = nullptr;
	}
	else if (sender == itsVarMenu && message.Is(JXMenu::kItemSelected))
	{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		JIndex index = selection->GetIndex();
		JString str = itsVarMenu->GetItemText(index);
		itsFunctionString->Paste(str);
	}
	else
	{
		JXDialogDirector::Receive(sender, message);
	}
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
