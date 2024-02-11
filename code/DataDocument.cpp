/******************************************************************************
 DataDocument.cpp

	BASE CLASS = JXFileDocument

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "DataDocument.h"
#include "RaggedFloatTable.h"
#include "gloveModule.h"
#include "Plotter.h"
#include "PlotApp.h"
#include "ChooseFileImportDialog.h"
#include "GetDelimiterDialog.h"
#include "PlotDirector.h"
#include "RowHeaderWidget.h"
#include "ColHeaderWidget.h"
#include "RaggedFloatTableData.h"
#include "globals.h"
#include "PrefsMgr.h"

#include <jx-af/j2dplot/JX2DPlotWidget.h>
#include <jx-af/j2dplot/J2DPlotData.h>
#include <jx-af/j2dplot/J2DVectorData.h>

#include <jx-af/jx/JXToolBar.h>

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jx/JXDocumentMenu.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXScrollbar.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jx/JXSaveFileDialog.h>
#include <jx-af/jx/JXCloseDirectorTask.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>

#include <jx-af/jcore/JLatentPG.h>
#include <jx-af/jcore/JOutPipeStream.h>
#include <jx-af/jcore/JStringIterator.h>

#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <sstream>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kGloveFileSignature = "*** Glove File Format ***";
static const JFileVersion kCurrentGloveVersion = 3;

// version 2:
//	Fixed problem with PlotDirector::WriteData() (needed a space)
//	Changed PlotDirector to use PWXRead/WriteSetup()

const JSize kMaxPlotTitleSize = 20;

enum
{
	kDataPlot = 1,
	kVectorPlot
};

static const JUtf8Byte* kInternalModuleNames[] =
{
	"Text (delimited)",
	"Text (fixed-width)"
};

const JSize kInternalModuleCount = sizeof(kInternalModuleNames)/sizeof(JUtf8Byte*);

enum
{
	kDelimitedText = 1,
	kFixedWidthText
};

/******************************************************************************
 Constructor

 ******************************************************************************/

