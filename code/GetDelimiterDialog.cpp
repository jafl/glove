/******************************************************************************
 GetDelimiterDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GetDelimiterDialog.h"
#include "PlotApp.h"
#include "PrefsMgr.h"
#include "globals.h"

#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXInputField.h>

#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

const JIndex kDelimiterPrefsVersionID = 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

GetDelimiterDialog::GetDelimiterDialog
	(
	JXWindowDirector*	supervisor,
	const JString&		text
	)
	:
	JXDialogDirector(supervisor, true),
	JPrefObject(GetPrefsMgr(), kDelimiterPrefsID)
{
	BuildWindow();

	JString s(text);
	JStringIterator iter(&s);

	while (iter.Next(" "))
	{
		iter.ReplaceLastMatch("\xE2\x80\xA2");
	}

	while (iter.Prev("\t"))
	{
		iter.ReplaceLastMatch("\xE2\x86\x92");
	}

	itsFileText->GetText()->SetText(s);

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GetDelimiterDialog::~GetDelimiterDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GetDelimiterDialog::BuildWindow()
{
	JXTextRadioButton* rb[4];

// begin JXLayout

	auto* window = jnew JXWindow(this, 330,360, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,220, 310,100);
	assert( scrollbarSet != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 210,330, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::GetDelimiterDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,330, 70,20);
	assert( cancelButton != nullptr );

	itsRG =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 215,110);
	assert( itsRG != nullptr );

	rb[0] =
		jnew JXTextRadioButton(1, JGetString("rb[0]::GetDelimiterDialog::JXLayout"), itsRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,10, 120,20);
	assert( rb[0] != nullptr );

	rb[1] =
		jnew JXTextRadioButton(2, JGetString("rb[1]::GetDelimiterDialog::JXLayout"), itsRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,30, 120,20);
	assert( rb[1] != nullptr );

	rb[2] =
		jnew JXTextRadioButton(3, JGetString("rb[2]::GetDelimiterDialog::JXLayout"), itsRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,50, 120,20);
	assert( rb[2] != nullptr );

	rb[3] =
		jnew JXTextRadioButton(4, JGetString("rb[3]::GetDelimiterDialog::JXLayout"), itsRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,70, 100,20);
	assert( rb[3] != nullptr );

	itsCharInput =
		jnew JXInputField(itsRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 125,70, 40,20);
	assert( itsCharInput != nullptr );

	auto* helpLabel =
		jnew JXStaticText(JGetString("helpLabel::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,200, 310,20);
	assert( helpLabel != nullptr );
	helpLabel->SetToLabel();

	itsSkipCB =
		jnew JXTextCheckbox(JGetString("itsSkipCB::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,140, 80,20);
	assert( itsSkipCB != nullptr );

	itsCommentCB =
		jnew JXTextCheckbox(JGetString("itsCommentCB::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,160, 170,20);
	assert( itsCommentCB != nullptr );

	itsSkipCountInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,140, 40,20);
	assert( itsSkipCountInput != nullptr );

	auto* lineLabel =
		jnew JXStaticText(JGetString("lineLabel::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 135,140, 60,20);
	assert( lineLabel != nullptr );
	lineLabel->SetToLabel();

	itsCommentInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 180,160, 50,20);
	assert( itsCommentInput != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GetDelimiterDialog"));
	SetButtons(okButton, cancelButton);

	itsFileText =
		jnew JXStaticText(JString::empty, false, false, false,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic, 10,60, 310,90);
	assert(itsFileText != nullptr);
	itsFileText->FitToEnclosure();

	itsCharInput->Deactivate();
	itsCharInput->SetMaxLength(1);

	itsSkipCountInput->Deactivate();
	itsCommentInput->Deactivate();

	helpLabel->GetText()->SetText(JGetString("Help::GetDelimiterDialog"));

	ListenTo(itsRG);
	ListenTo(itsSkipCB);
	ListenTo(itsCommentCB);

	itsCommentInput->GetText()->SetText(JString("#", JString::kNoCopy));
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GetDelimiterDialog::Receive
	(
	JBroadcaster* sender,
	const Message& message
	)
{
	if (sender == itsRG && message.Is(JXRadioGroup::kSelectionChanged))
	{
		JIndex showIndex = itsRG->GetSelectedItem();
		if (showIndex == kChar)
		{
			itsCharInput->Activate();
		}
		else
		{
			itsCharInput->Deactivate();
		}
	}
	else if (sender == itsSkipCB && message.Is(JXCheckbox::kPushed))
	{
		if (itsSkipCB->IsChecked())
		{
			itsSkipCountInput->Activate();
		}
		else
		{
			itsSkipCountInput->Deactivate();
		}
	}
	else if (sender == itsCommentCB && message.Is(JXCheckbox::kPushed))
	{
		if (itsCommentCB->IsChecked())
		{
			itsCommentInput->Activate();
		}
		else
		{
			itsCommentInput->Deactivate();
		}
	}
	else
	{
		JXDialogDirector::Receive(sender, message);
	}
}

/******************************************************************************
 GetFilterIndex

 ******************************************************************************/

