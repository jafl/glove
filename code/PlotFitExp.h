/*********************************************************************************
 PlotFitExp.h

	Interface for the PlotFitExp class.

	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_PlotFitExp
#define _H_PlotFitExp

#include "PlotFitBase.h"
#include <jx-af/jcore/JString.h>

class J2DPlotWidget;

class PlotFitExp : public PlotFitBase
{
public:

	PlotFitExp(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	PlotFitExp(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	~PlotFitExp() override;

	void	GenerateFit();

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

	JFloat	itsAParm;
	JFloat	itsBParm;
	JFloat	itsAErr;
	JFloat	itsBErr;

	JFloat	itsChi2Start;

private:

	void	CalculateFirstPass();

	void	JPlotFitExpX();
};

#endif
