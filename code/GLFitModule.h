/*********************************************************************************
 GLFitModule.h

	Interface for the GLFitModule class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_GLFitModule
#define _H_GLFitModule

#include <jx-af/jcore/JPtrArray.h>

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <jx-af/jcore/JMessageProtocol.h>	// template which requires ace includes

class GLPlotDir;
class JProgressDisplay;
class JProcess;
class JString;
class J2DPlotDataBase;
class JOutPipeStream;

class GLFitModule : virtual public JBroadcaster
{
public:

	static bool Create(	GLFitModule** module,
							GLPlotDir* dir,
							J2DPlotDataBase* fitData,
							const JString& sysCmd);
	virtual ~GLFitModule();

protected:

	GLFitModule(GLPlotDir* dir, J2DPlotDataBase* fitData, JProcess* process,
				const int fd, JOutPipeStream* output);

	virtual void	Receive(JBroadcaster* sender,
							const JBroadcaster::Message& message);

private:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	ProcessLink;

private:

	GLPlotDir* 			itsDir;			// We don't own this.
	J2DPlotDataBase*		itsData;		// We don't own this.
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

	void		HandleInput(JString& str);
	void		HandleDataRead(JString& str);
	void		HandleFit();

	// not allowed

	GLFitModule(const GLFitModule&) = delete;
	GLFitModule& operator=(const GLFitModule&) = delete;
};

#endif

