/*********************************************************************************
 FitModule.h

	Interface for the FitModule class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_FitModule
#define _H_FitModule

#include <jx-af/jcore/JPtrArray.h>

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <jx-af/jcore/JMessageProtocol.h>	// template which requires ace includes

class PlotDirector;
class JProgressDisplay;
class JProcess;
class JString;
class J2DPlotDataBase;
class JOutPipeStream;

class FitModule : virtual public JBroadcaster
{
public:

	static bool Create(	FitModule** module,
						PlotDirector* dir,
						J2DPlotDataBase* fitData,
						const JString& sysCmd);
	~FitModule() override;

protected:

	FitModule(PlotDirector* dir, J2DPlotDataBase* fitData, JProcess* process,
			  const int fd, JOutPipeStream* output);

	void	Receive(JBroadcaster* sender, const JBroadcaster::Message& message) override;

private:

	using ProcessLink = JMessageProtocol<ACE_LSOCK_STREAM>;

private:

	PlotDirector*		itsDir;			// We don't own this.
	J2DPlotDataBase*	itsData;		// We don't own this.
	bool				itsStatusRead;
	bool				itsHeaderRead;
	bool				itsFunctionRead;
	bool				itsHasErrors;
	bool				itsHasGOF;
	JSize				itsParmsCount;
	JProgressDisplay*	itsPG;
	JProcess*			itsProcess;
	JArray<JFloat>*		itsValues;
	JPtrArray<JString>*	itsNames;
	JString				itsFunction;
	ProcessLink*		itsLink;

private:

	void	HandleInput(JString& str);
	void	HandleDataRead(JString& str);
	void	HandleFit();

	// not allowed

	FitModule(const FitModule&) = delete;
	FitModule& operator=(const FitModule&) = delete;
};

#endif
