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
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 370,55, 80,20);
	assert( itsTransformButton != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,55, 80,20);
	assert( itsCloseButton != nullptr );

	itsClearButton =
		jnew JXTextButton(JGetString("itsClearButton::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,55, 80,20);
	assert( itsClearButton != nullptr );

	itsFunctionString =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 200,20, 200,20);
	assert( itsFunctionString != nullptr );

	itsEditButton =
		jnew JXTextButton(JGetString("itsEditButton::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 410,20, 50,20);
	assert( itsEditButton != nullptr );
	itsEditButton->SetShortcuts(JGetString("itsEditButton::TransformFunctionDialog::shortcuts::JXLayout"));

	itsDestMenu =
		jnew JXTextMenu(JGetString("itsDestMenu::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 115,20);
	assert( itsDestMenu != nullptr );

	itsVarMenu =
		jnew JXTextMenu(JGetString("itsVarMenu::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 470,20, 90,20);
	assert( itsVarMenu != nullptr );

	itsColNumber =
		jnew JXStaticText(JGetString("itsColNumber::TransformFunctionDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 135,20, 65,20);
	assert( itsColNumber != nullptr );
	itsColNumber->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TransformFunctionDialog"));
	SetButtons(itsTransformButton, itsCloseButton);

	itsDestMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsDestMenu->SetPopupArrowPosition(JXMenu::kArrowAtLeft);
	ListenTo(itsDestMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsDestCol = msg.GetIndex();
		JString num((JUInt64) itsDestCol);
		JString str = "col[" + num + "] = ";
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
		assert(dlog != nullptr);
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
