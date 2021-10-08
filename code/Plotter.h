/******************************************************************************
 Plotter.h

	Interface for the Plotter class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_Plotter
#define _H_Plotter

#include <jx-af/j2dplot/JX2DPlotWidget.h>
#include <jx-af/jcore/JArray.h>

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <jx-af/jcore/JMessageProtocol.h>	// template which requires ace includes

class JXTextMenu;
class HistoryDir;
class JProcess;
class JOutPipeStream;

class Plotter : public JX2DPlotWidget
{
public:

	Plotter(HistoryDir* sessionDir,
				JXMenuBar* menuBar, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~Plotter() override;

protected:

	void	Receive(JBroadcaster* sender, const JBroadcaster::Message& message) override;

private:

	using ProcessLink = JMessageProtocol<ACE_LSOCK_STREAM>;

private:

	JXTextMenu*		itsModuleMenu;
	HistoryDir*		itsSessionDir;
	JProcess*		itsCursorProcess;
	ProcessLink*	itsLink;
	bool			itsIsProcessingCursor;
	bool			itsCursorFirstPass;


private:

	void	UpdateModuleMenu();
	void	HandleModuleMenu(const JIndex index);
};


#endif
