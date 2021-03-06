/******************************************************************************
 ExprDirector.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "ExprDirector.h"
#include "VarList.h"
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jexpr/JXExprEditor.h>
#include <jx-af/jexpr/JXExprEditorSet.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jcore/jglobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ExprDirector::ExprDirector
	(
	JXDirector* supervisor,
	const VarList* list,
	const JString& function
	)
	:
	JXDialogDirector(supervisor, true)
{
	auto* window = jnew JXWindow(this, 300,240, JGetString("WindowTitle::ExprDirector"));
	assert( window != nullptr );

	auto* set =
		jnew JXExprEditorSet(list, &itsEditor, window,
		JXWidget::kHElastic, JXWidget::kVElastic,0,0,300,200);
	assert (set != nullptr);
	assert (itsEditor != nullptr);

	JExprParser p(itsEditor);

	JFunction* f;
	if (!function.IsEmpty() && p.Parse(function, &f))
	{
		itsEditor->SetFunction(list, f);
	}

	auto* ok = jnew JXTextButton(JGetString("OKLabel::JXGlobal"), window,
						JXWidget::kHElastic, JXWidget::kVElastic,60,210,70,20);
	assert (ok != nullptr);
	ok->SetShortcuts(JGetString("OKShortcut::JXGlobal"));

	auto* cancel = jnew JXTextButton(JGetString("CancelLabel::JXGlobal"), window,
						JXWidget::kHElastic, JXWidget::kVElastic,170,210,70,20);
	assert (cancel != nullptr);
	cancel->SetShortcuts(JGetString("CancelShortcut::JXGlobal"));

	SetButtons(ok,cancel);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ExprDirector::~ExprDirector()
{
}

/******************************************************************************
 GetString

 ******************************************************************************/

JString
ExprDirector::GetString()
{
	const JFunction* f = itsEditor->GetFunction();
	return f->Print();
}
