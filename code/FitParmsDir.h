/******************************************************************************
 FitParmsDir.h

	Interface for the FitParmsDir class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_FitParmsDir
#define _H_FitParmsDir

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPtrArray.h>

class JXTextMenu;
class FitBase;
class JXTextButton;
class JXStaticText;
class FitParmsTable;
class PlotDirector;

class FitParmsDir : public JXWindowDirector
{
public:

	FitParmsDir(PlotDirector* supervisor, JPtrArray<FitBase>* fits);

	~FitParmsDir() override;

	void	ShowFit(const JIndex index);
	void	SendAllToSession();

protected:

	void	Receive(JBroadcaster* sender, const JBroadcaster::Message& message) override;

private:

	PlotDirector*		itsPlotDir;
	JPtrArray<FitBase>*	itsFits;	// We don't own this!
	JIndex				itsCurrentIndex;

// begin JXLayout

	FitParmsTable* itsTable;
	JXTextMenu*    itsFitMenu;
	JXTextButton*  itsCloseButton;
	JXTextButton*  itsSessionButton;

// end JXLayout

private:

	void	BuildWindow();
	void	SendToSession(const JIndex index);

	void	HandleFitMenu(const JIndex index);
	void	UpdateFitMenu();
};

#endif
