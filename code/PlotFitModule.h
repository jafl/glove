/******************************************************************************
 PlotFitModule.h

	Interface for the PlotFitModule class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_PlotFitModule
#define _H_PlotFitModule

#include "PlotFitBase.h"
#include <jx-af/jcore/JString.h>

class DLFitModule;
class JVector;

class PlotFitModule : public PlotFitBase
{
public:

	PlotFitModule(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					  const JFloat xMin, const JFloat xMax);
	PlotFitModule(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					  const JFloat xmin, const JFloat xmax,
					  const JFloat ymin, const JFloat ymax);
	~PlotFitModule() override;

	void	SetFitModule(DLFitModule* fit);
	void	SetInitialParameters(const JVector& p);

	bool	GetParameterName(const JIndex index, JString* name) const override;
	bool	GetParameter(const JIndex index, JFloat* value) const override;

	bool	GetParameterError(const JIndex index, JFloat* value) const override;

	bool	GetYValue(const JFloat x, JFloat* y) const override;

protected:

	void	SetCurrentParameters(const JVector& p) override;
	void	SetErrors(const JVector& p) override;
	JFloat	FunctionN(const JFloat x) override;
	JFloat	FunctionNPrimed(const JFloat x) override;

private:

	DLFitModule*	itsModule;			// we don't own this
	JVector*		itsErrors;
	JVector*		itsParameters;
};
#endif
