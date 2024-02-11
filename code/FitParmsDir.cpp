/******************************************************************************
 FitParmsDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "FitParmsDir.h"
#include "FitParmsTable.h"
#include "FitBase.h"
#include "PlotDirector.h"
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
	PlotDirector*		supervisor,
	JPtrArray<FitBase>*	fits
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
	itsFitMenu->AttachHandler(this, &FitParmsDir::HandleFitMenu);
	UpdateFitMenu();

	ListenTo(itsSessionButton, std::function([this](const JXButton::Pushed&)
	{
		SendToSession(itsCurrentIndex);
	}));

	ListenTo(itsCloseButton, std::function([this](const JXButton::Pushed&)
	{
		Deactivate();
	}));
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
		(message.Is(JListT::kItemsInserted) ||
		 message.Is(JListT::kItemsRemoved)))
	{
		UpdateFitMenu();
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

	const JSize count = itsFits->GetItemCount();
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
	assert ( index <= itsFits->GetItemCount());
	itsCurrentIndex = index;
	itsTable->Clear();
	FitBase* fit = itsFits->GetItem(index);
	const JSize count = fit->GetParameterCount();
	itsTable->Append(JGetString("FunctionLabel::FitParmsDir"), fit->GetFitFunctionString());

	for (JIndex i = 1; i <= count; i++)
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
	JSize count = itsFits->GetItemCount();
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
	assert ( index <= itsFits->GetItemCount());
	HistoryDir* dir = itsPlotDir->GetSessionDir();
	FitBase* fit = itsFits->GetItem(index);
	const JSize count = fit->GetParameterCount();

	JString str(index);
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