DataDocument::DataDocument
	(
	JXDirector*		supervisor,
	const JString&	fileName,
	const bool		onDisk
	)
	:
	JXFileDocument(supervisor, fileName, onDisk, false, ".glv")
{
	itsData = jnew RaggedFloatTableData(0.0);

	itsPlotWindows = jnew JPtrArray<PlotDirector>(JPtrArrayT::kForgetAll);
	assert( itsPlotWindows != nullptr );

	itsPrinter = nullptr;

	BuildWindow();

	itsPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPrinter != nullptr );

	itsPlotNumber = 1;
	UpdateExportMenu();
	itsListenToData = true;
	ListenTo(itsData);

	if (onDisk)
	{
		LoadFile(fileName);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DataDocument::~DataDocument()
{
	jdelete itsData;
	jdelete itsPrinter;
	jdelete itsPlotWindows;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "DataDocument-File.h"
#include "DataDocument-Export.h"
#include "Generic-Preferences.h"

void
DataDocument::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 480,370, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 480,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsMgr(), kDataToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 480,340);
	assert( itsToolBar != nullptr );

// end JXLayout

	window->SetMinSize(150, 150);

	itsFileMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::DataDocument_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->AttachHandlers(this,
		&DataDocument::UpdateFileMenu,
		&DataDocument::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	itsExportMenu = jnew JXTextMenu(itsFileMenu, kExportCmd, menuBar);
	itsExportMenu->SetMenuItems(kExportMenuStr);
	itsExportMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsExportMenu->AttachHandler(this, &DataDocument::HandleExportMenu);
	ConfigureExportMenu(itsExportMenu);

	itsScrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( itsScrollbarSet != nullptr );
	itsScrollbarSet->FitToEnclosure();

	AdjustWindowTitle();

	// layout table and headers

	const JCoordinate kRowHeaderWidth  = 30;
	const JCoordinate kColHeaderHeight = 20;

	JXContainer* encl = itsScrollbarSet->GetScrollEnclosure();
	JRect enclApG     = encl->GetApertureGlobal();

	auto* okButton =
		jnew JXTextButton(JGetString("OKLabel::JXGlobal"), encl,
						JXWidget::kFixedLeft, JXWidget::kFixedTop,
						0, 0, kRowHeaderWidth-2, kColHeaderHeight-2);

	itsTable =
		jnew RaggedFloatTable(this, okButton, itsData, 6,
							menuBar, itsScrollbarSet, encl,
							JXWidget::kHElastic, JXWidget::kVElastic,
							kRowHeaderWidth,kColHeaderHeight,
							enclApG.width()  - kRowHeaderWidth,
							enclApG.height() - kColHeaderHeight);

	enclApG = encl->GetApertureGlobal();	// JXScrollableWidget forces a change in this

	jnew RowHeaderWidget(itsTable, itsScrollbarSet, encl,
						  JXWidget::kFixedLeft, JXWidget::kVElastic,
						  0,kColHeaderHeight, kRowHeaderWidth,
						  enclApG.height() - kColHeaderHeight);

	auto* colHeader =
		jnew ColHeaderWidget(itsTable, itsScrollbarSet, encl,
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  kRowHeaderWidth,0, enclApG.width() - kRowHeaderWidth,
							  kColHeaderHeight);
	colHeader->TurnOnColResizing(20);

	auto* windowListMenu =
		jnew JXDocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
			JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	menuBar->AppendMenu(windowListMenu);
	if (JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle)
	{
		windowListMenu->SetShortcuts(JGetString("WindowsMenuShortcut::JXGlobal"));
	}

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::Generic_Preferences"));
	itsPrefsMenu->SetMenuItems(kPreferencesMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &DataDocument::HandlePrefsMenu);
	ConfigurePreferencesMenu(itsPrefsMenu);

	auto* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "TableHelp");

	itsToolBar->LoadPrefs(nullptr);
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kNewCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenCmd);
		itsToolBar->AppendButton(itsFileMenu, kSaveCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kPrintCmd);

		itsTable->LoadDefaultToolButtons(itsToolBar);
		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}

	GetPrefsMgr()->GetWindowSize(kTableWindowSizePrefsID, GetWindow());
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
DataDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsData && itsListenToData)
	{
		DataModified();
	}

	else if (message.Is(J2DPlotWidget::kPlotChanged) ||
			 message.Is(J2DPlotWidget::kTitleChanged) ||
			 message.Is(J2DPlotWidget::kIsEmpty))
	{
		DataModified();
	}

	else
	{
		JXFileDocument::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
DataDocument::UpdateFileMenu()
{
	itsFileMenu->EnableItem(kOpenCmd);
	itsFileMenu->EnableItem(kNewCmd);
	if (NeedsSave())
	{
		itsFileMenu->EnableItem(kSaveCmd);
	}
	itsFileMenu->EnableItem(kSaveAsCmd);
	itsFileMenu->EnableItem(kPageSetupCmd);
	itsFileMenu->EnableItem(kCloseCmd);
	itsFileMenu->EnableItem(kQuitCmd);

	if (!itsData->IsEmpty())
	{
		itsFileMenu->EnableItem(kPrintCmd);
	}
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
DataDocument::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kNewCmd)
	{
		GetApplication()->NewFile();
	}
	else if (index == kOpenCmd)
	{
		auto* dlog = JXChooseFileDialog::Create();
		if (dlog->DoDialog())
		{
			GetApplication()->OpenFile(dlog->GetFullName());
		}
	}
	else if (index == kSaveCmd)
	{
		SaveInCurrentFile();
	}
	else if (index == kSaveAsCmd)
	{
		SaveInNewFile();
	}
	else if (index == kPageSetupCmd)
	{
		itsPrinter->EditUserPageSetup();
	}
	else if (index == kPrintCmd && itsTable->EndEditing())
	{
		if (itsPrinter->ConfirmUserPrintSetup())
		{
			itsTable->PrintRealTable(*itsPrinter);
		}
	}
	else if (index == kCloseCmd)
	{
		Close();
	}

	else if (index == kQuitCmd)
	{
		GetApplication()->Quit();
	}
}

