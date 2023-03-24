/******************************************************************************
 FitDirector.h

	Interface for the FitDirector class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_FitDirector
#define _H_FitDirector

#include <jx-af/jx/JXWindowDirector.h>

class ParmColHeaderWidget;
class JXExprEditor;
class JXHorizPartition;
class JXVertPartition;
class JXImage;
class JXPSPrinter;
class JXStaticText;
class JXTextMenu;
class JXToolBar;

class PlotFitFunction;
class J2DPlotFunction;
class J2DPlotWidget;
class JX2DPlotWidget;

class CurveNameList;
class FitDescriptionList;
class FitParameterTable;
class PlotDir;

class HistoryDir;

class VarList;

class FitDirector : public JXWindowDirector
{
public:

public:

	FitDirector(PlotDir* supervisor, J2DPlotWidget* plot, const JString& file);
	~FitDirector() override;

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTextMenu*		itsFitMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;
	J2DPlotWidget*	itsPlot;

	CurveNameList*			itsCurveList;
	FitDescriptionList*		itsFitList;
	FitParameterTable*		itsParameterTable;
	ParmColHeaderWidget*	itsParameterColHeader;
	JX2DPlotWidget*			itsFitPlot;
	JX2DPlotWidget*			itsDiffPlot;
	JXStaticText*			itsChiSq;
	PlotFitFunction*		itsCurrentFit;
	J2DPlotFunction*		itsTestFunction;

	JXHorizPartition*		itsMainPartition;
	JXVertPartition*		itsListPartition;
	JXVertPartition*		itsPlotPartition;

	HistoryDir*				itsHistory;

	JXExprEditor*			itsExprWidget;
	VarList*				itsExprVarList;

	PlotDir*				itsDir;
	JXPSPrinter*			itsPrinter;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

// begin parameterLayout


// end parameterLayout

private:

	void Fit();
	void TestFit();
	void Refit();
	void Plot();
	void Print();

	void AddHistoryText(const bool refit = false);

	void RemoveCurves();
	void RemoveFit();

	void UpdateFitMenu();
	void HandleFitMenu(const JIndex index);

	void HandlePrefsMenu(const JIndex index);

	void HandleHelpMenu(const JIndex index);

	void BuildWindow();
};


#endif
