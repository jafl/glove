/*********************************************************************************
 PlotFitPowerLaw.h

	Interface for the PlotFitPowerLaw class.

	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_PlotFitPowerLaw
#define _H_PlotFitPowerLaw

#include "PlotFitBase.h"
#include <jx-af/jcore/JString.h>

class J2DPlotWidget;

class PlotFitPowerLaw : public PlotFitBase
{
public:

	PlotFitPowerLaw(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	PlotFitPowerLaw(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	~PlotFitPowerLaw() override;

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

	void	JPlotFitPowerLawX();
};

#endif
