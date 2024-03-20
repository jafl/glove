/******************************************************************************
 GetDelimiterDialog.cpp

	BASE CLASS = JXModalDialogDirector

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
#include <jx-af/jx/JXCharInput.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>

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
	const JString& text
	)
	:
	JXModalDialogDirector(),
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

	iter.Invalidate();
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

	auto* window = jnew JXWindow(this, 380,310, JGetString("WindowTitle::GetDelimiterDialog::JXLayout"));

	itsRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 150,100);

	itsSkipCB =
		jnew JXTextCheckbox(JGetString("itsSkipCB::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,10, 80,20);

	auto* lineLabel =
		jnew JXStaticText(JGetString("lineLabel::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 305,10, 40,20);
	lineLabel->SetToLabel(false);

	rb[0] =
		jnew JXTextRadioButton(kWhiteSpace, JGetString("rb[0]::GetDelimiterDialog::JXLayout"), itsRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 100,20);

	itsCommentCB =
		jnew JXTextCheckbox(JGetString("itsCommentCB::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,40, 160,20);

	rb[1] =
		jnew JXTextRadioButton(kSpace, JGetString("rb[1]::GetDelimiterDialog::JXLayout"), itsRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 100,20);

	rb[2] =
		jnew JXTextRadioButton(kTab, JGetString("rb[2]::GetDelimiterDialog::JXLayout"), itsRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 100,20);

	rb[3] =
		jnew JXTextRadioButton(kChar, JGetString("rb[3]::GetDelimiterDialog::JXLayout"), itsRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 100,20);

	auto* helpLabel =
		jnew JXStaticText(JGetString("helpLabel::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,120, 360,20);
	helpLabel->SetToLabel(false);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,140, 360,130);
	assert( scrollbarSet != nullptr );

	itsFileText =
		jnew JXStaticText(JString::empty, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 360,130);
	itsFileText->SetBorderWidth(0);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,280, 80,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::GetDelimiterDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 240,280, 80,20);
	assert( okButton != nullptr );

	itsCharInput =
		jnew JXCharInput(itsRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,70, 30,20);

	itsSkipCountInput =
		jnew JXIntegerInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 260,10, 40,20);
	itsSkipCountInput->SetLowerLimit(0);

	itsCommentInput =
		jnew JXCharInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 340,40, 30,20);

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsCharInput->Deactivate();
	itsSkipCountInput->Deactivate();
	itsCommentInput->Deactivate();
	itsCommentInput->GetText()->SetText("#");

	ListenTo(itsRG);
	ListenTo(itsSkipCB);
	ListenTo(itsCommentCB);
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
		JXModalDialogDirector::Receive(sender, message);
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
	output << itsCharInput->GetText()->GetText() << ' ';
	output << JBoolToString(itsSkipCB->IsChecked()) << ' ';
	JInteger value;
	bool ok = itsSkipCountInput->GetValue(&value);
	output << JBoolToString(ok) << ' ';
	if (ok)
	{
		output << value << ' ';
	}
	output << JBoolToString(itsCommentCB->IsChecked()) << ' ';
	output << itsCommentInput->GetText()->GetText() << ' ';
	output << GetWindow()->GetDesktopLocation() << ' ';
	output << GetWindow()->GetFrameWidth() << ' ';
	output << GetWindow()->GetFrameHeight() << ' ';
}
