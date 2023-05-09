/******************************************************************************
 PlotDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "PlotDir.h"
#include "VarList.h"
#include "PlotFunctionDialog.h"
#include "PlotLinearFit.h"
#include "PlotQuadFit.h"
#include "PlotFitFunction.h"
#include "FitBase.h"
#include "FitParmsDir.h"
#include "RaggedFloatTableData.h"
#include "HistoryDir.h"
#include "FitModule.h"
#include "FitModuleDialog.h"
#include "PlotApp.h"
#include "PlotModuleFit.h"
#include "Plotter.h"
#include "globals.h"
#include "FitDirector.h"
#include "PlotFitProxy.h"

#include <jx-af/j2dplot/J2DPlotDataBase.h>
#include <jx-af/j2dplot/J2DPlotData.h>
#include <jx-af/j2dplot/J2DVectorData.h>
#include <jx-af/j2dplot/J2DPlotFunction.h>
#include <jx-af/j2dplot/JX2DPlotEPSPrinter.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXDocumentMenu.h>
#include <jx-af/jx/JXFileDocument.h>
#include <jx-af/jx/JXApplication.h>
#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jx/JXCloseDirectorTask.h>
#include <jx-af/jx/JXHelpManager.h>

#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/JUserNotification.h>

#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 2;

// Plot menu

static const JUtf8Byte* kPlotMenuStr =
	"    Page setup..."
	"  | Print...         %k Meta-P"
	"  | Print session..."
	"%l| Print plot as EPS..."
	"  | Print marks as EPS..."
	"%l| Close            %k Meta-W";

enum
{
	kPageSetupCmd = 1,
	kPrintCmd,
	kPrintSessionCmd,
	kPrintPlotEPSCmd,
	kPrintMarksEPSCmd,
	kCloseCmd
};

// Analysis menu

static const JUtf8Byte* kAnalysisMenuStr =
	"    Plot function..."
	"%l| Open fit window %k Meta-F"
	"%l| Fit parameters"
	"  | Residual plots";

enum
{
	kPlotFunctionCmd = 1,
	kFitWindowCmd,
	kFitParmsCmd,
	kDiffPlotCmd
};

// Help menu

static const JUtf8Byte* kHelpMenuStr =
	"About %l| Table of Contents | This window "
	"%l|Changes|Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

PlotDir::PlotDir
	(
	JXDirector*       supervisor,
	JXFileDocument*   notifySupervisor,
	const JString&    filename,
	const bool    hideOnClose
	)
	:
	JXDocument(supervisor),
	itsFileName(filename),
	itsSupervisor(notifySupervisor),
	itsHideOnClose(hideOnClose),
	itsPlotIsClosing(false),
	itsPrinter(nullptr)
{
	itsSessionDir = jnew HistoryDir(JXGetApplication());
	assert(itsSessionDir != nullptr);
	JXGetDocumentManager()->DocumentMustStayOpen(itsSessionDir, true);
	ListenTo(itsSessionDir);

	itsVarList = jnew VarList();
	assert( itsVarList != nullptr );
	itsVarList->AddVariable(JGetString("DefaultVarName::global"), 0);

	itsXVarIndex = 1;

	itsFits = jnew JPtrArray<FitBase>(JPtrArrayT::kForgetAll);
	assert( itsFits != nullptr );
	ListenTo(itsFits);

	itsFitParmsDir = jnew FitParmsDir(this, itsFits);
	assert( itsFitParmsDir != nullptr );

	itsCurveStats = jnew JArray<GloveCurveStats>;
	assert( itsCurveStats != nullptr );

	itsCurrentCurveType = kGDataCurve;

	itsDiffDirs = jnew JPtrArray<PlotDir>(JPtrArrayT::kForgetAll);
	assert( itsDiffDirs != nullptr );
	ListenTo(itsDiffDirs);

	BuildWindow();

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != nullptr );
	itsPrinter->SetOrientation(JPSPrinter::kLandscape);

	itsEPSPrinter = jnew JX2DPlotEPSPrinter(GetDisplay());
	assert(itsEPSPrinter != nullptr);
	itsPlot->SetEPSPrinter(itsEPSPrinter);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PlotDir::~PlotDir()
{
	jdelete itsFits;
	jdelete itsCurveStats;
	jdelete itsDiffDirs;

	itsSessionDir->Deactivate();
	JXGetDocumentManager()->DocumentMustStayOpen(itsSessionDir, false);

	jdelete itsVarList;
	jdelete itsPrinter;
	jdelete itsEPSPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
PlotDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 480,370, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 480,30);
	assert( menuBar != nullptr );

	itsPlot =
		jnew Plotter(itsSessionDir, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 480,340);
	assert( itsPlot != nullptr );

// end JXLayout

	window->SetMinSize(400, 300);
	if (itsHideOnClose)
	{
		window->SetCloseAction(JXWindow::kDeactivateDirector);
	}

	ListenTo(itsPlot);

	itsPlotMenu = menuBar->PrependTextMenu(JGetString("PlotMenuTitle::PlotDir"));
	itsPlotMenu->SetMenuItems(kPlotMenuStr);
	itsPlotMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPlotMenu->AttachHandler(this, &PlotDir::HandlePlotMenu);

	itsAnalysisMenu = menuBar->AppendTextMenu(JGetString("AnalysisMenuTitle::PlotDir"));
	itsAnalysisMenu->SetMenuItems(kAnalysisMenuStr);
	itsAnalysisMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsAnalysisMenu->AttachHandler(this, &PlotDir::HandleAnalysisMenu);

	itsFitParmsMenu = jnew JXTextMenu(itsAnalysisMenu, kFitParmsCmd, menuBar);
	assert( itsFitParmsMenu != nullptr );
	itsFitParmsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFitParmsMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsFitParmsDir->Activate();
		itsFitParmsDir->ShowFit(msg.GetIndex());
	}));

	itsAnalysisMenu->DisableItem(kFitParmsCmd);

	itsDiffMenu = jnew JXTextMenu(itsAnalysisMenu, kDiffPlotCmd, menuBar);
	assert( itsDiffMenu != nullptr );
	itsDiffMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsDiffMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsDiffDirs->GetElement(msg.GetIndex())->Activate();
	}));

	itsAnalysisMenu->DisableItem(kDiffPlotCmd);

	JXDocumentMenu* windowListMenu =
		jnew JXDocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
			JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( windowListMenu != nullptr );
	menuBar->AppendMenu(windowListMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr);
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsHelpMenu->AttachHandler(this, &PlotDir::HandleHelpMenu);
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
PlotDir::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsPlot && message.Is(J2DPlotWidget::kTitleChanged))
	{
		JString title = itsFileName + ":  " + itsPlot->GetTitle();
		GetWindow()->SetTitle(title);
		JString sessiontitle = JGetString("SessionWindowPrefix::PlotDir") + title;
		itsSessionDir->GetWindow()->SetTitle(sessiontitle);
	}

	else if (sender == itsPlot && message.Is(J2DPlotWidget::kPlotChanged))
	{
		itsSupervisor->DataModified();
	}

	else if (sender == itsSessionDir && message.Is(HistoryDir::kSessionChanged))
	{
		itsSupervisor->DataModified();
	}

	else if (sender == itsPlot && message.Is(J2DPlotWidget::kIsEmpty))
	{
		if (!itsPlotIsClosing)
		{
			JXCloseDirectorTask::Close(this);
		}
	}

	else if (sender == itsPlot && message.Is(J2DPlotWidget::kCurveAdded))
	{
		HandleCurveAdded();
	}

	else if (sender == itsPlot && message.Is(J2DPlotWidget::kCurveRemoved))
	{
		const J2DPlotWidget::CurveRemoved* info =
			dynamic_cast<const J2DPlotWidget::CurveRemoved*>(&message);
		assert( info != nullptr );
		HandleCurveRemoved(info->GetIndex());
	}

	else if (sender == itsFits &&
		(	message.Is(JListT::kElementsInserted) ||
			message.Is(JListT::kElementsRemoved)))
	{
		UpdateFitParmsMenu();
	}

	else if (sender == itsDiffDirs &&
		(	message.Is(JListT::kElementsInserted) ||
			message.Is(JListT::kElementsRemoved)))
	{
		UpdateDiffMenu();
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 NewFileName

 ******************************************************************************/

