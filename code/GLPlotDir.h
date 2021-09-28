/******************************************************************************
 GLPlotDir.h

	Interface for the GLPlotDir class

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GLPlotDir
#define _H_GLPlotDir

#include <jx-af/jx/JXDocument.h>
#include <jx-af/jcore/JPtrArray.h>
#include "GloveCurveStats.h"
#include <jx-af/jcore/JXPM.h>

class GLPlotter;
class JString;
class JFunction;
class GLVarList;
class JXTextMenu;
class GLPlotFunctionDialog;
class GLFitBase;
class GLFitParmsDir;
class GLRaggedFloatTableData;
class JXFileDocument;
class J2DPlotDataBase;
class GLHistoryDir;
class JXPSPrinter;
class GLFitModuleDialog;
class GLPlotModuleFit;
class GLPlotFitFunction;
class GLPlotFitProxy;
class JX2DPlotEPSPrinter;

class GLPlotDir : public JXDocument
{
public:

	GLPlotDir(JXDirector* supervisor, JXFileDocument* notifySupervisor,
			const JString& filename, const bool hideOnClose = false);

	virtual ~GLPlotDir();

	GLPlotter*			GetPlot();
	void 				NewFileName(const JString& filename);

	void 				WriteSetup(std::ostream& os);
	void 				ReadSetup(std::istream& is, const JFloat gloveVersion);

	void 				WriteData(std::ostream& os, GLRaggedFloatTableData* data);
	void 				ReadData(std::istream& is, GLRaggedFloatTableData* data, const JFloat gloveVersion);
	virtual bool	NeedsSave() const override;
	GLHistoryDir*		GetSessionDir();
	bool			AddFitModule(GLPlotModuleFit* fit, J2DPlotDataBase* fitData);
	virtual void		SafetySave(const JXDocumentManager::SafetySaveReason reason) override;
	virtual bool	GetMenuIcon(const JXImage** icon) const override;

	void				AddFitProxy(GLPlotFitProxy* fit, const JIndex index, const JString& name);

	virtual bool	Close() override;

	bool			CurveIsFit(const JIndex index) const;

protected:

	virtual bool	OKToClose() override;
	virtual bool	OKToRevert() override;
	virtual bool	CanRevert() override;
	virtual void	DiscardChanges() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	GLPlotter* 					itsPlot;
	JString 					itsFileName;
	GLVarList*					itsVarList;
	JXTextMenu*					itsPlotMenu;
	JXTextMenu*					itsAnalysisMenu;
	JXTextMenu*					itsFitParmsMenu;
	JXTextMenu*					itsDiffMenu;
	JXTextMenu*					itsHelpMenu;
	JIndex						itsXVarIndex;
	GLPlotFunctionDialog*		itsFunctionDialog;
	JPtrArray<GLFitBase>*		itsFits;
	GLFitParmsDir*				itsFitParmsDir;
	JArray<GloveCurveStats>*	itsCurveStats;
	GCurveType					itsCurrentCurveType;
	JXFileDocument*				itsSupervisor;
	JPtrArray<GLPlotDir>*		itsDiffDirs;
	bool						itsHideOnClose;
	GLHistoryDir*				itsSessionDir;
	JXPSPrinter*				itsPrinter;
	JX2DPlotEPSPrinter*			itsEPSPrinter;
	bool						itsIsPrintAll;
	GLFitModuleDialog*			itsFitModuleDialog;
	JIndex						itsCurveForFit;

	bool	itsPlotIsClosing;

private:

	void	WriteCurves(std::ostream& os, GLRaggedFloatTableData* data);
	void 	ReadCurves(std::istream& is, GLRaggedFloatTableData* data);

	void	HandlePlotMenu(const JIndex index);

	void	HandleAnalysisMenu(const JIndex index);
	void	CreateFunction();
	void 	PlotFunction(JFunction* f);
	void	SelectFitModule();
	void	UpdateFitParmsMenu();

	void	UpdateDiffMenu();

	void	HandleHelpMenu(const JIndex index);

	void	HandleCurveRemoved(const JIndex index);
	void	HandleCurveAdded();
	void	RemoveFit(const JIndex index);
	void	NewFit(const JIndex plotindex, const GCurveFitType type);
	void	AddDiffCurve(J2DPlotDataBase* ddata);
	void	AddFit(GLPlotFitFunction* fit, const JIndex plotIndex,
					const GCurveFitType type);
};

/******************************************************************************
 GetPlot

 ******************************************************************************/

inline GLPlotter*
GLPlotDir::GetPlot()
{
	return itsPlot;
}

/******************************************************************************
 GetSessionDir

 ******************************************************************************/

inline GLHistoryDir*
GLPlotDir::GetSessionDir()
{
	return itsSessionDir;
}

#endif