GetDelimiterDialog::DelimiterType
GetDelimiterDialog::GetDelimiterType()
{
	return (GetDelimiterDialog::DelimiterType) itsRG->GetSelectedItem();
}

/******************************************************************************
 GetFileText

 ******************************************************************************/

JUtf8Byte
GetDelimiterDialog::GetCharacter()
{
	assert(itsRG->GetSelectedItem() == kChar);
	assert(!itsCharInput->GetText()->IsEmpty());
	const JString& temp = itsCharInput->GetText()->GetText();
	return temp.GetRawBytes()[0];
}

/******************************************************************************
 IsSkippingLines

 ******************************************************************************/

bool
GetDelimiterDialog::IsSkippingLines()
{
	return itsSkipCB->IsChecked();
}

/******************************************************************************
 GetSkipLineCount

 ******************************************************************************/

JSize
GetDelimiterDialog::GetSkipLineCount()
{
	JInteger value;
	if (itsSkipCountInput->GetValue(&value))
	{
		return value;
	}
	return 0;
}

/******************************************************************************
 HasComments

 ******************************************************************************/

bool
GetDelimiterDialog::HasComments()
{
	return itsCommentCB->IsChecked();
}

/******************************************************************************
 GetCommentString

 ******************************************************************************/

const JString&
GetDelimiterDialog::GetCommentString()
{
	return itsCommentInput->GetText()->GetText();
}

/******************************************************************************
 ReadPrefs (protected)

 ******************************************************************************/

void
GetDelimiterDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JIndex id;
	input >> id;
	if (id <= kDelimiterPrefsVersionID)
	{
		input >> id;
		itsRG->SelectItem(id);
		JString str;
		input >> str;
		itsCharInput->GetText()->SetText(str);
		bool checked;
		input >> JBoolFromString(checked);
		itsSkipCB->SetState(checked);
		bool ok;
		input >> JBoolFromString(ok);
		if (ok)
		{
			JInteger value;
			input >> value;
			itsSkipCountInput->SetValue(value);
		}
		input >> JBoolFromString(checked);
		itsCommentCB->SetState(checked);
		input >> str;
		itsCommentInput->GetText()->SetText(str);
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
GetDelimiterDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kDelimiterPrefsVersionID << ' ';
	output << itsRG->GetSelectedItem() << ' ';
	output << itsCharInput->GetText() << ' ';
	output << JBoolToString(itsSkipCB->IsChecked()) << ' ';
	JInteger value;
	bool ok = itsSkipCountInput->GetValue(&value);
	output << JBoolToString(ok) << ' ';
	if (ok)
	{
		output << value << ' ';
	}
	output << JBoolToString(itsCommentCB->IsChecked()) << ' ';
	output << itsCommentInput->GetText() << ' ';
	output << GetWindow()->GetDesktopLocation() << ' ';
	output << GetWindow()->GetFrameWidth() << ' ';
	output << GetWindow()->GetFrameHeight() << ' ';
}
