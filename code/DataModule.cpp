/*******************************************************************************
 DataModule.cpp

	DataModule class.

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "DataModule.h"
#include "RaggedFloatTable.h"
#include "RaggedFloatTableData.h"
#include "gloveModule.h"
#include "UndoElementsInsert.h"

#include <jx-af/jx/JXApplication.h>

#include <jx-af/jcore/JOutPipeStream.h>
#include <jx-af/jcore/JProcess.h>
#include <jx-af/jcore/JUserNotification.h>
#include <jx-af/jcore/JCreateProgressDisplay.h>

#include <jx-af/jx/JXDeleteObjectTask.h>
#include <jx-af/jx/jXGlobals.h>

#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

/*********************************************************************************
 Create

 ********************************************************************************/

bool
DataModule::Create
	(
	DataModule** module,
	RaggedFloatTable* table,
	RaggedFloatTableData* data,
	const JString& sysCmd
	)
{
	int inFD;
	int outFD;
	JProcess* process;
	JError err =
			JProcess::Create(&process, sysCmd,
							kJCreatePipe, &outFD,
							kJCreatePipe, &inFD,
							kJIgnoreConnection, nullptr);

	if (err.OK())
	{
		auto op = jnew JOutPipeStream(outFD, true);
		assert( op->good() );
		*module = jnew DataModule(table, data, process, inFD, op);
		return true;
	}

	return false;
}

/*******************************************************************************
 Constructor (protected)

 ******************************************************************************/

DataModule::DataModule
	(
	RaggedFloatTable*	table,
	RaggedFloatTableData*	data,
	JProcess*				process,
	const int				fd,
	JOutPipeStream*		output
	)
{
	itsTable = table;
	itsData = data;
	ListenTo(itsData);
	itsProcess = process;
	ListenTo(itsProcess);

	itsLink = new ProcessLink(fd);
	assert(itsLink != nullptr);
	ListenTo(itsLink);

	itsOutput = output;
//	ListenTo(output);
	itsHeaderRead = false;
	itsSentData = false;
	itsPG = nullptr;
	itsCols = jnew JArray<JIndex>;
	assert(itsCols != nullptr);
}

/*******************************************************************************
 Destructor

 ******************************************************************************/

DataModule::~DataModule()
{
	jdelete itsProcess;
	delete itsLink;
	if (itsPG != nullptr)
	{
		itsPG->ProcessFinished();
		jdelete itsPG;
	}
}

/*******************************************************************************
 Receive

 ******************************************************************************/

void
DataModule::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (message.Is(JProcess::kFinished))
	{
		JGetUserNotification()->DisplayMessage(JGetString("Finished::DataModule"));
	}
	else if (sender == itsLink && message.Is(JMessageProtocolT::kMessageReady))
	{
		if (itsLink->HasMessages())
		{
			JSize count = itsLink->GetMessageCount();
			for (JSize i = 1; i <= count; i++)
			{
				JString str;
				if (itsLink->GetNextMessage(&str))
				{
					HandleInput(str);
				}
			}
		}
	}
	else if (sender == itsData && message.Is(JTableData::kColsRemoved))
	{
		auto info = dynamic_cast<const JTableData::ColsRemoved*>(&message);
		assert( info != nullptr );
		JIndex fIndex	= info->GetFirstIndex();
		JIndex lIndex	= info->GetLastIndex();
		for (JIndex index = fIndex; index <= lIndex; index++)
		{
			for (JIndex i = 1; i <= itsCols->GetItemCount(); i++)
			{
				JIndex cindex = itsCols->GetItem(i);
				if (index < cindex)
				{
					itsCols->SetItem(i, cindex-1);
				}
				else if (index == cindex)
				{
					JGetUserNotification()->ReportError(JGetString("ColumnRemoved::DataModule"));
					JXDeleteObjectTask<DataModule>::Delete(this);
					return;
				}
			}
		}
	}

	else if (sender == itsData && message.Is(JTableData::kColsInserted))
	{
		auto info = dynamic_cast<const JTableData::ColsInserted*>(&message);
		assert( info != nullptr );
		JIndex fIndex	= info->GetFirstIndex();
		JIndex lIndex	= info->GetLastIndex();
		for (JIndex index = fIndex; index <= lIndex; index++)
		{
			for (JIndex i = 1; i <= itsCols->GetItemCount(); i++)
			{
				JIndex cindex = itsCols->GetItem(i);
				if (index <= cindex)
				{
					itsCols->SetItem(i, cindex+1);
				}
			}
		}
	}
}

