/******************************************************************************
 FitDirector.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include <FitDirector.h>
#include "CurveNameList.h"
#include "FitDescriptionList.h"
#include "FitManager.h"
#include "FitParameterTable.h"
#include "ParmColHeaderWidget.h"
#include "ChiSqLabel.h"
#include "PlotFitFunction.h"
#include "PlotFitLinearEq.h"
#include "PlotFitExp.h"
#include "PlotFitPowerLaw.h"
#include "PlotFitProxy.h"
#include "PolyFitDescription.h"
#include "BuiltinFitDescription.h"
#include "NonLinearFitDescription.h"
#include "ModuleFitDescription.h"
#include "NonLinearFitDialog.h"
#include "PolyFitDialog.h"
#include "VarList.h"
#include "HistoryDir.h"
#include "PlotDirector.h"

#include "PlotFitLinear.h"
#include "PlotFitNonLinear.h"
#include "PlotFitModule.h"

#include "PrefsMgr.h"
#include "globals.h"
#include "PlotApp.h"

#include <jx-af/j2dplot/J2DPlotData.h>
#include <jx-af/j2dplot/J2DPlotFunction.h>
#include <jx-af/j2dplot/JX2DPlotWidget.h>

#include <jx-af/jx/JXToolBar.h>

#include <jx-af/jx/JXApplication.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXDownRect.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXHorizPartition.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXUpRect.h>
#include <jx-af/jx/JXVertPartition.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>

#include <jx-af/jexpr/JXExprEditor.h>
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jcore/JVector.h>
#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kCurrentPrefsVersion	= 1;

/******************************************************************************
 Constructor

 *****************************************************************************/

