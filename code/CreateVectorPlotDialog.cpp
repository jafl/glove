/******************************************************************************
 CreateVectorPlotDialog.cpp

	BASE CLASS = CreatePlotDialogBase

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "CreateVectorPlotDialog.h"
#include "DataDocument.h"
#include "RaggedFloatTableData.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>

#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CreateVectorPlotDialog::CreateVectorPlotDialog
	(
	DataDocument* supervisor,
	RaggedFloatTableData* data,
	const JIndex startX,
	const JIndex startY,
	const JIndex startX2,
	const JIndex startY2
	)
	:
	CreatePlotDialogBase(),
	itsTableDir(supervisor)
{
	BuildWindow(data, startX, startY, startX2, startY2);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CreateVectorPlotDialog::~CreateVectorPlotDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CreateVectorPlotDialog::BuildWindow
	(
	RaggedFloatTableData* data,
	const JIndex startX,
	const JIndex startY,
	const JIndex startX2,
	const JIndex startY2
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,160, JString::empty);
	assert( window != nullptr );

	itsX1Menu =
		jnew JXTextMenu(JGetString("itsX1Menu::CreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 160,30);
	assert( itsX1Menu != nullptr );

	itsY1Menu =
		jnew JXTextMenu(JGetString("itsY1Menu::CreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 200,40, 160,30);
	assert( itsY1Menu != nullptr );

	itsX2Menu =
		jnew JXTextMenu(JGetString("itsX2Menu::CreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 160,30);
	assert( itsX2Menu != nullptr );

	itsY2Menu =
		jnew JXTextMenu(JGetString("itsY2Menu::CreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 200,80, 160,30);
	assert( itsY2Menu != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,130, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CreateVectorPlotDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 190,130, 70,20);
	assert( cancelButton != nullptr );

	auto* plotMenu =
		jnew JXTextMenu(JGetString("plotMenu::CreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,120, 130,30);
	assert( plotMenu != nullptr );

	auto* labelInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 115,10, 200,20);
	assert( labelInput != nullptr );

	auto* label =
		jnew JXStaticText(JGetString("label::CreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 65,10, 50,20);
	assert( label != nullptr );
	label->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CreateVectorPlotDialog"));
	SetButtons(okButton, cancelButton);

	BuildColumnMenus("Column::global", data->GetDataColCount(),
					   itsX1Menu, itsX2Menu, itsY1Menu, itsY2Menu, nullptr);

	itsStartX1 = startX;
	if (startX == 0)
	{
		itsStartX1 = 1;
	}

	itsStartX2 = startX2;
	if (startX2 == 0)
	{
		itsStartX2 = 1;
	}

	itsStartY1 = startY;
	if (startY == 0)
	{
		itsStartY1 = 1;
	}

	itsStartY2 = startY2;
	if (startY2 == 0)
	{
		itsStartY2 = 1;
	}

	itsX1Menu->SetToPopupChoice(true, itsStartX1);
	itsX2Menu->SetToPopupChoice(true, itsStartX2);
	itsY1Menu->SetToPopupChoice(true, itsStartY1);
	itsY2Menu->SetToPopupChoice(true, itsStartY2);

	itsX1Menu->SetUpdateAction(JXMenu::kDisableNone);
	itsX2Menu->SetUpdateAction(JXMenu::kDisableNone);
	itsY1Menu->SetUpdateAction(JXMenu::kDisableNone);
	itsY2Menu->SetUpdateAction(JXMenu::kDisableNone);

	ListenTo(itsX1Menu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsStartX1 = msg.GetIndex();
	}));

	ListenTo(itsX2Menu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsStartX2 = msg.GetIndex();
	}));

	ListenTo(itsY1Menu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsStartY1 = msg.GetIndex();
	}));

	ListenTo(itsY2Menu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsStartY2 = msg.GetIndex();
	}));

	SetObjects(itsTableDir, labelInput, plotMenu);
}

/******************************************************************************
 GetColumns

 ******************************************************************************/

void
CreateVectorPlotDialog::GetColumns
	(
	JIndex* X1,
	JIndex* X2,
	JIndex* Y1,
	JIndex* Y2
	)
{
	*X1 = itsStartX1;
	*Y1 = itsStartY1;
	*X2 = itsStartX2;
	*Y2 = itsStartY2;
}
