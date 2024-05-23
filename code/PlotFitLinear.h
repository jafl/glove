/*********************************************************************************
 PlotFitLinear.h

	Interface for the PlotFitLinear class.

	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_PlotFitLinear
#define _H_PlotFitLinear

#include "PlotFitLinearEq.h"

class J2DPlotWidget;

class PlotFitLinear : public PlotFitLinearEq
{
public:

	PlotFitLinear(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	PlotFitLinear(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	~PlotFitLinear() override;

private:

	void	JPlotFitLinearX();
};

#endif