FitDirector::FitDirector
	(
	PlotDirector*	supervisor,
	J2DPlotWidget*	plot,
	const JString&	file
	)
	:
	JXWindowDirector(supervisor),
	itsPlot(plot),
	itsCurrentFit(nullptr),
	itsTestFunction(nullptr),
	itsDir(supervisor),
	itsPrinter(nullptr)
{
	BuildWindow();

	itsHistory	= jnew HistoryDir(this);

	JString name;
	JString path;
	JSplitPathAndName(file, &path, &name);
	const JString title = JGetString("FitTitle::FitDirector") + name;
	GetWindow()->SetTitle(title);

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != nullptr );
	itsPrinter->SetOrientation(JPSPrinter::kPortrait);

	GetPrefsMgr()->ReadFitDirectorSetup(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

FitDirector::~FitDirector()
{
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

#include "FitDirector-Fit.h"
#include "Generic-Preferences.h"

void
FitDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 600,500, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 600,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsMgr(), kFitToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 600,470);
	assert( itsToolBar != nullptr );

// end JXLayout

	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->LockCurrentMinSize();

	itsFitMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::FitDirector_Fit"));
	itsFitMenu->SetMenuItems(kFitMenuStr);
	itsFitMenu->SetUpdateAction(JXMenu::kDisableAll);
	itsFitMenu->AttachHandlers(this,
		&FitDirector::UpdateFitMenu,
		&FitDirector::HandleFitMenu);
	ConfigureFitMenu(itsFitMenu);

	JArray<JCoordinate> widths(2);
	widths.AppendItem(155);
	widths.AppendItem(300);

	const JIndex elasticIndex = 2;

	JArray<JCoordinate> minWidths(2);
	minWidths.AppendItem(100);
	minWidths.AppendItem(300);

	itsMainPartition =
		jnew JXHorizPartition(widths, elasticIndex, minWidths,
							  itsToolBar->GetWidgetEnclosure(),
							  JXWidget::kHElastic,JXWidget::kVElastic,
							  0,0, 500,100);
	assert( itsMainPartition != nullptr );
	itsMainPartition->FitToEnclosure();

	const JCoordinate kColHeaderHeight = 20;

	// This is the first column that contains the curve and fit lists.

	JXContainer* container = itsMainPartition->GetCompartment(1);

	JArray<JCoordinate> heights(3);
	heights.AppendItem(100);
	heights.AppendItem(100);
	heights.AppendItem(50);

	JArray<JCoordinate> minHeights(3);
	minHeights.AppendItem(50);
	minHeights.AppendItem(100);
	minHeights.AppendItem(40);

	itsListPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, container,
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, 100,300);
	itsListPartition->FitToEnclosure();

	// This will be the curve list

	container = itsListPartition->GetCompartment(1);

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(container,
							JXWidget::kHElastic,JXWidget::kVElastic,
							0,0, 100,100);
	scrollbarSet->FitToEnclosure();
	scrollbarSet->Move(0, kColHeaderHeight);
	scrollbarSet->AdjustSize(0, -kColHeaderHeight);

	itsCurveList =
		jnew CurveNameList(itsDir, itsPlot, scrollbarSet,
							scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0,  100,100);
	assert(itsCurveList != nullptr);
	itsCurveList->FitToEnclosure();
	ListenTo(itsCurveList);

	JXColHeaderWidget* header =
		jnew JXColHeaderWidget(itsCurveList, scrollbarSet, container,
								JXWidget::kHElastic, JXWidget::kFixedTop,
								0,0, 100,kColHeaderHeight);
	header->FitToEnclosure(true, false);
	header->SetColTitle(1, JGetString("CurvesColTitle::FitDirector"));

	// This will be the fit list

	container = itsListPartition->GetCompartment(2);

	scrollbarSet =
		jnew JXScrollbarSet(container,
							JXWidget::kHElastic,JXWidget::kVElastic,
							0,0, 100,100);
	scrollbarSet->FitToEnclosure();
	scrollbarSet->Move(0, kColHeaderHeight);
	scrollbarSet->AdjustSize(0, -kColHeaderHeight);

	itsFitList	=
		jnew FitDescriptionList(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
								JXWidget::kHElastic, JXWidget::kVElastic,
			0,0, 100,100);
	assert(itsFitList != nullptr);
	itsFitList->FitToEnclosure();
	ListenTo(itsFitList);

	header =
		jnew JXColHeaderWidget(itsFitList, scrollbarSet, container,
								JXWidget::kHElastic, JXWidget::kFixedTop,
								0,0, 100,kColHeaderHeight);
	header->FitToEnclosure(true, false);
	header->SetColTitle(1, JGetString("FitsColTitle::FitDirector"));

	// this is the expression widget that displays the current JFunction

	container = itsListPartition->GetCompartment(3);

	scrollbarSet =
		jnew JXScrollbarSet(container,
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0,0, 100,100);
	scrollbarSet->FitToEnclosure();
	assert( scrollbarSet != nullptr );

	itsExprVarList	= jnew VarList();

	itsExprWidget	=
		jnew JXExprEditor(itsExprVarList, menuBar, scrollbarSet,
						  scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic,
						  0,0, 100,100);
	assert(itsExprWidget != nullptr);
	itsExprWidget->FitToEnclosure();
	itsExprWidget->Hide();

	// This is the second column that will contain the parameter table
	// and the plots

	container = itsMainPartition->GetCompartment(2);

	heights.RemoveAll();
	heights.AppendItem(80);
	heights.AppendItem(150);
	heights.AppendItem(150);

	minHeights.RemoveAll();
	minHeights.AppendItem(70);
	minHeights.AppendItem(100);
	minHeights.AppendItem(100);

	itsPlotPartition =
		jnew JXVertPartition(heights, elasticIndex, minHeights, container,
							 JXWidget::kHElastic, JXWidget::kVElastic,
							 0,0, 100,400);
	itsPlotPartition->FitToEnclosure();

	container = itsPlotPartition->GetCompartment(1);

// begin parameterLayout

	const JRect parameterLayout_Aperture = container->GetAperture();
	container->AdjustSize(361 - parameterLayout_Aperture.width(), 241 - parameterLayout_Aperture.height());

	auto* scrollbarSet2 =
		jnew JXScrollbarSet(container,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 360,200);
	assert( scrollbarSet2 != nullptr );

	auto* label =
		jnew ChiSqLabel(container,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,220, 170,20);
	assert( label != nullptr );

	auto* downRect =
		jnew JXDownRect(container,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 170,220, 190,20);
	assert( downRect != nullptr );

	container->SetSize(parameterLayout_Aperture.width(), parameterLayout_Aperture.height());