/******************************************************************************
 LoadFile (private)

 ******************************************************************************/

void
DataDocument::LoadFile
	(
	const JString& fileName
	)
{
	std::ifstream is(fileName.GetBytes());

	if (is.bad())
	{
		JGetUserNotification()->ReportError(JGetString("OpenFailed::DataDocument"));
		return;
	}

	const JString str = JReadLine(is);
	if (str == kGloveFileSignature)
	{
		if (!LoadNativeFile(is))
		{
			JGetUserNotification()->ReportError(JGetString("FileTooNew::DataDocument"));
		}
	}
	else
	{
		is.close();
		FileChanged(fileName, false);
		itsCurrentFileName = fileName;
		ChooseFileFilter();
	}
}

/******************************************************************************
 LoadNativeFile (private)

 ******************************************************************************/

bool
DataDocument::LoadNativeFile
	(
	std::istream& is
	)
{
	JFloat version;
	is >> version;

	if (version > (JFloat) kCurrentGloveVersion)
	{
		return false;
	}

	itsListenToData = false;
	itsTable->ReadData(is, version);
	if (version > 0.5)
	{
		ReadPlotData(is, version);
		GetWindow()->ReadGeometry(is);
		JXScrollbar* vscroll = itsScrollbarSet->GetVScrollbar();
		vscroll->ReadSetup(is);
		JXScrollbar* hscroll = itsScrollbarSet->GetHScrollbar();
		hscroll->ReadSetup(is);
	}
	itsListenToData = true;
	DataReverted();
	return true;
}

/******************************************************************************
 ChooseFileFilter (private)

 ******************************************************************************/

void
DataDocument::ChooseFileFilter()
{
	auto* dlog = jnew ChooseFileImportDialog(this, itsCurrentFileName);
	if (!dlog->DoDialog())
	{
		JXCloseDirectorTask::Close(this);
		return;
	}

	const JIndex filterIndex = dlog->GetFilterIndex();
	if (filterIndex == kDelimitedText)
	{
		LoadDelimitedFile(dlog->GetFileText());
		return;
	}
	else if (filterIndex == kFixedWidthText)
	{
		JGetUserNotification()->ReportError(JGetString("LoadFixedWidthFile::DataDocument"));
		return;
	}

	JString filter;
	if (!GetApplication()->GetImportModulePath(filterIndex - kInternalModuleCount, &filter))
	{
		return;
	}

	const JUtf8Byte* argv[] = { filter.GetBytes(), itsCurrentFileName.GetBytes(), nullptr };

	int inFD;
	pid_t pid;
	JError err = JExecute(argv, sizeof(argv), &pid,
						  kJIgnoreConnection, nullptr,
						  kJCreatePipe, &inFD,
						  kJIgnoreConnection, nullptr);

	if (!err.OK())
	{
		JGetUserNotification()->ReportError(JGetString("FilterError::DataDocument"));
		return;
	}

	std::ifstream ip;
	JString tempName;
	if (JConvertToStream(inFD, &ip, &tempName))
	{
		int type;
		ip >> type;

		if (type == kGloveDataError)
		{
			JGetUserNotification()->ReportError(JGetString("FilterFileError::DataDocument"));
			return;
		}

		itsData->ShouldBroadcast(false);
		itsListenToData = false;

		JLatentPG pg(10);
		pg.VariableLengthProcessBeginning(JGetString("FilterProcess::DataDocument"), true, true);
		bool keepGoing = true;

		if (type == kGloveMatrixDataFormat)
		{
			JSize colCount;
			ip >> colCount;

			for (JIndex i = 1; i <= colCount; i++)
			{
				itsData->AppendCol();
			}

			while (keepGoing)
			{
				for (JIndex i = 1; i <= colCount && keepGoing; i++)
				{
					JFloat value;
					ip >> value;
					if (ip.fail() || (ip.eof() && i < colCount))
					{
						keepGoing = false;
						break;
					}
					itsData->AppendItem(i, value);
					keepGoing = pg.IncrementProgress();
				}
			}
		}

		else if (type == kGloveRaggedDataFormat)
		{
			JSize colCount;
			ip >> colCount;

			for (JIndex colIndex=1; colIndex <= colCount && keepGoing; colIndex++)
			{
				itsData->AppendCol();

				JSize rowCount;
				ip >> rowCount;

				for (JIndex rowIndex=1; rowIndex <= rowCount && keepGoing; rowIndex++)
				{
					JFloat value;
					ip >> value;
					if (ip.fail() || (ip.eof() && rowIndex < rowCount))
					{
						keepGoing = false;
						break;
					}
					itsData->AppendItem(colIndex, value);

					keepGoing = pg.IncrementProgress();
				}
			}
		}

		ip.close();
		JRemoveFile(tempName);

		pg.ProcessFinished();
	}

	itsListenToData = true;
	itsData->ShouldBroadcast(true);
}

