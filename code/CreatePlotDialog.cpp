/******************************************************************************
 CreatePlotDialog.cpp

	BASE CLASS = CreatePlotDialogBase

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "CreatePlotDialog.h"
#include "DataDocument.h"
#include "RaggedFloatTableData.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>

#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CreatePlotDialog::CreatePlotDialog
	(
	DataDocument* supervisor,
	RaggedFloatTableData* data,
	const JIndex startX,
	const JIndex startY,
	const JIndex startXErr,
	const JIndex startYErr
	)
	:
	CreatePlotDialogBase(),
	itsTableDir(supervisor)
{
	BuildWindow(data, startX, startXErr, startY, startYErr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CreatePlotDialog::~CreatePlotDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CreatePlotDialog::BuildWindow
	(
	RaggedFloatTableData* data,
	const JIndex startX,
	const JIndex startY,
	const JIndex startXErr,
	const JIndex startYErr
	)
{

// begin JXLayout

	auto* window = jnew JXWindow(this, 370,160, JString::empty);
	assert( window != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CreatePlotDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,130, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CreatePlotDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CreatePlotDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 180,130, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::CreatePlotDialog::shortcuts::JXLayout"));

	auto* labelInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 115,10, 200,20);
	assert( labelInput != nullptr );

	auto* labelLabel =
		jnew JXStaticText(JGetString("labelLabel::CreatePlotDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 65,10, 50,20);
	assert( labelLabel != nullptr );
	labelLabel->SetToLabel();

	itsXMenu =
		jnew JXTextMenu(JGetString("itsXMenu::CreatePlotDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 160,30);
	assert( itsXMenu != nullptr );

	itsXErrMenu =
		jnew JXTextMenu(JGetString("itsXErrMenu::CreatePlotDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 160,30);
	assert( itsXErrMenu != nullptr );

	itsYErrMenu =
		jnew JXTextMenu(JGetString("itsYErrMenu::CreatePlotDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 200,80, 160,30);
	assert( itsYErrMenu != nullptr );

	itsYMenu =
		jnew JXTextMenu(JGetString("itsYMenu::CreatePlotDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 200,40, 160,30);
	assert( itsYMenu != nullptr );

	auto* plotMenu =
		jnew JXTextMenu(JGetString("plotMenu::CreatePlotDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,120, 130,30);
	assert( plotMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CreatePlotDialog"));
	SetButtons(okButton, cancelButton);

	itsXErrMenu->AppendItem(JGetString("NoneItemLabel::CreatePlotDialog"));
	itsYErrMenu->AppendItem(JGetString("NoneItemLabel::CreatePlotDialog"));

	BuildColumnMenus("Column::global", data->GetDataColCount(),
					 itsXMenu, itsXErrMenu, itsYMenu, itsYErrMenu, nullptr);

	itsStartX = JMax(startX, 1UL);
	itsStartXErr = startXErr + 1;
	itsStartY = JMax(startY, 1UL);
	itsStartYErr = startYErr + 1;

	itsXMenu->SetToPopupChoice(true, itsStartX);
	itsXErrMenu->SetToPopupChoice(true, itsStartXErr);
	itsYMenu->SetToPopupChoice(true, itsStartY);
	itsYErrMenu->SetToPopupChoice(true, itsStartYErr);

	itsXMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsXErrMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsYMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsYErrMenu->SetUpdateAction(JXMenu::kDisableNone);

	ListenTo(itsXMenu);
	ListenTo(itsXErrMenu);
	ListenTo(itsYMenu);
	ListenTo(itsYErrMenu);

	SetObjects(itsTableDir, labelInput, plotMenu);
}

/******************************************************************************
 GetColumns

 ******************************************************************************/

void
CreatePlotDialog::GetColumns
	(
	JIndex* startX,
	JIndex* startXErr,
	JIndex* startY,
	JIndex* startYErr
	)
{
	*startX = itsStartX;
	*startY = itsStartY;
	*startXErr = itsStartXErr - 1;
	*startYErr = itsStartYErr - 1;
}

/******************************************************************************
 GetColumns

 ******************************************************************************/

void
CreatePlotDialog::Receive
	(
	JBroadcaster* sender,
	const Message& message
	)
{
	if (sender == itsXMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection = dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartX = selection->GetIndex();
	}

	else if (sender == itsXErrMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection = dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartXErr = selection->GetIndex();
	}

	else if (sender == itsYMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection = dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartY = selection->GetIndex();
	}

	else if (sender == itsYErrMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection = dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartYErr = selection->GetIndex();
	}

	else
	{
		CreatePlotDialogBase::Receive(sender, message);
	}
}