// end parameterLayout

	itsParameterTable =
		jnew FitParameterTable(scrollbarSet2, scrollbarSet2->GetScrollEnclosure(),
								JXWidget::kHElastic, JXWidget::kVElastic,
								0,0, 100,100);
	assert(itsParameterTable != nullptr);
	itsParameterTable->FitToEnclosure();
	ListenTo(itsParameterTable);

	itsParameterColHeader =
		jnew ParmColHeaderWidget(itsParameterTable, scrollbarSet2, container,
								 JXWidget::kHElastic, JXWidget::kFixedTop,
								 0,0, 100, kColHeaderHeight);
	itsParameterColHeader->FitToEnclosure(true, false);
	assert(itsParameterColHeader != nullptr);

	itsParameterTable->SetColHeaderWidget(itsParameterColHeader);

	itsChiSq =
		jnew JXStaticText(JString::empty, downRect,
						  JXWidget::kHElastic, JXWidget::kVElastic,
						  0,0, 100,100);
	itsChiSq->FitToEnclosure();
	itsChiSq->SetBackColor(JColorManager::GetWhiteColor());

	// now add the 2 plots

	container = itsPlotPartition->GetCompartment(2);

	itsFitPlot	=
		jnew JX2DPlotWidget(menuBar, container,
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	itsFitPlot->FitToEnclosure();
	itsFitPlot->SetTitle(JGetString("FitPlotTitle::FitDirector"));
	itsFitPlot->SetXLabel(itsPlot->GetXLabel());
	itsFitPlot->SetYLabel(itsPlot->GetYLabel());

	container = itsPlotPartition->GetCompartment(3);

	itsDiffPlot	=
		jnew JX2DPlotWidget(itsFitPlot, container,
							JXWidget::kHElastic, JXWidget::kVElastic,
							0,0, 100,100);
	itsDiffPlot->FitToEnclosure();
	itsDiffPlot->SetTitle(JGetString("DiffPlotTitle::FitDirector"));
	itsDiffPlot->SetXLabel(itsPlot->GetXLabel());
	itsDiffPlot->SetYLabel(itsPlot->GetYLabel());
	itsDiffPlot->ShowFrame(false);

	// menus & toolbar

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::Generic_Preferences"));
	itsPrefsMenu->SetMenuItems(kPreferencesMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &FitDirector::HandlePrefsMenu);
	ConfigurePreferencesMenu(itsPrefsMenu);

	auto* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "FitHelp");

	itsCurveList->SetCurrentCurveIndex(1);

	itsToolBar->LoadPrefs(nullptr);
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFitMenu, kFitCmd);
		itsToolBar->AppendButton(itsFitMenu, kRefitCmd);
		itsToolBar->AppendButton(itsFitMenu, kTestFitCmd);
		itsToolBar->AppendButton(itsFitMenu, kPlotCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFitMenu, kCloseCmd);

		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
FitDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCurveList && message.Is(CurveNameList::kCurveSelected))
	{
		const CurveNameList::CurveSelected* info =
			dynamic_cast<const CurveNameList::CurveSelected*>(&message);
		assert(info != nullptr);

		RemoveFit();
		RemoveCurves();

		itsParameterTable->ClearValues();
		itsChiSq->GetText()->SetText(JString::empty);

		// add new curve.
		J2DPlotDataBase* curve = itsPlot->GetCurve(info->GetIndex());
		itsFitPlot->AddCurve(curve, false, itsPlot->GetCurveName(info->GetIndex()));
		itsFitPlot->ProtectCurve(1, true);
	}
	else if (sender == itsFitList && message.Is(FitDescriptionList::kFitSelected))
	{
		const FitDescriptionList::FitSelected* info =
			dynamic_cast<const FitDescriptionList::FitSelected*>(&message);
		assert(info != nullptr);
		const FitDescription& fd	= GetFitManager()->GetFitDescription(info->GetIndex());
		itsParameterTable->SetFitDescription(fd);
		RemoveFit();
		itsChiSq->GetText()->SetText(JString::empty);

		JExprParser p(const_cast<FitDescription&>(fd).GetVarList());

		JFunction* f;
		if (p.Parse(fd.GetFitFunctionString(), &f))
		{
			itsExprWidget->SetFunction(const_cast<FitDescription&>(fd).GetVarList(), f);
			itsExprWidget->Show();
		}
	}
	else if (sender == itsFitList && message.Is(FitDescriptionList::kFitInitiated))
	{
		const FitDescriptionList::FitInitiated* info =
			dynamic_cast<const FitDescriptionList::FitInitiated*>(&message);
		assert(info != nullptr);
		if (!itsParameterTable->BeginEditingStartValues())
		{
			Fit();
			AddHistoryText();
		}
	}
	else if (sender == itsParameterTable && message.Is(FitParameterTable::kValueChanged))
	{
		const FitParameterTable::ValueChanged* info =
			dynamic_cast<const FitParameterTable::ValueChanged*>(&message);
		assert(info != nullptr);
		JIndex index;
		bool ok	= itsFitList->GetCurrentFitIndex(&index);
		assert(ok);
		FitDescription& fd	= GetFitManager()->GetFitDescription(index);
		const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
		const JSize count	= parms.GetItemCount();
		for (JIndex i = 1; i <= count; i++)
		{
			fd.GetVarList()->SetValue(i + 1, parms.GetItem(i));
		}
		GetWindow()->Refresh();
	}
	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 HandleFitMenu

 ******************************************************************************/

