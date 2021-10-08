/*********************************************************************************
 DataModule.h

	Interface for the DataModule class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_DataModule
#define _H_DataModule

#include <jx-af/jcore/JArray.h>

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <jx-af/jcore/JMessageProtocol.h>	// template which requires ace includes

class RaggedFloatTable;
class RaggedFloatTableData;
class JProcess;
class JOutPipeStream;
//class JIPCIOStream;
class JProgressDisplay;
class JString;

class DataModule : virtual public JBroadcaster
{
public:

	static bool Create(	DataModule** module,
							RaggedFloatTable* table,
							RaggedFloatTableData* data,
							const JString& sysCmd);
	~DataModule() override;

protected:

	DataModule(	RaggedFloatTable* table, RaggedFloatTableData* data,
				JProcess* process, const int fd, JOutPipeStream* output);

	void	Receive(JBroadcaster* sender, const JBroadcaster::Message& message) override;

private:

	using ProcessLink = JMessageProtocol<ACE_LSOCK_STREAM>;

private:

	RaggedFloatTable*		itsTable;
	RaggedFloatTableData*	itsData;
	JOutPipeStream*			itsOutput;
	bool					itsHeaderRead;
	bool					itsSentData;
	bool					itsDataIsDump;
	JSize					itsColNum;
	JIndex					itsColStart;
	JProgressDisplay*		itsPG;
	JProcess*				itsProcess;
	JArray<JIndex>*			itsCols;
	ProcessLink*			itsLink;

private:

	void	HandleInput(const JString& str);
	void	HandleDataRead(const JString& str);
	void	HandlePrepareCols(const JSize cols);

	// not allowed

	DataModule(const DataModule&) = delete;
	DataModule& operator=(const DataModule&) = delete;
};

#endif