/******************************************************************************
 CreateNewPlot

 ******************************************************************************/

void
DataDocument::CreateNewPlot
	(
	const JIndex			type,
	const JArray<JFloat>&	xCol,
	const JArray<JFloat>*	x2Col,
	const JArray<JFloat>&	yCol,
	const JArray<JFloat>*	y2Col,
	const bool			linked,
	const JString&			label
	)
{
	JString str(itsPlotNumber);
	const JUtf8Byte* map[] =
	{
		"i", str.GetBytes()
	};
	str = JGetString("PlotTitle::DataDocument", map, sizeof(map));
	itsPlotNumber++;

	auto* plotDir = jnew PlotDirector(this, this, GetFileName());
	assert (plotDir != nullptr);
	itsPlotWindows->Append(plotDir);

	JX2DPlotWidget* plot = plotDir->GetPlot();
	assert( plot != nullptr );
	ListenTo(plot);

	AddToPlot(itsPlotWindows->GetItemCount(), type, xCol, x2Col, yCol, y2Col, linked, label);

	plot->SetTitle(str);
}

/******************************************************************************
 AddToPlot

 ******************************************************************************/

void
DataDocument::AddToPlot
	(
	const JIndex			plotIndex,
	const JIndex			type,
	const JArray<JFloat>&	xCol,
	const JArray<JFloat>*	x2Col,
	const JArray<JFloat>&	yCol,
	const JArray<JFloat>*	y2Col,
	const bool				linked,
	const JString&			label
	)
{
	assert( itsPlotWindows->IndexValid(plotIndex) );

	auto* plotDir = itsPlotWindows->GetItem(plotIndex);
	auto* plot    = plotDir->GetPlot();
	ListenTo(plot);

	if (type == kDataPlot)
	{
		J2DPlotData* curve;
		if (J2DPlotData::Create(&curve, xCol, yCol, linked))
		{
			plot->AddCurve(curve, true, label);
		}
		else
		{
			JGetUserNotification()->ReportError(JGetString("MismatchedColumns::DataDocument"));
			return;
		}
		if (x2Col != nullptr)
		{
			curve->SetXErrors(*x2Col);
		}
		if (y2Col != nullptr)
		{
			curve->SetYErrors(*y2Col);
		}
	}
	else if (type == kVectorPlot)
	{
		J2DVectorData* curve;
		if (J2DVectorData::Create(&curve, xCol, yCol, *x2Col, *y2Col, linked))
		{
			plot->AddCurve(curve, true, label);
		}
		else
		{
			JGetUserNotification()->ReportError(JGetString("MismatchedColumns::DataDocument"));
			return;
		}
	}
	plotDir->Activate();
	DataModified();
}

/******************************************************************************
 GetPlotNames

 ******************************************************************************/