void
FitDirector::HandleFitMenu
	(
	const JIndex index
	)
{
	if (index == kNonLinearCmd)
	{
		auto* dlog = jnew NonLinearFitDialog();
		if (dlog->DoDialog())
		{
			NonLinearFitDescription* fit =
				jnew NonLinearFitDescription(dlog->GetFitName(),
											 dlog->GetFunctionString(),
											 dlog->GetDerivativeString(),
											 dlog->GetVarList().GetVariables());
			assert(fit != nullptr);
			GetFitManager()->AddFitDescription(fit);
		}
	}
	else if (index == kPolyCmd)
	{
		auto* dlog = jnew PolyFitDialog();
		if (dlog->DoDialog())
		{
			JArray<JIndex> powers;
			dlog->GetPowers(&powers);
			PolyFitDescription* fit	= jnew PolyFitDescription(dlog->GetFitName(), powers);
			assert(fit != nullptr);
			GetFitManager()->AddFitDescription(fit);
		}
	}
	else if (index == kRemoveFitCmd)
	{
		JIndex index1;
		if (itsFitList->GetCurrentFitIndex(&index1))
		{
			itsExprWidget->ClearFunction();
			itsExprWidget->Hide();
			GetFitManager()->RemoveFitDescription(index1);
		}
	}
	else if (index == kFitCmd)
	{
		Fit();
		AddHistoryText();
	}
	else if (index == kTestFitCmd)
	{
		TestFit();
	}
	else if (index == kShowStartCmd)
	{
		itsParameterTable->ShowStartCol(!itsParameterTable->IsShowingStartCol());
	}
	else if (index == kRefitCmd)
	{
		Refit();
		AddHistoryText(true);
	}
	else if (index == kPlotCmd)
	{
//		JString fn	= itsCurrentFit->GetFitFunctionString();
//		need to check - via
//		JFunction* f;
//		if (JParseFunction(itsFunctionString->GetText(), itsList, &f))
//			{
//			return true;
//			}
//		whether or not the function is valid.

		JIndex index1;
		bool ok	= itsCurveList->GetCurrentCurveIndex(&index1);
		assert(ok);
		J2DPlotDataBase* data = itsPlot->GetCurve(index1);
		assert(itsCurrentFit != nullptr);
		PlotFitProxy* proxy	= jnew PlotFitProxy(itsCurrentFit, itsPlot, data);
		JIndex findex;
		ok = itsFitList->GetCurrentFitIndex(&findex);
		assert(ok);
		const FitDescription& fd = GetFitManager()->GetFitDescription(findex);
		itsDir->AddFitProxy(proxy, index1, fd.GetFnName());
	}
	else if (index == kShowHistoryCmd)
	{
		itsHistory->Activate();
	}
	else if (index == kPrintCmd)
	{
		if (itsPrinter->ConfirmUserPrintSetup())
		{
			Print();
		}
	}
	else if (index == kCloseCmd)
	{
		Close();
	}
}

/******************************************************************************
 UpdateFitMenu

 ******************************************************************************/

