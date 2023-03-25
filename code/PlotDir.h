/******************************************************************************
 PlotDir.h

	Interface for the PlotDir class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_PlotDir
#define _H_PlotDir

#include <jx-af/jx/JXDocument.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include "gloveCurveStats.h"

class Plotter;
class JFunction;
class VarList;
class JXTextMenu;
class FitBase;
class FitParmsDir;
class RaggedFloatTableData;
class JXFileDocument;
class J2DPlotDataBase;
class HistoryDir;
class JXPSPrinter;
class PlotModuleFit;
class PlotFitFunction;
class PlotFitProxy;
class JX2DPlotEPSPrinter;

class PlotDir : public JXDocument
{
public:

	PlotDir(JXDirector* supervisor, JXFileDocument* notifySupervisor,
			const JString& filename, const bool hideOnClose = false);

	~PlotDir() override;

	Plotter*	GetPlot();
	void				NewFileName(const JString& filename);

	void		WriteSetup(std::ostream& os);
	void		ReadSetup(std::istream& is, const JFloat gloveVersion);

	void		WriteData(std::ostream& os, RaggedFloatTableData* data);
	void		ReadData(std::istream& is, RaggedFloatTableData* data, const JFloat gloveVersion);
	bool		NeedsSave() const override;
	HistoryDir*	GetSessionDir();
	bool		AddFitModule(PlotModuleFit* fit, J2DPlotDataBase* fitData);
	void		SafetySave(const JXDocumentManager::SafetySaveReason reason) override;

	void		AddFitProxy(PlotFitProxy* fit, const JIndex index, const JString& name);

	bool		Close() override;

	bool		CurveIsFit(const JIndex index) const;

protected:

	bool	OKToClose() override;
	bool	OKToRevert() override;
	bool	CanRevert() override;
	void	DiscardChanges() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JString						itsFileName;
	VarList*					itsVarList;
	JXTextMenu*					itsPlotMenu;
	JXTextMenu*					itsAnalysisMenu;
	JXTextMenu*					itsFitParmsMenu;
	JXTextMenu*					itsDiffMenu;
	JXTextMenu*					itsHelpMenu;
	JIndex						itsXVarIndex;
	JPtrArray<FitBase>*			itsFits;
	FitParmsDir*				itsFitParmsDir;
	JArray<GloveCurveStats>*	itsCurveStats;
	GCurveType					itsCurrentCurveType;
	JXFileDocument*				itsSupervisor;
	JPtrArray<PlotDir>*			itsDiffDirs;
	bool						itsHideOnClose;
	bool						itsPlotIsClosing;
	HistoryDir*					itsSessionDir;
	JXPSPrinter*				itsPrinter;
	JX2DPlotEPSPrinter*			itsEPSPrinter;
	JIndex						itsCurveForFit;

// begin JXLayout

	Plotter* itsPlot;

// end JXLayout

private:

	void	BuildWindow();

	void	WriteCurves(std::ostream& os, RaggedFloatTableData* data);
	void	ReadCurves(std::istream& is, RaggedFloatTableData* data);

	void	HandlePlotMenu(const JIndex index);

	void	HandleAnalysisMenu(const JIndex index);
	void	CreateFunction();
	void	PlotFunction(JFunction* f);
	void	SelectFitModule();
	void	UpdateFitParmsMenu();

	void	UpdateDiffMenu();

	void	HandleHelpMenu(const JIndex index);

	void	HandleCurveRemoved(const JIndex index);
	void	HandleCurveAdded();
	void	RemoveFit(const JIndex index);
	void	NewFit(const JIndex plotindex, const GCurveFitType type);
	void	AddDiffCurve(J2DPlotDataBase* ddata);
	void	AddFit(PlotFitFunction* fit, const JIndex plotIndex,
					const GCurveFitType type);
};

/******************************************************************************
 GetPlot

 ******************************************************************************/

inline Plotter*
PlotDir::GetPlot()
{
	return itsPlot;
}

/******************************************************************************
 GetSessionDir

 ******************************************************************************/

inline HistoryDir*
PlotDir::GetSessionDir()
{
	return itsSessionDir;
}

#endif
