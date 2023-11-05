/******************************************************************************
 FitModuleDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "FitModuleDialog.h"
#include "PlotApp.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FitModuleDialog::FitModuleDialog()
	:
	JXModalDialogDirector()
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FitModuleDialog::~FitModuleDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
FitModuleDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 270,100, JString::empty);

	auto* prompt =
		jnew JXStaticText(JGetString("prompt::FitModuleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,10, 230,20);
	assert( prompt != nullptr );
	prompt->SetToLabel();

	itsFilterMenu =
		jnew JXTextMenu(JGetString("itsFilterMenu::FitModuleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,40, 70,20);
	assert( itsFilterMenu != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::FitModuleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 70,20);
	assert( cancelButton != nullptr );

	itsOKButton =
		jnew JXTextButton(JGetString("itsOKButton::FitModuleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,70, 70,20);
	assert( itsOKButton != nullptr );
	itsOKButton->SetShortcuts(JGetString("itsOKButton::FitModuleDialog::shortcuts::JXLayout"));

	itsReloadButton =
		jnew JXTextButton(JGetString("itsReloadButton::FitModuleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,70, 70,20);
	assert( itsReloadButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::FitModuleDialog"));
	SetButtons(itsOKButton, cancelButton);

	JPtrArray<JString>* names = GetApplication()->GetFitModules();
	for (auto* name : *names)
	{
		itsFilterMenu->AppendItem(*name);
	}

	itsFilterIndex = 1;

	itsFilterMenu->SetToPopupChoice(true, itsFilterIndex);
	itsFilterMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFilterMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsFilterIndex = msg.GetIndex();
	}));

	if (names->IsEmpty())
	{
		itsFilterMenu->Deactivate();
		itsOKButton->Deactivate();
	}

	ListenTo(itsReloadButton, std::function([this](const JXButton::Pushed&)
	{
		GetApplication()->ReloadFitModules();
		itsFilterMenu->RemoveAllItems();
		JPtrArray<JString>* names = GetApplication()->GetFitModules();
		for (auto* name : *names)
		{
			itsFilterMenu->AppendItem(*name);
		}
		itsFilterIndex = 1;
		itsFilterMenu->SetToPopupChoice(true, itsFilterIndex);
		if (names->IsEmpty())
		{
			itsFilterMenu->Deactivate();
			itsOKButton->Deactivate();
		}
		else
		{
			itsFilterMenu->Activate();
			itsOKButton->Activate();
		}
	}));
}