void
FitDirector::UpdateFitMenu()
{
	JIndex index;
	if (itsCurveList->GetCurrentCurveIndex(&index) &&
		itsFitList->GetCurrentFitIndex(&index))
	{
		itsFitMenu->EnableItem(kFitCmd);
		const FitDescription& fd	= GetFitManager()->GetFitDescription(index);
		if (fd.RequiresStartValues())
		{
			itsFitMenu->EnableItem(kTestFitCmd);
			itsFitMenu->CheckItem(kShowStartCmd);
			itsFitMenu->DisableItem(kShowStartCmd);
			if (itsCurrentFit != nullptr)
			{
				itsFitMenu->EnableItem(kRefitCmd);
			}
		}
		else if (fd.CanUseStartValues())
		{
			itsFitMenu->EnableItem(kShowStartCmd);
			if (itsParameterTable->IsShowingStartCol())
			{
				itsFitMenu->CheckItem(kShowStartCmd);
				itsFitMenu->EnableItem(kTestFitCmd);
			}
			if (itsCurrentFit != nullptr)
			{
				itsFitMenu->EnableItem(kRefitCmd);
			}
		}
		if (GetFitManager()->FitIsRemovable(index))
		{
			itsFitMenu->EnableItem(kRemoveFitCmd);
		}
		if (itsCurrentFit != nullptr)
		{
			itsFitMenu->EnableItem(kPlotCmd);
			itsFitMenu->EnableItem(kPrintCmd);
		}
	}
	itsFitMenu->EnableItem(kNonLinearCmd);
	itsFitMenu->EnableItem(kPolyCmd);
	itsFitMenu->EnableItem(kShowHistoryCmd);
	itsFitMenu->EnableItem(kCloseCmd);
}

/******************************************************************************
 HandlePrefsMenu

 ******************************************************************************/

void
FitDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kEditToolBarCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kEditMacWinPrefsCmd)
	{
		JXMacWinPrefsDialog::EditPrefs();
	}

	else if (index == kSaveWindowSizeCmd)
	{
		GetPrefsMgr()->WriteFitDirectorSetup(this);
	}
}

/******************************************************************************
 Prefs (protected)

 ******************************************************************************/

void
FitDirector::ReadPrefs
	(
	std::istream& input
	)
{
	int version;
	input >> version;
	if (version > kCurrentPrefsVersion)
	{
		return;
	}
	GetWindow()->ReadGeometry(input);
	itsMainPartition->ReadGeometry(input);
	itsListPartition->ReadGeometry(input);
	itsPlotPartition->ReadGeometry(input);
	if (version >= 1)
	{
		itsPrinter->ReadXPSSetup(input);
	}
}

void
FitDirector::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion << ' ' ;
	GetWindow()->WriteGeometry(output);
	itsMainPartition->WriteGeometry(output);
	itsListPartition->WriteGeometry(output);
	itsPlotPartition->WriteGeometry(output);
	itsPrinter->WriteXPSSetup(output);
}

/******************************************************************************
 Fit (private)

 ******************************************************************************/

