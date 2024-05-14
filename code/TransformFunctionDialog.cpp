/******************************************************************************
 TransformFunctionDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "TransformFunctionDialog.h"
#include "VarList.h"
#include "EditExprDialog.h"
#include "globals.h"

#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXWindow.h>

#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TransformFunctionDialog::TransformFunctionDialog
	(
	VarList* list,
	const JSize colCount
	)
	:
	JXModalDialogDirector(),
	itsList(list)
{
	BuildWindow();

	BuildColumnMenus("Column::global", colCount, itsDestMenu, nullptr);

	for (JSize i = 1; i <= list->GetItemCount(); i++)
	{
		itsVarMenu->AppendItem(list->GetVariableName(i));
	}

	itsDestCol = colCount;
	JString num(itsDestCol);
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

	auto* window = jnew JXWindow(this, 570,80, JGetString("WindowTitle::TransformFunctionDialog::JXLayout"));

	itsDestMenu =
		jnew JXTextMenu(JGetString("itsDestMenu::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 120,20);
	itsDestMenu->SetPopupArrowPosition(JXMenu::kArrowAtLeft);

	itsColNumber =
		jnew JXStaticText(JGetString("itsColNumber::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,10, 60,20);
	itsColNumber->SetToLabel(false);

	itsEditButton =
		jnew JXTextButton(JGetString("itsEditButton::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 400,10, 50,20);
	itsEditButton->SetShortcuts(JGetString("itsEditButton::shortcuts::TransformFunctionDialog::JXLayout"));

	itsVarMenu =
		jnew JXTextMenu(JGetString("itsVarMenu::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 460,10, 100,20);

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,50, 80,20);

	itsClearButton =
		jnew JXTextButton(JGetString("itsClearButton::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,50, 80,20);

	itsTransformButton =
		jnew JXTextButton(JGetString("itsTransformButton::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 400,50, 80,20);
	itsTransformButton->SetShortcuts(JGetString("itsTransformButton::shortcuts::TransformFunctionDialog::JXLayout"));

	itsFunctionString =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 190,10, 200,20);
	itsFunctionString->SetIsRequired();

// end JXLayout

	SetButtons(itsTransformButton, itsCloseButton);

	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsDestMenu->SetPopupArrowPosition(JXMenu::kArrowAtLeft);
	ListenTo(itsDestMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsDestCol = msg.GetIndex();
		JString num(itsDestCol);
		JString str = "col[" + num + "] =";
		itsColNumber->GetText()->SetText(str);
	}));

	itsVarMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsVarMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsFunctionString->Paste(itsVarMenu->GetItemText(msg.GetIndex()));
	}));

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

	JExprParser p(itsList);

	const JString& s = GetFunctionString();

	JFunction* f;
	if (!s.IsEmpty() && p.Parse(s, &f))
	{
		jdelete f;
		return true;
	}

	JGetUserNotification()->ReportError(JGetString("ParseFailed::RaggedFloatTable"));
	return false;
}