/*******************************************************************************
 HandleInput

 ******************************************************************************/

void
DataModule::HandleInput
	(
	const JString& str
	)
{
	if (itsHeaderRead)
	{
		HandleDataRead(str);
	}
	else
	{
		std::string s(str.GetRawBytes(), str.GetByteCount());
		std::istringstream iss(s);
		int eval;
		if (!itsSentData)
		{
			iss >> eval;
			iss >> std::ws;
			if (eval == kGloveRequiresData)
			{
				int cols;
				iss >> cols;
				iss >> std::ws;
				bool success = itsTable->WriteDataCols(*itsOutput, cols);
				if (!success)
				{
					JGetUserNotification()->ReportError(JGetString("Error::DataModule"));
					JXDeleteObjectTask<DataModule>::Delete(this);
					return;
				}
				itsSentData = true;
				return;
			}
		}
		iss >> eval;
		iss >> std::ws;
		if (eval == kGloveDataDump)
		{
			itsDataIsDump = true;
		}
		else
		{
			itsDataIsDump = false;
		}
		iss >> eval;
		iss >> std::ws;
		HandlePrepareCols(eval);
		itsHeaderRead = true;
	}
}

/*******************************************************************************
 HandlePrepareCols

 ******************************************************************************/

void
DataModule::HandlePrepareCols
	(
	const JSize cols
	)
{
	itsColNum = cols;
	itsColStart = itsData->GetDataColCount() + 1;
	for (JSize i = 1; i <= cols; i++)
	{
		itsData->AppendCol();
		itsCols->AppendItem(itsColStart - 1 + i);
	}
	auto undo =
		jnew UndoElementsInsert(itsTable, JPoint(itsColStart, 1),
								 JPoint(itsColStart + itsColNum - 1, itsTable->GetRowCount()),
								 UndoElementsBase::kCols);
	itsTable->GetUndoRedoChain()->NewUndo(undo);
}

/*******************************************************************************
 HandleDataRead

 ******************************************************************************/

void
DataModule::HandleDataRead
	(
	const JString& str
	)
{
	if (itsPG == nullptr)
	{
		itsPG = JGetCreatePG()->New();
		itsPG->VariableLengthProcessBeginning(
			JGetString("Loading::DateModule"), true, false);
	}
	std::string s(str.GetRawBytes(), str.GetByteCount());
	std::istringstream iss(s);
	int eval;
	iss >> eval;
	if (eval == kGloveFinished)
	{
		JXDeleteObjectTask<DataModule>::Delete(this);
		return;
	}
	else if (eval == kGloveFail || eval != kGloveOK)
	{
		JGetUserNotification()->ReportError(JGetString("Error::DataModule"));
		JXDeleteObjectTask<DataModule>::Delete(this);
		return;
	}

	bool success = true;

	JFloat value;
	if (itsDataIsDump)
	{
		for (JSize i = 0; i < itsColNum; i++)
		{
			if (iss.good())
			{
				iss >> value;
				const JIndex cindex = itsCols->GetItem(i+1);
				assert(itsData->ColIndexValid(cindex));
				itsData->AppendItem(cindex, value);
			}
			else
			{
				success = false;
				break;
			}
		}
	}
	else
	{
		success = false;

		JSize row, col;
		if (iss.good())
		{
			iss >> row >> col >> value;
			if (col <= itsColNum)
			{
				JIndex cindex = itsCols->GetItem(col);
				assert(itsData->CellValid(row,cindex));
				itsData->SetItem(row, cindex, value);
				success = true;
			}
		}
	}

	if (!success)
	{
		JGetUserNotification()->ReportError(JGetString("Error::DataModule"));
		JXDeleteObjectTask<DataModule>::Delete(this);
		return;
	}

	const bool keepGoing = itsPG->IncrementProgress();
	if (!keepGoing)
	{
		JXDeleteObjectTask<DataModule>::Delete(this);
		return;
	}
	itsTable->Redraw();
}
