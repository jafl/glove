/******************************************************************************
 EditExprDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "EditExprDialog.h"
#include "VarList.h"
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jexpr/JXExprEditor.h>
#include <jx-af/jexpr/JXExprEditorSet.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

EditExprDialog::EditExprDialog
	(
	const VarList* list,
	const JString& function
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(list, function);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EditExprDialog::~EditExprDialog()
{
}

/******************************************************************************
 Constructor

 ******************************************************************************/

void
EditExprDialog::BuildWindow
	(
	const VarList* list,
	const JString& function
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 300,240, JString::empty);

	auto* set =
		jnew JXExprEditorSet(list, &itsEditor, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 300,200);
	assert( set != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::EditExprDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,210, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::EditExprDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 170,210, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::EditExprDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::EditExprDialog"));
	SetButtons(okButton, cancelButton);

	assert (itsEditor != nullptr);
	JExprParser p(itsEditor);

	JFunction* f;
	if (!function.IsEmpty() && p.Parse(function, &f))
	{
		itsEditor->SetFunction(list, f);
	}
}

/******************************************************************************
 GetString

 ******************************************************************************/

JString
EditExprDialog::GetString()
{
	const JFunction* f = itsEditor->GetFunction();
	return f->Print();
}
