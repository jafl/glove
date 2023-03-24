/******************************************************************************
 FitParmsDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "FitParmsDir.h"
#include "FitParmsTable.h"
#include "FitBase.h"
#include "PlotDir.h"
#include "HistoryDir.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FitParmsDir::FitParmsDir
	(
	PlotDir* supervisor,
	JPtrArray<FitBase>* fits
	)
	:
	JXWindowDirector(supervisor),
	itsPlotDir(supervisor),
	itsFits(fits)
{
	ListenTo(itsFits);

	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FitParmsDir::~FitParmsDir()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
FitParmsDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 260,240, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 260,190);
	assert( scrollbarSet != nullptr );

	itsFitMenu =
		jnew JXTextMenu(JGetString("itsFitMenu::FitParmsDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 10,210, 60,20);
	assert( itsFitMenu != nullptr );

	itsCloseButton =
		jnew JXTextButton(JGetString("itsCloseButton::FitParmsDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 90,210, 70,20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts(JGetString("itsCloseButton::FitParmsDir::shortcuts::JXLayout"));

	itsSessionButton =
		jnew JXTextButton(JGetString("itsSessionButton::FitParmsDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 180,210, 70,20);
	assert( itsSessionButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::FitParmsDir"));
	window->PlaceAsDialogWindow();
	window->LockCurrentSize();

	itsTable =
		jnew FitParmsTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,100,100);
	assert (itsTable != nullptr);
	itsTable->FitToEnclosure();

	itsFitMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFitMenu);
	UpdateFitMenu();

	ListenTo(itsSessionButton);
	ListenTo(itsCloseButton);
}

/******************************************************************************
 ShowFit

 ******************************************************************************/

void
FitParmsDir::ShowFit
	(
	const JIndex index
	)
{
	HandleFitMenu(index);
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
FitParmsDir::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsFits &&
		(	message.Is(JListT::kElementsInserted) ||
			message.Is(JListT::kElementsRemoved)))
	{
		UpdateFitMenu();
	}

	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
	{
		Deactivate();
	}

	else if (sender == itsSessionButton && message.Is(JXButton::kPushed))
	{
		SendToSession(itsCurrentIndex);
	}

	else if (sender == itsFitMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFitMenu(selection->GetIndex());
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateFitMenu

 ******************************************************************************/

void
FitParmsDir::UpdateFitMenu()
{
	itsFitMenu->RemoveAllItems();

	const JSize count = itsFits->GetElementCount();
	if (count == 0)
	{
		itsFitMenu->Deactivate();
	}
	else
	{
		itsFitMenu->Activate();
	}

	BuildColumnMenus("FitMenuItem::global", count, itsFitMenu, nullptr);
}

/******************************************************************************
 HandleFitMenu

 ******************************************************************************/

void
FitParmsDir::HandleFitMenu
	(
	const JIndex index
	)
{
	assert ( index <= itsFits->GetElementCount());
	itsCurrentIndex = index;
	itsTable->Clear();
	FitBase* fit = itsFits->GetElement(index);
	const JSize count = fit->GetParameterCount();
	itsTable->Append(JGetString("FunctionLabel::FitParmsDir"), fit->GetFitFunctionString());
	JSize i;
	for (i = 1; i <= count; i++)
	{
		JString str;
		fit->GetParameterName(i, &str);
		JFloat value;
		fit->GetParameter(i, &value);
		JString pstr = str + ":";
		if (value < 0.001 || value > 100000)
		{
			itsTable->Append(pstr, JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5));
		}
		else
		{
			itsTable->Append(pstr, JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5));
		}
		if (fit->GetParameterError(i, &value))
		{
			str += " error:";
			if (value < 0.001 || value > 100000)
			{
				itsTable->Append(str, JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5));
			}
			else
			{
				itsTable->Append(str, JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5));
			}
		}
	}
	if (fit->HasGoodnessOfFit())
	{
		JString str;
		fit->GetGoodnessOfFitName(&str);
		JFloat value;
		if (fit->GetGoodnessOfFit(&value))
		{
			str += ":";
			if (value < 0.001 || value > 100000)
			{
				itsTable->Append(str, JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5));
			}
			else
			{
				itsTable->Append(str, JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5));
			}
		}
	}
}

/******************************************************************************
 SendAllToSession

 ******************************************************************************/

void
FitParmsDir::SendAllToSession()
{
	JSize count = itsFits->GetElementCount();
	for (JSize i = 1; i <= count; i++)
	{
		SendToSession(i);
	}
}

/******************************************************************************
 SendToSession

 ******************************************************************************/

void
FitParmsDir::SendToSession
	(
	const JIndex index
	)
{
	assert ( index <= itsFits->GetElementCount());
	HistoryDir* dir = itsPlotDir->GetSessionDir();
	FitBase* fit = itsFits->GetElement(index);
	const JSize count = fit->GetParameterCount();

	JString str((JUInt64) index);
	const JUtf8Byte* map[] =
	{
		"i", str.GetBytes()
	};
	str = JGetString("FitMenuItem::global", map, sizeof(map));
	dir->AppendText(str);

	str = "Function: " + fit->GetFitFunctionString();
	dir->AppendText(str);

	for (JIndex i = 1; i <= count; i++)
	{
		fit->GetParameterName(i, &str);
		JFloat value;
		fit->GetParameter(i, &value);
		JString pstr = str + ": ";
		if (value < 0.001 || value > 100000)
		{
			pstr = pstr + JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5);
		}
		else
		{
			pstr = pstr + JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5);
		}
		dir->AppendText(pstr);
		if (fit->GetParameterError(i, &value))
		{
			str += " error: ";
			if (value < 0.001 || value > 100000)
			{
				str = str + JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5);
			}
			else
			{
				str = str + JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5);
			}
			dir->AppendText(str);
		}
	}
	if (fit->HasGoodnessOfFit())
	{
		fit->GetGoodnessOfFitName(&str);
		JFloat value;
		if (fit->GetGoodnessOfFit(&value))
		{
			str += ": ";
			if (value < 0.001 || value > 100000)
			{
				str = str + JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5);
			}
			else
			{
				str = str + JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5);
			}
			dir->AppendText(str);
		}
	}
	dir->AppendText(JString::newline);
}