void
DataDocument::GetPlotNames
	(
	JPtrArray<JString>& names
	)
{
	JString* name;

	const JIndex index = itsPlotWindows->GetItemCount();

	for (JIndex i = 1; i <= index; i++)
	{
		auto* plotDir = itsPlotWindows->GetItem(i);
		auto* plot    = plotDir->GetPlot();

		name = jnew JString(plot->GetTitle());
		if (name->GetCharacterCount() > kMaxPlotTitleSize)
		{
			JStringIterator iter(name, JStringIterator::kStartAfterChar, kMaxPlotTitleSize);
			iter.RemoveAllNext();
			name->Append(JGetString("Ellipsis::DataDocument"));
		}
		names.Append(name);
	}
}

/******************************************************************************
 DirectorClosed

 ******************************************************************************/

void
DataDocument::DirectorClosed
	(
	JXDirector* theDirector
	)
{
	JIndex index;
	const auto* dir = (const PlotDirector*) theDirector;
	if (itsPlotWindows->Find(dir, &index))
	{
		itsPlotWindows->Remove(dir);
		DataModified();
	}
}

/******************************************************************************
 WriteTextFile

 ******************************************************************************/

void
DataDocument::WriteTextFile
	(
	std::ostream&	output,
	const bool		safetySave
	)
	const
{
	output << kGloveFileSignature << std::endl;
	output << kCurrentGloveVersion << std::endl;
	itsTable->WriteData(output);

	JSize plotCount = itsPlotWindows->GetItemCount();
	output << plotCount << " ";

	for (JSize i = 1; i <= plotCount; i++)
	{
		auto* plotDir = itsPlotWindows->GetItem(i);
		plotDir->WriteSetup(output);
		plotDir->WriteData(output, itsData);
	}
	GetWindow()->WriteGeometry(output);
	auto* vscroll = itsScrollbarSet->GetVScrollbar();
	vscroll->WriteSetup(output);
	auto* hscroll = itsScrollbarSet->GetHScrollbar();
	hscroll->WriteSetup(output);
}

/******************************************************************************
 ReadPlotData

 ******************************************************************************/

void
DataDocument::ReadPlotData
	(
	std::istream&	is,
	const JFloat	gloveVersion
	)
{
	JSize count;
	is >> count;
	for (JSize i = 1; i <= count; i++)
	{
		auto* plotDir = jnew PlotDirector(this, this, GetFileName());
		assert (plotDir != nullptr);

		plotDir->ReadSetup(is, gloveVersion);
		plotDir->ReadData(is, itsData, gloveVersion);
		plotDir->Activate();
		itsPlotWindows->Append(plotDir);
	}
	itsPlotNumber = count + 1;
}

/******************************************************************************
 HandleExportMenu

 ******************************************************************************/

void
DataDocument::HandleExportMenu
	(
	const JIndex index
	)
{
	if (index == kReloadModuleCmd)
	{
		UpdateExportMenu();
		return;
	}

	JString modName;
	GetApplication()->GetExportModulePath(index - 1, &modName);

	auto* dlog = JXSaveFileDialog::Create(
			JGetString("ExportPrompt::DataDocument"),
			JGetString("ExportFileName::DataDocument"));
	if (!dlog->DoDialog())
	{
		return;
	}

	const JString fileName = dlog->GetFullName();
	const JUtf8Byte* argv[] = { modName.GetBytes(), fileName.GetBytes(), nullptr };

	int inFD, outFD;
	pid_t pid;
	JError err = JExecute(argv, sizeof(argv), &pid,
						  kJCreatePipe, &outFD,
						  kJCreatePipe, &inFD,
						  kJIgnoreConnection, nullptr);
	if (!err.OK())
	{
		JGetUserNotification()->ReportError(JGetString("ModuleError::DataDocument"));
		return;
	}

	std::ifstream ip;
	JString tempName;
	if (JConvertToStream(inFD, &ip, &tempName))
	{
		JOutPipeStream op(outFD, true);
		assert( op.good() );

		int type;
		ip >> type;

		if (type == kGloveMatrixDataFormat)
		{
			itsTable->ExportDataMatrix(op);
		}
		else
		{
			itsTable->ExportData(op);
		}

		ip.close();
		JRemoveFile(tempName);
	}
}

