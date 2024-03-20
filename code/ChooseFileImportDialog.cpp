/******************************************************************************
 ChooseFileImportDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "ChooseFileImportDialog.h"
#include "PlotApp.h"
#include "DataDocument.h"
#include "PrefsMgr.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXColorManager.h>

#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize  kFileByteCount				= 1000;
const JIndex kFileModulePrefsVersionID	= 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

ChooseFileImportDialog::ChooseFileImportDialog
	(
	DataDocument* supervisor,
	const JString& filename
	)
	:
	JXModalDialogDirector(true),
	JPrefObject(GetPrefsMgr(), kFileModulePrefsID),
	itsDir(supervisor)
{
	BuildWindow(filename);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ChooseFileImportDialog::~ChooseFileImportDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
ChooseFileImportDialog::BuildWindow
	(
	const JString& filename
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 330,230, JGetString("WindowTitle::ChooseFileImportDialog::JXLayout"));

	auto* errorMessage =
		jnew JXStaticText(JGetString("errorMessage::ChooseFileImportDialog::JXLayout"), true, false, false, nullptr, window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 310,40);
	errorMessage->SetBorderWidth(0);

	auto* textScrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,50, 310,100);
	assert( textScrollbarSet != nullptr );

	itsFilterMenu =
		jnew JXTextMenu(JGetString("itsFilterMenu::ChooseFileImportDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,160, 70,30);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::ChooseFileImportDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,200, 70,20);
	assert( cancelButton != nullptr );

	itsReloadButton =
		jnew JXTextButton(JGetString("itsReloadButton::ChooseFileImportDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,200, 70,20);

	okButton =
		jnew JXTextButton(JGetString("okButton::ChooseFileImportDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 230,200, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::ChooseFileImportDialog::JXLayout"));

// end JXLayout

	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);

	const JSize dirModCount = itsDir->GetInternalModuleCount();
	for (JIndex i=1; i<=dirModCount; i++)
	{
		itsFilterMenu->AppendItem(itsDir->GetInternalModuleName(i));
	}

	for (const auto* name : *GetApplication()->GetImportModules())
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

	ListenTo(itsReloadButton, std::function([this](const JXButton::Pushed&)
	{
		GetApplication()->ReloadImportModules();
		itsFilterMenu->RemoveAllItems();

		const JSize dirModCount = itsDir->GetInternalModuleCount();
		for (JSize i = 1; i <= dirModCount; i++)
		{
			itsFilterMenu->AppendItem(itsDir->GetInternalModuleName(i));
		}

		JPtrArray<JString>* names = GetApplication()->GetImportModules();
		const JSize strCount = names->GetItemCount();
		for (JSize i = 1; i <= strCount; i++)
		{
			itsFilterMenu->AppendItem(*(names->GetItem(i)));
		}
		itsFilterIndex = 1;
		itsFilterMenu->SetToPopupChoice(true, itsFilterIndex);
	}));

	std::ifstream is(filename.GetBytes());
	JString text;
	text.Read(is, kFileByteCount);

	itsFileText =
		jnew JXStaticText(text, false, true, false,
			textScrollbarSet, textScrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic, 10,60, 310,90);
	assert(itsFileText != nullptr);
	itsFileText->FitToEnclosure();
	itsFileText->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsFileText->SetCaretLocation(1);
}

/******************************************************************************
 GetFilterIndex

 ******************************************************************************/

JIndex
ChooseFileImportDialog::GetFilterIndex()
{
	return itsFilterIndex;
}

/******************************************************************************
 GetFileText (public)

 ******************************************************************************/

const JString&
ChooseFileImportDialog::GetFileText()
{
	return itsFileText->GetText()->GetText();
}

/******************************************************************************
 ReadPrefs (protected)

 ******************************************************************************/

void
ChooseFileImportDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JIndex id;
	input >> id;
	if (id <= kFileModulePrefsVersionID)
	{
		input >> id;
		if (id <= itsFilterMenu->GetItemCount())
		{
			itsFilterIndex = id;
			itsFilterMenu->SetToPopupChoice(true, itsFilterIndex);
		}
		JPoint loc;
		input >> loc;
		JCoordinate w;
		JCoordinate h;
		input >> w;
		input >> h;
		GetWindow()->SetSize(w, h);
		GetWindow()->Place(loc.x, loc.y);
	}
}

/******************************************************************************
 WritePrefs (protected)

 ******************************************************************************/

void
ChooseFileImportDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kFileModulePrefsVersionID << ' ';
	output << itsFilterIndex << ' ';
	output << GetWindow()->GetDesktopLocation() << ' ';
	output << GetWindow()->GetFrameWidth() << ' ';
	output << GetWindow()->GetFrameHeight() << ' ';
}