void
FitDirector::Fit()
{
	JXGetApplication()->DisplayBusyCursor();
	RemoveFit();
	JIndex index;
	bool ok = itsCurveList->GetCurrentCurveIndex(&index);
	assert(ok);
	J2DPlotDataBase* data = itsPlot->GetCurve(index);
	ok = itsFitList->GetCurrentFitIndex(&index);
	assert(ok);
	const FitDescription& fd = GetFitManager()->GetFitDescription(index);
	if (fd.GetType() == FitDescription::kPolynomial)
	{
		JArray<JIndex> powers;
		const PolyFitDescription& pd = dynamic_cast<const PolyFitDescription&>(fd);
		pd.GetPowers(&powers);
		assert(ok);

		JFloat xmax, xmin, ymax, ymin;
		PlotFitLinearEq* fit;
		if (itsFitPlot->IsUsingRange())
		{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew PlotFitLinearEq(itsFitPlot, data, xmin, xmax, ymin, ymax);
		}
		else
		{
			data->GetXRange(&xmin, &xmax);
			fit = jnew PlotFitLinearEq(itsFitPlot, data, xmin, xmax);
		}
		fit->InitializePolynomial(powers);
		if (itsParameterTable->IsShowingStartCol())
		{
			JVector p(const_cast<FitDescription&>(fd).GetVarList()->GetVariableCount() - 1);
			const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
			const JSize count = p.GetDimensionCount();
			for (JIndex i = 1; i <= count; i++)
			{
				p.SetElement(i, parms.GetItem(i));
			}
			fit->PlotFitBase::GenerateFit(p, 0);
		}
		else
		{
			fit->GenerateFit();
		}
		itsCurrentFit = fit;
	}
	else if (fd.GetType() == FitDescription::kNonLinear)
	{
		NonLinearFitDescription& nd	=
			dynamic_cast<NonLinearFitDescription&>(const_cast<FitDescription&>(fd));

		JFloat xmax, xmin, ymax, ymin;
		PlotFitNonLinear* fit;
		if (itsFitPlot->IsUsingRange())
		{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew PlotFitNonLinear(itsFitPlot, data, xmin, xmax, ymin, ymax);
		}
		else
		{
			data->GetXRange(&xmin, &xmax);
			fit = jnew PlotFitNonLinear(itsFitPlot, data, xmin, xmax);
		}
		fit->SetVarList(nd.GetVarList());
		fit->SetFunction(nd.GetFunctionString());
		JString fp = nd.GetFunctionPrimedString();
		if (!fp.IsEmpty())
		{
			fit->SetFPrimed(fp);
		}
		JVector p(nd.GetVarList()->GetVariableCount() - 1);
		const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
		const JSize count = p.GetDimensionCount();
		for (JIndex i = 1; i <= count; i++)
		{
			p.SetElement(i, parms.GetItem(i));
		}
		fit->SetInitialParameters(p);
		itsCurrentFit = fit;
	}
	else if (fd.GetType() == FitDescription::kBLinear)
	{
		JFloat xmax, xmin, ymax, ymin;
		PlotFitLinear* fit;
		if (itsFitPlot->IsUsingRange())
		{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew PlotFitLinear(itsFitPlot, data, xmin, xmax, ymin, ymax);
		}
		else
		{
			data->GetXRange(&xmin, &xmax);
			fit = jnew PlotFitLinear(itsFitPlot, data, xmin, xmax);
		}
		if (itsParameterTable->IsShowingStartCol())
		{
			JVector p(const_cast<FitDescription&>(fd).GetVarList()->GetVariableCount() - 1);
			const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
			const JSize count = p.GetDimensionCount();
			for (JIndex i = 1; i <= count; i++)
			{
				p.SetElement(i, parms.GetItem(i));
			}
			fit->PlotFitBase::GenerateFit(p, 0);
		}
		else
		{
			fit->GenerateFit();
		}
		itsCurrentFit = fit;
	}
	else if (fd.GetType() == FitDescription::kBExp)
	{
		JFloat xmax, xmin, ymax, ymin;
		PlotFitExp* fit;
		if (itsFitPlot->IsUsingRange())
		{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew PlotFitExp(itsFitPlot, data, xmin, xmax, ymin, ymax);
		}
		else
		{
			data->GetXRange(&xmin, &xmax);
			fit = jnew PlotFitExp(itsFitPlot, data, xmin, xmax);
		}
		if (itsParameterTable->IsShowingStartCol())
		{
			JVector p(const_cast<FitDescription&>(fd).GetVarList()->GetVariableCount() - 1);
			const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
			const JSize count = p.GetDimensionCount();
			for (JIndex i = 1; i <= count; i++)
			{
				p.SetElement(i, parms.GetItem(i));
			}
			fit->PlotFitBase::GenerateFit(p, 0);
		}
		else
		{
			fit->GenerateFit();
		}
		itsCurrentFit = fit;
	}
	else if (fd.GetType() == FitDescription::kBPower)
	{
		JFloat xmax, xmin, ymax, ymin;
		PlotFitPowerLaw* fit;
		if (itsFitPlot->IsUsingRange())
		{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew PlotFitPowerLaw(itsFitPlot, data, xmin, xmax, ymin, ymax);
		}
		else
		{
			data->GetXRange(&xmin, &xmax);
			fit = jnew PlotFitPowerLaw(itsFitPlot, data, xmin, xmax);
		}
		if (itsParameterTable->IsShowingStartCol())
		{
			JVector p(const_cast<FitDescription&>(fd).GetVarList()->GetVariableCount() - 1);
			const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
			const JSize count	= p.GetDimensionCount();
			for (JIndex i = 1; i <= count; i++)
			{
				p.SetElement(i, parms.GetItem(i));
			}
			fit->PlotFitBase::GenerateFit(p, 0);
		}
		else
		{
			fit->GenerateFit();
		}
		itsCurrentFit = fit;
	}
	else if (fd.GetType() == FitDescription::kModule)
	{
		ModuleFitDescription& md	=
			dynamic_cast<ModuleFitDescription&>(const_cast<FitDescription&>(fd));
		JFloat xmax, xmin, ymax, ymin;
		PlotFitModule* fit;
		if (itsFitPlot->IsUsingRange())
		{
			itsFitPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			fit = jnew PlotFitModule(itsFitPlot, data, xmin, xmax, ymin, ymax);
		}
		else
		{
			data->GetXRange(&xmin, &xmax);
			fit = jnew PlotFitModule(itsFitPlot, data, xmin, xmax);
		}
		fit->SetFitModule(md.GetFitModule());
		JVector p(md.GetParameterCount());
		const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
		const JSize count	= p.GetDimensionCount();
		for (JIndex i = 1; i <= count; i++)
		{
			p.SetElement(i, parms.GetItem(i));
		}
		fit->SetInitialParameters(p);
		itsCurrentFit = fit;
	}
	itsFitPlot->AddCurve(itsCurrentFit, false, fd.GetFnName());
	itsDiffPlot->AddCurve(itsCurrentFit->GetDiffData(), false, fd.GetFnName());
	const JSize count = itsCurrentFit->GetParameterCount();
	for (JIndex i = 1; i <= count; i++)
	{
		JFloat value;
		bool ok	= itsCurrentFit->GetParameter(i, &value);
		assert(ok);
		JFloat error = 0;
		itsCurrentFit->GetParameterError(i, &error);
		itsParameterTable->SetValue(i, value, error);
		if (itsCurrentFit->GetGoodnessOfFit(&value))
		{
			itsChiSq->GetText()->SetText(JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5));
		}
		else
		{
			itsChiSq->GetText()->SetText(JString::empty);
		}
	}
}