/******************************************************************************
 UpdateExportMenu

 ******************************************************************************/

void
DataDocument::UpdateExportMenu()
{
	while (itsExportMenu->GetItemCount() > 1)
	{
		itsExportMenu->RemoveItem(2);
	}

	GetApplication()->ReloadExportModules();
	for (const auto* name : *GetApplication()->GetExportModules())
	{
		itsExportMenu->AppendItem(*name);
	}
}

/******************************************************************************
 HandlePrefsMenu

 ******************************************************************************/

void
DataDocument::HandlePrefsMenu
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
		GetPrefsMgr()->SaveWindowSize(kTableWindowSizePrefsID, GetWindow());
	}
}

/******************************************************************************
 DiscardChanges

 ******************************************************************************/

void
DataDocument::DiscardChanges()
{

}

/******************************************************************************
 GetInternalModuleName (public)

 ******************************************************************************/

JString
DataDocument::GetInternalModuleName
	(
	const JIndex index
	)
	const
{
	assert(index <= kInternalModuleCount);
	assert(index > 0);
	return JString(kInternalModuleNames[index - 1], JString::kNoCopy);
}

/******************************************************************************
 GetInternalModuleCount (public)

 ******************************************************************************/

JSize
DataDocument::GetInternalModuleCount()
{
	return kInternalModuleCount;
}

/******************************************************************************
 LoadDelimitedFile (private)

 ******************************************************************************/

void
DataDocument::LoadDelimitedFile
	(
	const JString& fileText
	)
{
	auto* dlog = jnew GetDelimiterDialog(fileText);
	if (!dlog->DoDialog())
	{
		return;
	}

	const GetDelimiterDialog::DelimiterType type = dlog->GetDelimiterType();
	JUtf8Byte delim = ' ';
	if (type == GetDelimiterDialog::kChar)
	{
		delim = dlog->GetCharacter();
	}
	else if (type == GetDelimiterDialog::kTab)
	{
		delim = '\t';
	}

	std::ifstream is(itsCurrentFileName.GetBytes());
	if (is.bad())
	{
		JGetUserNotification()->ReportError(JGetString("OpenFailed::DataDocument"));
		return;
	}

	if (dlog->IsSkippingLines())
	{
		const JSize count = dlog->GetSkipLineCount();
		for (JIndex i = 1; i <= count; i++)
		{
			JIgnoreUntil(is, '\n');
		}
	}

	itsData->ShouldBroadcast(false);
	itsListenToData = false;

	const bool hasComments    = dlog->HasComments();
	const JString& commentStr = dlog->GetCommentString();

	JLatentPG pg(100);
	pg.VariableLengthProcessBeginning(JGetString("FilterProcess::DataDocument"), true, true);

	JIndex row = 0;
	JString line, strVal;
	while (is.good())
	{
		line = JReadLine(is);
		if (line.IsEmpty() || (hasComments && line.StartsWith(commentStr)))
		{
			continue;
		}

		row++;
		std::string s(line.GetRawBytes(), line.GetByteCount());
		std::istringstream iss(s);

		JIndex col = 1;
		if (type == GetDelimiterDialog::kWhiteSpace)
		{
			while (true)
			{
				JFloat value;
				iss >> value;
				if (iss.fail())
				{
					if (col == 1)
					{
						row--;
					}
					break;
				}

				itsData->SetItem(row, col, value);
				col++;
			}
		}
		else
		{
			bool found = true;
			while (found)
			{
				strVal = JReadUntil(iss, delim, &found);
				JFloat value;
				if (strVal.ConvertToFloat(&value))
				{
					itsData->SetItem(row, col, value);
				}
				col++;
			}
		}

		if (!pg.IncrementProgress())
		{
			break;
		}
	}

	pg.ProcessFinished();

	itsListenToData = true;
	itsData->ShouldBroadcast(true);
}
