/******************************************************************************
 CreatePlotDialogBase.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright @ 2023 by John Lindal.

 ******************************************************************************/

#include "CreatePlotDialogBase.h"
#include "DataDocument.h"
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CreatePlotDialogBase::CreatePlotDialogBase()
	:
	JXModalDialogDirector(),
	itsPlotIndex(1)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CreatePlotDialogBase::~CreatePlotDialogBase()
{
}

/******************************************************************************
 GetLabel

 ******************************************************************************/

const JString&
CreatePlotDialogBase::GetLabel()
{
	return itsLabelInput->GetText()->GetText();
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
CreatePlotDialogBase::SetObjects
	(
	DataDocument*	tableDir,
	JXInputField*	labelInput,
	JXTextMenu*		plotMenu
	)
{
	itsLabelInput = labelInput;
	itsLabelInput->GetText()->SetText(JGetString("DefaultLabel::global"));

	itsPlotMenu = plotMenu;
	itsPlotMenu->AppendItem(JGetString("NewPlotItemLabel::global"));
	itsPlotMenu->ShowSeparatorAfter(1, true);

	JPtrArray<JString> names(JPtrArrayT::kDeleteAll);
	tableDir->GetPlotNames(names);
	for (auto* name : names)
	{
		itsPlotMenu->AppendItem(*name);
	}

	itsPlotMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPlotMenu->SetToPopupChoice(true, itsPlotIndex);
	ListenTo(itsPlotMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsPlotIndex = msg.GetIndex();
	}));
}

/******************************************************************************
 OKToDeactivate

 ******************************************************************************/

bool
CreatePlotDialogBase::OKToDeactivate()
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