/******************************************************************************
 Refit (private)

 ******************************************************************************/

void
FitDirector::Refit()
{
	itsParameterTable->CopyParmValuesToStart();
	Fit();
}

/******************************************************************************
 Plot (private)

 ******************************************************************************/

void
FitDirector::Plot()
{
}

/******************************************************************************
 RemoveCurves (private)

 ******************************************************************************/

void
FitDirector::RemoveCurves()
{
	// remove fit curves.
	const JSize count	= itsFitPlot->GetCurveCount();
	if (count >= 1)
	{
		itsFitPlot->ProtectCurve(1, false);
		itsFitPlot->RemoveCurve(1);
	}
}

/******************************************************************************
 RemoveFit (private)

 ******************************************************************************/

void
FitDirector::RemoveFit()
{
	// remove fit.
	const JSize count	= itsFitPlot->GetCurveCount();
	for (JIndex i = 2; i <= count; i++)
	{
		itsFitPlot->ProtectCurve(2, false);
		itsFitPlot->RemoveCurve(2);
	}

	// remove diff curve.
	const JSize fcount	= itsDiffPlot->GetCurveCount();
	for (JIndex i = 1; i <= fcount; i++)
	{
		itsDiffPlot->ProtectCurve(1, false);
		itsDiffPlot->RemoveCurve(1);
	}

	jdelete itsCurrentFit;
	itsCurrentFit	= nullptr;
	jdelete itsTestFunction;
	itsTestFunction	= nullptr;
	GetWindow()->Refresh();
}

/******************************************************************************
 TestFit (private)

 ******************************************************************************/

void
FitDirector::TestFit()
{
	RemoveFit();
	JIndex index;
	bool ok	= itsCurveList->GetCurrentCurveIndex(&index);
	assert(ok);
	J2DPlotDataBase* data = itsPlot->GetCurve(index);
	ok	= itsFitList->GetCurrentFitIndex(&index);
	assert(ok);
	FitDescription& fd	= GetFitManager()->GetFitDescription(index);
	const JArray<JFloat>& parms	= itsParameterTable->GetStartValues();
	const JSize count = parms.GetItemCount();
	for (JIndex i = 1; i <= count; i++)
	{
		fd.GetVarList()->SetValue(i + 1, parms.GetItem(i));
	}
	JFloat xmin, xmax;
	data->GetXRange(&xmin, &xmax);
	ok	= J2DPlotFunction::Create(&itsTestFunction, itsFitPlot, fd.GetVarList(),
								  fd.GetFitFunctionString(), 1, xmin, xmax);
	if (ok)
	{
		itsFitPlot->AddCurve(itsTestFunction, false, fd.GetFnName());
	}

}

