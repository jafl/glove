/******************************************************************************
 PlotFitNonLinear.h

	Interface for the PlotFitNonLinear class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_PlotFitNonLinear
#define _H_PlotFitNonLinear

#include "PlotFitBase.h"
#include <jx-af/jcore/JString.h>

class VarList;
class JFunction;
class JVector;

class PlotFitNonLinear : public PlotFitBase
{
public:

	PlotFitNonLinear(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					  const JFloat xMin, const JFloat xMax);
	PlotFitNonLinear(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					  const JFloat xmin, const JFloat xmax,
					  const JFloat ymin, const JFloat ymax);
	~PlotFitNonLinear() override;

	void	SetVarList(VarList* varList);
	void	SetFunction(const JString& function);
	void	SetFPrimed(const JString& fPrimed);
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

	VarList*		itsVarList;			// we don't own this
	JFunction*		itsFunction;
	JFunction*		itsFPrimed;
	JVector*		itsErrors;
};

#endif
