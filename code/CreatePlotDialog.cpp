/******************************************************************************
 CreatePlotDialog.cpp

	BASE CLASS = JXDialogDirector

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

#include <jx-af/jcore/JPtrArray.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JUserNotification.h>
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
	JXDialogDirector(supervisor, true)
{
	itsTableDir = supervisor;
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
					JXWidget::kHElastic, JXWidget::kVElastic, 280,130, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CreatePlotDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 180,130, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::CreatePlotDialog::shortcuts::JXLayout"));

	itsLabelInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 115,10, 200,20);
	assert( itsLabelInput != nullptr );

	auto* labelLabel =
		jnew JXStaticText(JGetString("labelLabel::CreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 65,10, 50,20);
	assert( labelLabel != nullptr );
	labelLabel->SetToLabel();

	itsXMenu =
		jnew JXTextMenu(JGetString("itsXMenu::CreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,40, 160,30);
	assert( itsXMenu != nullptr );

	itsXErrMenu =
		jnew JXTextMenu(JGetString("itsXErrMenu::CreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,80, 160,30);
	assert( itsXErrMenu != nullptr );

	itsYErrMenu =
		jnew JXTextMenu(JGetString("itsYErrMenu::CreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,80, 160,30);
	assert( itsYErrMenu != nullptr );

	itsYMenu =
		jnew JXTextMenu(JGetString("itsYMenu::CreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,40, 160,30);
	assert( itsYMenu != nullptr );

	itsPlotsMenu =
		jnew JXTextMenu(JGetString("itsPlotsMenu::CreatePlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,120, 130,30);
	assert( itsPlotsMenu != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CreatePlotDialog"));
	SetButtons(okButton, cancelButton);
	
	itsXErrMenu->AppendItem(JGetString("NoneItemLabel::CreatePlotDialog"));
	itsYErrMenu->AppendItem(JGetString("NoneItemLabel::CreatePlotDialog"));

	BuildColumnMenus("Column::global", data->GetDataColCount(),
					   itsXMenu, itsXErrMenu, itsYMenu, itsYErrMenu, nullptr);
	
	itsStartX = startX;
	if (startX == 0)
	{
		itsStartX = 1;
	}
		
	itsStartXErr = startXErr + 1;
		
	itsStartY = startY;
	if (startY == 0)
	{
		itsStartY = 1;
	}
		
	itsStartYErr = startYErr + 1;
	
	JPtrArray<JString> names(JPtrArrayT::kDeleteAll);
	itsTableDir->GetPlotNames(names);
	
	itsPlotsMenu->AppendItem(JGetString("NewPlotItemLabel::global"));
	
	const JSize strCount = names.GetElementCount();
	
	for (JSize i = 1; i <= strCount; i++)
	{
		itsPlotsMenu->AppendItem(*(names.GetElement(i)));
	}

	itsPlotsMenu->ShowSeparatorAfter(1, true);

	itsPlotIndex = 1;
	
	itsXMenu->SetToPopupChoice(true, itsStartX);
	itsXErrMenu->SetToPopupChoice(true, itsStartXErr);
	itsYMenu->SetToPopupChoice(true, itsStartY);
	itsYErrMenu->SetToPopupChoice(true, itsStartYErr);
	itsPlotsMenu->SetToPopupChoice(true, itsPlotIndex);
	
	itsXMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsXErrMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsYMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsYErrMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPlotsMenu->SetUpdateAction(JXMenu::kDisableNone);
	
	ListenTo(itsXMenu);
	ListenTo(itsXErrMenu);
	ListenTo(itsYMenu);
	ListenTo(itsYErrMenu);
	ListenTo(itsPlotsMenu);

	itsLabelInput->GetText()->SetText(JGetString("DefaultLabel::global"));
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
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartX = selection->GetIndex();
	}
		
	else if (sender == itsXErrMenu && message.Is(JXMenu::kItemSelected))
	{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartXErr = selection->GetIndex();
	}
		
	else if (sender == itsYMenu && message.Is(JXMenu::kItemSelected))
	{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartY = selection->GetIndex();
	}
		
	else if (sender == itsYErrMenu && message.Is(JXMenu::kItemSelected))
	{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartYErr = selection->GetIndex();
	}
		
	else if (sender == itsPlotsMenu && message.Is(JXMenu::kItemSelected))
	{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsPlotIndex = selection->GetIndex();
	}
		
	else
	{
		JXDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 GetPlotIndex 

 ******************************************************************************/

bool 
CreatePlotDialog::GetPlotIndex
	(
	JIndex* index
	)
{
	if (itsPlotIndex == 1)
	{
		return false;
	}
		
	*index = itsPlotIndex - 1;
	return true;
}

/******************************************************************************
 GetPlotIndex 

 ******************************************************************************/

const JString& 
CreatePlotDialog::GetLabel()
{
	return itsLabelInput->GetText()->GetText();
}	

/******************************************************************************
 OKToDeactivate 

 ******************************************************************************/

bool 
CreatePlotDialog::OKToDeactivate()
{
	if (Cancelled())
	{
		return true;
	}
	if (GetLabel().IsEmpty())
	{
		JGetUserNotification()->ReportError(JGetString("SpecifyCurveLabel::global"));
		return false;
	}
	return true;
}