void
PlotDir::NewFileName
	(
	const JString& filename
	)
{
	itsFileName = filename;
	JString title = itsFileName + ":  " + itsPlot->GetTitle();
	GetWindow()->SetTitle(title);
	JString sessiontitle = JGetString("SessionWindowPrefix::PlotDir") + title;
	(itsSessionDir->GetWindow())->SetTitle(sessiontitle);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
PlotDir::WriteSetup
	(
	std::ostream& os
	)
{
	GetWindow()->WriteGeometry(os);
	itsPrinter->WriteXPSSetup(os);
	itsEPSPrinter->WriteX2DEPSSetup(os);
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
PlotDir::ReadSetup
	(
	std::istream&		is,
	const JFloat	gloveVersion
	)
{
	GetWindow()->ReadGeometry(is);
	itsPrinter->ReadXPSSetup(is);
	if (gloveVersion >= 3)
	{
		itsEPSPrinter->ReadX2DEPSSetup(is);
	}
}

/******************************************************************************
 WriteData

 ******************************************************************************/

void
PlotDir::WriteData
	(
	std::ostream& os,
	RaggedFloatTableData* data
	)
{
	os << kCurrentSetupVersion << ' ';

	WriteCurves(os, data);
	itsPlot->PWXWriteSetup(os);
	os << ' ';
	itsPlot->PWWriteCurveSetup(os);
	os << ' ';

	os << itsDiffDirs->GetElementCount() << ' ';

	for (auto* dir : *itsDiffDirs)
	{
		dir->WriteSetup(os);
		dir->WriteData(os, data);
	}
	itsSessionDir->WriteData(os);
}

/******************************************************************************
 WriteCurves

 ******************************************************************************/

void
PlotDir::WriteCurves
	(
	std::ostream& os,
	RaggedFloatTableData* data
	)
{
	const JSize tempCount = itsPlot->GetCurveCount();
	JSize count = 0;
	JSize i;
	for (i = 1; i <= tempCount; i++)
	{
		J2DPlotDataBase* jpdb = itsPlot->GetCurve(i);
		GloveCurveStats stat = itsCurveStats->GetElement(i);
		if (stat.type == kGDataCurve && jpdb->GetType() == J2DPlotDataBase::kScatterPlot)
		{
			J2DPlotData* pd = dynamic_cast<J2DPlotData*>(jpdb);
			assert( pd != nullptr );
			if (pd->IsValid())
			{
				count++;
			}
		}
		else if (stat.type == kGFitCurve)
		{
			J2DPlotDataBase* pdb = itsPlot->GetCurve(stat.provider);
			J2DPlotData* pd = dynamic_cast<J2DPlotData*>(pdb);
			assert( pd != nullptr );
			if (pd->IsValid())
			{
				count++;
			}
		}
		else
		{
			count++;
		}
	}

	os << count << ' ';
	for (i = 1; i <= tempCount; i++)
	{
		J2DPlotDataBase* jpdb = itsPlot->GetCurve(i);
		GloveCurveStats stat = itsCurveStats->GetElement(i);
		if (stat.type == kGDataCurve)
		{
			if (jpdb->GetType() == J2DPlotDataBase::kScatterPlot)
			{
				J2DPlotData* pd = dynamic_cast<J2DPlotData*>(jpdb);
				assert( pd != nullptr );
				if (pd->IsValid())
				{
					os << (int)kGDataCurve << ' ';
					os << (int)jpdb->GetType() << ' ';
					JIndex index;
					data->FindColumn(const_cast< JArray<JFloat>* >(&(pd->GetXData())), &index);
					os << index << ' ';
					data->FindColumn(const_cast< JArray<JFloat>* >(&(pd->GetYData())), &index);
					os << index << ' ';
					os << JBoolToString(pd->HasXErrors())
					   << JBoolToString(pd->HasYErrors()) << ' ';
					if (pd->HasXErrors())
					{
						const JArray<JFloat>* testArray;
						pd->GetXPErrorData(&testArray);
						data->FindColumn(testArray, &index);
						os << index << ' ';
					}
					if (pd->HasYErrors())
					{
						const JArray<JFloat>* testArray;
						pd->GetYPErrorData(&testArray);
						data->FindColumn(testArray, &index);
						os << index << ' ';
					}
				}
			}
			else if (jpdb->GetType() == J2DPlotDataBase::kVectorPlot)
			{
				J2DVectorData* vd = dynamic_cast<J2DVectorData*>(jpdb);
				assert( vd != nullptr );
				os << (int)kGDataCurve << ' ';
				os << (int)jpdb->GetType() << ' ';
				JIndex index;
				const JArray<JFloat>* carray;
				carray	= &(vd->GetXData());
				JArray<JFloat>* array = const_cast< JArray<JFloat>* >(carray);
				data->FindColumn(array, &index);
				os << index << ' ';
				carray	= &(vd->GetYData());
				array = const_cast< JArray<JFloat>* >(carray);
				data->FindColumn(array, &index);
				os << index << ' ';
				carray	= &(vd->GetVXData());
				array = const_cast< JArray<JFloat>* >(carray);
				data->FindColumn(array, &index);
				os << index << ' ';
				carray	= &(vd->GetVYData());
				array = const_cast< JArray<JFloat>* >(carray);
				data->FindColumn(array, &index);
				os << index << ' ';
			}
		}
		else if (stat.type == kGFitCurve)
		{
			if (stat.fitType == kGModFit)
			{
				os << (int)kGFitCurve << ' ';
				os << (int)stat.fitType << ' ';
				os << stat.provider << ' ';
				PlotModuleFit* mf = dynamic_cast<PlotModuleFit*>(jpdb);
				assert( mf != nullptr );
				mf->WriteData(os);
			}
			else
			{
				J2DPlotDataBase* pdb = itsPlot->GetCurve(stat.provider);
				J2DPlotData* pd = dynamic_cast<J2DPlotData*>(pdb);
				assert( pd != nullptr );
				if (pd->IsValid())
				{
					os << (int)kGFitCurve << ' ';
					os << (int)stat.fitType << ' ';
					os << stat.provider << ' ';

					if (stat.fitType == kGProxyFit)
					{
						PlotFitProxy* pf = dynamic_cast<PlotFitProxy*>(jpdb);
						assert(pf != nullptr);
						pf->WriteData(os);
					}
				}
			}
		}
		else if (stat.type == kGFunctionCurve)
		{
			J2DPlotFunction* pd = dynamic_cast<J2DPlotFunction*>(jpdb);
			assert( pd != nullptr );
			os << (int)kGFunctionCurve << ' ';
			os << pd->GetFunctionString() << ' ';
		}
		else
		{
			os << (int)kGDiffCurve << ' ';
		}
	}
}

/******************************************************************************
 ReadData

 ******************************************************************************/

void
PlotDir::ReadData
	(
	std::istream& is,
	RaggedFloatTableData* data,
	const JFloat gloveVersion
	)
{
	JFileVersion version;
	is >> version;

	if (version == 0)
	{
		itsPlot->PWReadSetup(is);
		ReadCurves(is,data);
	}
	else if (version == 1)
	{
		ReadCurves(is,data);
		itsPlot->PWReadSetup(is);
	}
	else
	{
		ReadCurves(is,data);
		itsPlot->PWXReadSetup(is);
	}

	if (version <= kCurrentSetupVersion)
	{
		itsPlot->PWReadCurveSetup(is);

		JSize diffCount;
		is >> diffCount;
		assert(diffCount == itsDiffDirs->GetElementCount());
		for (JIndex i = 1; i <= diffCount; i++)
		{
			(itsDiffDirs->GetElement(i))->ReadSetup(is, gloveVersion);
			(itsDiffDirs->GetElement(i))->ReadData(is, data, gloveVersion);
		}

		itsSessionDir->ReadData(is);
	}
}

/******************************************************************************
 ReadCurves (private)

 ******************************************************************************/

void
PlotDir::ReadCurves
	(
	std::istream& is,
	RaggedFloatTableData* data
	)
{
	JSize curveCount;
	is >> curveCount;

	for (JIndex j = 1; j <= curveCount; j++)
	{
		GCurveType type;
		int temp;
		is >> temp;
		type = (GCurveType)temp;
		if (type == kGDataCurve)
		{
			is >> temp;
			if (temp == J2DPlotDataBase::kScatterPlot)
			{
				JIndex xIndex, yIndex, xErrIndex, yErrIndex;
				bool xerrors, yerrors;
				is >> xIndex >> yIndex;
				is >> JBoolFromString(xerrors) >> JBoolFromString(yerrors);
				if (xerrors)
				{
					is >> xErrIndex;
				}
				if (yerrors)
				{
					is >> yErrIndex;
				}
				itsCurrentCurveType = kGDataCurve;
				J2DPlotData* curve;
				if (J2DPlotData::Create(&curve, data->GetColPointer(xIndex),
										data->GetColPointer(yIndex), true))
				{
					itsPlot->AddCurve(curve, true, JString::empty);

					if (xerrors)
					{
						curve->SetXErrors(data->GetColPointer(xErrIndex));
					}
					if (yerrors)
					{
						curve->SetYErrors(data->GetColPointer(yErrIndex));
					}
				}
			}
			else if (temp == J2DPlotDataBase::kVectorPlot)
			{
				JIndex xIndex, yIndex, vxIndex, vyIndex;
				is >> xIndex >> yIndex >> vxIndex >> vyIndex;
				itsCurrentCurveType = kGDataCurve;
				J2DVectorData* curve;
				if (J2DVectorData::Create(&curve, data->GetColPointer(xIndex),
										  data->GetColPointer(yIndex),
										  data->GetColPointer(vxIndex),
										  data->GetColPointer(vyIndex), true))
				{
					itsPlot->AddCurve(curve, true, JString::empty, false, false);
				}
			}
		}
		else if (type == kGFitCurve)
		{
			int temp;
			is >> temp;
			GCurveFitType ftype = (GCurveFitType)temp;
			JIndex provider;
			is >> provider;
			if (ftype == kGModFit)
			{
				J2DPlotDataBase* pdata = itsPlot->GetCurve(provider);
				PlotModuleFit* mf = jnew PlotModuleFit(itsPlot, pdata, is);
				assert(mf != nullptr);
				if (!AddFitModule(mf, pdata))
				{
					jdelete mf;
				}
			}
			else if (ftype == kGProxyFit)
			{
				J2DPlotDataBase* pdata = itsPlot->GetCurve(provider);
				PlotFitProxy* pf = jnew PlotFitProxy(itsPlot, pdata, is);
				assert(pf != nullptr);

				itsFits->Append(pf);
				AddFit(pf, provider, kGProxyFit);
			}
			else
			{
				NewFit(provider, ftype);
			}
		}
		else if (type == kGFunctionCurve)
		{
			JString fnString;
			is >> fnString;
			J2DPlotFunction* pf;
			JFloat min, max, inc;
			itsPlot->GetXScale(&min, &max, &inc);
			if (J2DPlotFunction::Create(&pf, itsPlot, itsVarList, fnString, itsXVarIndex, min, max))
			{
				itsCurrentCurveType = kGFunctionCurve;
				itsPlot->AddCurve(pf, true, pf->GetFunctionString(), true, false);
				itsCurrentCurveType = kGDataCurve;
			}
		}
	}
}

/******************************************************************************
 OKToClose

 ******************************************************************************/

bool
PlotDir::OKToClose()
{
	return true;
}

/******************************************************************************
 OKToRevert

 ******************************************************************************/

bool
PlotDir::OKToRevert()
{
	return true;
}

/******************************************************************************
 CanRevert

 ******************************************************************************/

bool
PlotDir::CanRevert()
{
	return true;
}

/******************************************************************************
 NeedsSave

 ******************************************************************************/

bool
PlotDir::NeedsSave()
	const
{
	return false;
}

/******************************************************************************
 HandlePlotMenu

 ******************************************************************************/

void
PlotDir::HandlePlotMenu
	(
	const JIndex index
	)
{
	if (index == kPageSetupCmd)
	{
		if (itsPrinter->EditUserPageSetup())
		{
			itsSupervisor->DataModified();
			// for WYSISYG: adjust display to match new paper size
		}
	}
	else if (index == kPrintCmd)
	{
		if (itsPrinter->ConfirmUserPrintSetup())
		{
			itsPlot->Print(*itsPrinter);
		}
	}
	else if (index == kPrintSessionCmd)
	{
		if (itsPrinter->ConfirmUserPrintSetup())
		{
			itsPlot->Print(*itsPrinter);

			itsFitParmsDir->SendAllToSession();
			itsSessionDir->Print();
		}
	}

	else if (index == kPrintPlotEPSCmd)
	{
		itsPlot->PrintPlotEPS();
	}
	else if (index == kPrintMarksEPSCmd)
	{
		itsPlot->PrintMarksEPS();
	}

	else if (index == kCloseCmd)
	{
		if (itsHideOnClose)
		{
			Deactivate();
		}
		else
		{
			Close();
		}
	}
}

/******************************************************************************
 HandleAnalysisMenu

 ******************************************************************************/

void
PlotDir::HandleAnalysisMenu
	(
	const JIndex index
	)
{
	if (index == kPlotFunctionCmd)
	{
		CreateFunction();
	}
	else if (index == kFitWindowCmd)
	{
		FitDirector* dir = jnew FitDirector(this, itsPlot, itsFileName);
		assert(dir != nullptr);
		dir->Activate();
	}
}

/******************************************************************************
 CreateFunction

 ******************************************************************************/

void
PlotDir::CreateFunction()
{
	auto* dlog = jnew PlotFunctionDialog(itsVarList);
	assert( dlog != nullptr);

	if (dlog->DoDialog())
	{
		JExprParser p(itsVarList);

		JFunction* f;
		if (p.Parse(dlog->GetFunctionString(), &f))
		{
			PlotFunction(f);
		}
	}
}

/******************************************************************************
 PlotFunction

 ******************************************************************************/

void
PlotDir::PlotFunction
	(
	JFunction* f
	)
{
	JFloat min, max, inc;
	itsPlot->GetXScale(&min, &max, &inc);
	J2DPlotFunction* pf = jnew J2DPlotFunction(itsPlot, itsVarList, f, true, itsXVarIndex, min, max);
	itsCurrentCurveType = kGFunctionCurve;
	itsPlot->AddCurve(pf, true, f->Print(), true, false);
	itsCurrentCurveType = kGDataCurve;
}

/******************************************************************************
 UpdateFitParmsMenu

 ******************************************************************************/

void
PlotDir::UpdateFitParmsMenu()
{
	itsFitParmsMenu->RemoveAllItems();
	BuildColumnMenus("FitMenuItem::global", itsFits->GetElementCount(),
					   itsFitParmsMenu, nullptr);

	if (itsFits->IsEmpty())
	{
		itsAnalysisMenu->DisableItem(kFitParmsCmd);
	}
}

/******************************************************************************
 UpdateDiffMenu

 ******************************************************************************/

void
PlotDir::UpdateDiffMenu()
{
	itsDiffMenu->RemoveAllItems();
	BuildColumnMenus("DiffMenuItem::PlotDir", itsDiffDirs->GetElementCount(),
					   itsDiffMenu, nullptr);

	if (itsDiffDirs->IsEmpty())
	{
		itsAnalysisMenu->DisableItem(kDiffPlotCmd);
	}
}

/******************************************************************************
 SelectFitModule

 ******************************************************************************/

void
PlotDir::SelectFitModule()
{
	auto* dlog = jnew FitModuleDialog();
	assert( dlog != nullptr);

	if (dlog->DoDialog())
	{
		J2DPlotDataBase* data = itsPlot->GetCurve(itsCurveForFit);

		JString modName;
		GetApplication()->GetFitModulePath(dlog->GetFilterIndex(), &modName);

		FitModule* fm;
		if (!FitModule::Create(&fm, this, data, modName))
		{
			JGetUserNotification()->ReportError(JGetString("UnknownError::FitModule"));
		}
	}
}

/******************************************************************************
 HandleCurveAdded

 ******************************************************************************/

void
PlotDir::HandleCurveAdded()
{
	GloveCurveStats stats;
	stats.type = itsCurrentCurveType;
	stats.provider = 0;
	stats.fitNumber = 0;
	itsCurveStats->AppendElement(stats);
}

/******************************************************************************
 HandleCurveRemoved

 ******************************************************************************/

void
PlotDir::HandleCurveRemoved
	(
	const JIndex index
	)
{
	GloveCurveStats stats = itsCurveStats->GetElement(index);
	if (stats.type == kGFitCurve)
	{
		RemoveFit(index);
	}
	itsCurveStats->RemoveElement(index);
	const JSize ccount = itsCurveStats->GetElementCount();
	JSize count = ccount;
	JSize i;
	if (stats.type == kGDataCurve)
	{
		i = index;
		while (i <= count)
		{
			GloveCurveStats statsi = itsCurveStats->GetElement(i);
			if (statsi.type == kGFitCurve)
			{
				if (statsi.provider == index)
				{
					itsPlot->RemoveCurve(i);
					count --;
				}
				else
				{
					statsi.provider--;
					itsCurveStats->SetElement(i, statsi);
					i++;
				}
			}
			else
			{
				i++;
			}
		}
	}
	else if (stats.type == kGFunctionCurve)
	{
		for (i = index + 1; i <= count; i++)
		{
			GloveCurveStats statsi = itsCurveStats->GetElement(i);
			if (statsi.type == kGFitCurve)
			{
				statsi.provider--;
				itsCurveStats->SetElement(i, statsi);
			}
		}
	}
}

/******************************************************************************
 RemoveFit

 ******************************************************************************/

void
PlotDir::RemoveFit
	(
	const JIndex index
	)
{
	GloveCurveStats stats = itsCurveStats->GetElement(index);
	itsFits->RemoveElement(stats.fitNumber);
	PlotDir* dir = itsDiffDirs->GetElement(stats.fitNumber);
	dir->Close();
	itsDiffDirs->RemoveElement(stats.fitNumber);
	const JSize count = itsCurveStats->GetElementCount();
	for (JIndex i = index + 1; i <= count; i++)
	{
		stats = itsCurveStats->GetElement(i);
		if (stats.type == kGFitCurve)
		{
			stats.provider--;
			stats.fitNumber--;
			itsCurveStats->SetElement(i, stats);
		}
	}
}

/******************************************************************************
 NewFit

 ******************************************************************************/

void
PlotDir::NewFit
	(
	const JIndex plotindex,
	const GCurveFitType type
	)
{
	J2DPlotDataBase* data = itsPlot->GetCurve(plotindex);
	PlotFitFunction* df = nullptr;
	if (type == kLinearFit)
	{
		PlotLinearFit* lf;
		JFloat xmax, xmin, ymax, ymin;
		if (itsPlot->IsUsingRange())
		{
			itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			lf = jnew PlotLinearFit(itsPlot, data, xmin, xmax, ymin, ymax);
		}
		else
		{
			data->GetXRange(&xmin, &xmax);
			lf = jnew PlotLinearFit(itsPlot, data, xmin, xmax);
		}
		assert(lf != nullptr);
		itsFits->Append(lf);
		df = lf;
	}
	else if (type == kGExpFit)
	{
		PlotLinearFit* lf;
		JFloat xmax, xmin, ymax, ymin;
		itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
		if (itsPlot->IsUsingRange())
		{
			itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			lf = jnew PlotLinearFit(itsPlot, data, xmin, xmax, ymin, ymax, false, true);
		}
		else
		{
			data->GetXRange(&xmin, &xmax);
			lf = jnew PlotLinearFit(itsPlot, data, xmin, xmax, false, true);
		}
		assert(lf != nullptr);
		itsFits->Append(lf);
		df = lf;
//		PlotExpFit* ef = jnew PlotExpFit(itsPlot, data);
//		assert(ef != nullptr);
//		itsFits->Append(ef);
//		df = ef;
	}
	else if (type == kGQuadFit)
	{
		PlotQuadFit* lf;
		JFloat xmax, xmin, ymax, ymin;
		if (itsPlot->IsUsingRange())
		{
			itsPlot->GetRange(&xmin, &xmax, &ymin, &ymax);
			lf = jnew PlotQuadFit(itsPlot, data, xmin, xmax, ymin, ymax);
		}
		else
		{
			data->GetXRange(&xmin, &xmax);
			lf = jnew PlotQuadFit(itsPlot, data, xmin, xmax);
		}
		assert(lf != nullptr);
		itsFits->Append(lf);
		df = lf;
	}
	else
	{
		assert( false );
	}
	AddFit(df, plotindex, type);
}

/******************************************************************************
 AddDiffCurve

 ******************************************************************************/

void
PlotDir::AddDiffCurve
	(
	J2DPlotDataBase* ddata
	)
{
	itsCurrentCurveType = kGDiffCurve;
	itsPlot->AddCurve(ddata, false, JGetString("DiffCurveName::PlotDir"), false, true);
	itsCurrentCurveType = kGDataCurve;
}

/******************************************************************************
 AddFitModule

 ******************************************************************************/

bool
PlotDir::AddFitModule
	(
	PlotModuleFit* fit,
	J2DPlotDataBase* fitData
	)
{
	JIndex plotindex;
	bool found = itsPlot->GetCurveIndex(fitData, &plotindex);
	if (!found)
	{
		return false;
	}
	itsFits->Append(fit);
	AddFit(fit, plotindex, kGModFit);
	return true;
}

/******************************************************************************
 AddFit

 ******************************************************************************/

void
PlotDir::AddFit
	(
	PlotFitFunction* fit,
	const JIndex plotindex,
	const GCurveFitType type
	)
{
	itsCurrentCurveType = kGFitCurve;
	JIndex i = itsPlot->AddCurve(fit, true, fit->GetFunctionString(), true, false);
	itsCurrentCurveType = kGDataCurve;
	GloveCurveStats stats = itsCurveStats->GetElement(i);
	stats.type		= kGFitCurve;
	stats.provider	= plotindex;
	stats.fitNumber = itsFits->GetElementCount();
	stats.fitType	= type;
	itsCurveStats->SetElement(i, stats);
	itsAnalysisMenu->EnableItem(kFitParmsCmd);
	itsAnalysisMenu->EnableItem(kDiffPlotCmd);

	PlotDir* dir = jnew PlotDir(this, itsSupervisor, itsFileName, true);
	assert( dir != nullptr );
	JXGetDocumentManager()->DocumentMustStayOpen(dir, true);
	J2DPlotDataBase* ddata = fit->GetDiffData();
	dir->AddDiffCurve(ddata);
	J2DPlotWidget* plot = dir->GetPlot();
	JString numS((JUInt64)itsFits->GetElementCount());

	const JUtf8Byte* map[] =
	{
		"n", numS.GetBytes()
	};
	const JString str = JGetString("DiffMenuItem::PlotDir", map, sizeof(map));
	plot->SetTitle(str);
	plot->SetXLabel(itsPlot->GetXLabel());
	plot->SetYLabel(itsPlot->GetYLabel());
	plot->ProtectCurve(1, true);
	plot->ShowFrame(false);
	itsDiffDirs->Append(dir);
}

/******************************************************************************
 AddFitProxy (public)

 ******************************************************************************/

void
PlotDir::AddFitProxy
	(
	PlotFitProxy*		fit,
	const JIndex		index,
	const JString&		name
	)
{
	itsFits->Append(fit);
	AddFit(fit, index, kGProxyFit);
	itsPlot->SetCurveName(itsPlot->GetCurveCount(), name);
}

/******************************************************************************
 CurveIsFit (public)

 ******************************************************************************/

bool
PlotDir::CurveIsFit
	(
	const JIndex index
	)
	const
{
	assert(itsCurveStats->IndexValid(index));
	GloveCurveStats stat = itsCurveStats->GetElement(index);
	if (stat.type == kGFitCurve)
	{
		return true;
	}
	return false;
}


/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
PlotDir::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
	{
		GetApplication()->DisplayAbout();
	}
	else if (index == kTOCCmd)
	{
		JXGetHelpManager()->ShowTOC();
	}
	else if (index == kThisWindowCmd)
	{
		JXGetHelpManager()->ShowSection("PlotHelp");
	}
	else if (index == kChangesCmd)
	{
		JXGetHelpManager()->ShowChangeLog();
	}
	else if (index == kCreditsCmd)
	{
		JXGetHelpManager()->ShowCredits();
	}
}

/******************************************************************************
 SafetySave

 ******************************************************************************/

void
PlotDir::SafetySave
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
}

/******************************************************************************
 DiscardChanges

 ******************************************************************************/

void
PlotDir::DiscardChanges()
{
}

/******************************************************************************
 Close (public)

 ******************************************************************************/

bool
PlotDir::Close()
{
	itsPlotIsClosing = true;
	return JXDocument::Close();
}
