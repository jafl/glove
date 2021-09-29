/******************************************************************************
 PlotFitModule.h

	Interface for the PlotFitModule class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_PlotFitModule
#define _H_PlotFitModule

// Superclass Header
#include <PlotFitBase.h>
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
	virtual ~PlotFitModule();

	void	SetFitModule(DLFitModule* fit);
	void	SetInitialParameters(const JVector& p);

	virtual bool	GetParameterName(const JIndex index, JString* name) const;
	virtual bool	GetParameter(const JIndex index, JFloat* value) const;

	virtual bool	GetParameterError(const JIndex index, JFloat* value) const;

	virtual bool	GetYValue(const JFloat x, JFloat* y) const;

protected:

	virtual void		SetCurrentParameters(const JVector& p);
	virtual void		SetErrors(const JVector& p);
	virtual JFloat		FunctionN(const JFloat x);
	virtual JFloat		FunctionNPrimed(const JFloat x);

private:

	DLFitModule*	itsModule;			// we don't own this
	JVector*		itsErrors;
	JVector*		itsParameters;
};
#endif
