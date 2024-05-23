/*********************************************************************************
 PlotFitQuad.h

	Interface for the PlotFitQuad class.

	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_PlotFitQuad
#define _H_PlotFitQuad

#include "PlotFitLinearEq.h"

class J2DPlotWidget;

class PlotFitQuad : public PlotFitLinearEq
{
public:

	PlotFitQuad(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
				const JFloat xMin, const JFloat xMax);
	PlotFitQuad(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
				const JFloat xmin, const JFloat xmax,
				const JFloat ymin, const JFloat ymax);
	~PlotFitQuad() override;

private:

	void	JPlotFitQuadX();
};

#endif