/******************************************************************************
 AddHistoryText (private)

 ******************************************************************************/

void
FitDirector::AddHistoryText
	(
	const bool refit
	)
{
	JString str;
	if (refit)
	{
		str	= "Refit\n";
	}
	else
	{
		str	= "Fit\n";
	}
	itsHistory->AppendText(str, false);
	JIndex index;
	bool ok	= itsCurveList->GetCurrentCurveIndex(&index);
	assert(ok);
	str	= "Curve: " + itsPlot->GetCurveName(index) + "\n";
	itsHistory->AppendText(str, false);
	ok	= itsFitList->GetCurrentFitIndex(&index);
	assert(ok);
	const FitDescription& fd	= GetFitManager()->GetFitDescription(index);
	str	= "Fit type: " + fd.GetFnName() + "\n";
	itsHistory->AppendText(str, false);
	str.Clear();
	itsParameterTable->GetValueString(&str);
	itsHistory->AppendText(str, false);
	str = "Reduced Chi^2: " + itsChiSq->GetText()->GetText() + "\n";
	itsHistory->AppendText(str, false);
	itsHistory->AppendText("\n\n", false);
}

/******************************************************************************
 Print

 ******************************************************************************/

void
FitDirector::Print()
{
	if (itsPrinter->OpenDocument())
	{
		if (itsPrinter->NewPage())
		{
			JCoordinate kLeftMargin	= 20;
			JCoordinate width		= (JCoordinate)(itsPrinter->GetPageWidth() * 0.8);
			JCoordinate tableD		= 70;
			JCoordinate kPlotSep	= 20;
			JCoordinate kLMargin	= (itsPrinter->GetPageWidth() - width)/2;

			// draw header info
			JString str	= JGetString("CurveTitle::FitDirector");
			JIndex index;
			bool ok	= itsCurveList->GetCurrentCurveIndex(&index);
			assert(ok);
			str	+= itsPlot->GetCurveName(index);
			itsPrinter->String(kLeftMargin, 0, str);

			str	= JGetString("FitTitle::FitDirector");
			ok	= itsFitList->GetCurrentFitIndex(&index);
			assert(ok);
			const FitDescription& fd = GetFitManager()->GetFitDescription(index);
			str += fd.GetFnName();
			itsPrinter->String(kLeftMargin, kPlotSep, str);

			str = JString(fd.GetParameterCount());
			const JUtf8Byte* map[] =
			{
				"i", str.GetBytes()
			};
			str  = JGetString("ChiSqTitle::FitDirector", map, sizeof(map));
			str += itsChiSq->GetText()->GetText();
			itsPrinter->String(kLeftMargin, kPlotSep*2, str);

			// draw expression widget
			JRect eRect	= itsExprWidget->GetPrintBounds();
			itsExprWidget->DrawForPrint(*itsPrinter, JPoint(itsPrinter->GetPageWidth() - eRect.width(), 0));

			if (eRect.height() > tableD)
			{
				tableD	= eRect.height() + 5;
			}

			// draw table
			itsParameterTable->DrawForPrint(*itsPrinter, JPoint(0, tableD), false, true);
			JCoordinate parmHeight = itsParameterTable->GetBoundsHeight() + itsParameterColHeader->GetBoundsHeight();

			// draw plots
			JCoordinate height = (itsPrinter->GetPageHeight() - tableD - parmHeight)/2;
			JRect r;//(0, 0, height, width);
			r.left		= kLMargin;
			r.right		= r.left + width;
			r.top		= tableD + parmHeight;
			r.bottom	= r.top + height;
//			itsPrinter->ShiftOrigin(kLMargin, tableD + parmHeight);
			itsFitPlot->Print(*itsPrinter);//, r);
			r.Shift(0, height);
			itsDiffPlot->Print(*itsPrinter);//, r);
		}
		itsPrinter->CloseDocument();
	}
}
