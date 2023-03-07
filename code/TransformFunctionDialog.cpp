/******************************************************************************
 TransformFunctionDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "TransformFunctionDialog.h"
#include "VarList.h"
#include "ExprDirector.h"
#include "globals.h"

#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXWindow.h>

#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TransformFunctionDialog::TransformFunctionDialog
	(
	JXDirector* supervisor,
	VarList* list,
	const JSize colCount
	)
	:
	JXModalDialogDirector(supervisor, true)
{
	itsEditor = nullptr;
	BuildWindow();
	itsList = list;

	BuildColumnMenus("Column::global", colCount, itsDestMenu, nullptr);

	for (JSize i = 1; i <= list->GetElementCount(); i++)
	{
		itsVarMenu->AppendItem(list->GetVariableName(i));
	}

	itsDestCol = colCount;
	JString num((JUInt64) itsDestCol);
	JString str = "col[" + num + "] = ";
	itsColNumber->GetText()->SetText(str);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TransformFunctionDialog::~TransformFunctionDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TransformFunctionDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 580,90, JString::empty);
	assert( window != nullptr );

	itsTransformButton =
		jnew JXTextButton(JGetString("itsTransformButton::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 370,55, 80,20);
	assert( itsTransformButton != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 130,55, 80,20);
	assert( itsCloseButton != nullptr );

	itsClearButton =
		jnew JXTextButton(JGetString("itsClearButton::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 250,55, 80,20);
	assert( itsClearButton != nullptr );

	itsFunctionString =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,20, 200,20);
	assert( itsFunctionString != nullptr );

	itsEditButton =
		jnew JXTextButton(JGetString("itsEditButton::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 410,20, 50,20);
	assert( itsEditButton != nullptr );
	itsEditButton->SetShortcuts(JGetString("itsEditButton::TransformFunctionDialog::shortcuts::JXLayout"));

	itsDestMenu =
		jnew JXTextMenu(JGetString("itsDestMenu::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,20, 115,20);
	assert( itsDestMenu != nullptr );

	itsVarMenu =
		jnew JXTextMenu(JGetString("itsVarMenu::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 470,20, 90,20);
	assert( itsVarMenu != nullptr );

	itsColNumber =
		jnew JXStaticText(JGetString("itsColNumber::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 65,20);
	assert( itsColNumber != nullptr );
	itsColNumber->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TransformFunctionDialog"));
	SetButtons(itsTransformButton, itsCloseButton);
	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsDestMenu->SetPopupArrowPosition(JXMenu::kArrowAtLeft);
	itsVarMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsClearButton);
	ListenTo(itsEditButton);
	ListenTo(itsDestMenu);
	ListenTo(itsVarMenu);
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
TransformFunctionDialog::Receive
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
	else if (sender == itsEditor && message.Is(JXModalDialogDirector::kDeactivated))
	{
		const JXModalDialogDirector::Deactivated* info =
			dynamic_cast<const JXModalDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			itsFunctionString->GetText()->SetText(itsEditor->GetString());
		}
		itsEditor = nullptr;
	}
	else if (sender == itsDestMenu && message.Is(JXMenu::kItemSelected))
	{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsDestCol = selection->GetIndex();
		JString num((JUInt64) itsDestCol);
		JString str = "col[" + num + "] = ";
		itsColNumber->GetText()->SetText(str);
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
		JXModalDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 GetDestination

 ******************************************************************************/

JIndex
TransformFunctionDialog::GetDestination()
{
	return itsDestCol;
}

/******************************************************************************
 GetFunctionString

 ******************************************************************************/

const JString&
TransformFunctionDialog::GetFunctionString()
{
	return itsFunctionString->GetText()->GetText();
}

/******************************************************************************
 OKToDeactivate

 ******************************************************************************/

bool
TransformFunctionDialog::OKToDeactivate()
{
	if (Cancelled())
	{
		return true;
	}
	JFunction* f = nullptr;
	if (JParseFunction(itsFunctionString->GetText(), itsList, &f))
	{
		jdelete f;
		return true;
	}
	return false;
}
