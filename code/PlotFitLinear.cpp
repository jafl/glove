/*********************************************************************************
 PlotFitLinear.cpp

	PlotFitLinear class.

	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "PlotFitLinear.h"
#include <jx-af/j2dplot/J2DPlotWidget.h>
#include <jx-af/j2dplot/J2DPlotDataBase.h>

#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JMatrix.h>
#include <jx-af/jcore/JVector.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jAssert.h>

/*********************************************************************************
 Constructor

 ********************************************************************************/

PlotFitLinear::PlotFitLinear
	(
	J2DPlotWidget*		plot,
	J2DPlotDataBase*	fitData,
	const JFloat		xMin,
	const JFloat		xMax
	)
	:
	PlotFitLinearEq(plot, fitData, xMin, xMax)
{
	JPlotFitLinearX();
}

PlotFitLinear::PlotFitLinear
	(
	J2DPlotWidget* plot,
	J2DPlotDataBase* fitData,
	const JFloat xmin,
	const JFloat xmax,
	const JFloat ymin,
	const JFloat ymax
	)
	:
	PlotFitLinearEq(plot, fitData, xmin, xmax, ymin, ymax)
{
	JPlotFitLinearX();
}

void
PlotFitLinear::JPlotFitLinearX()
{
	JArray<JIndex> powers;
	powers.AppendItem(0);
	powers.AppendItem(1);
	InitializePolynomial(powers);
}


/*********************************************************************************
 Destructor

 ********************************************************************************/

PlotFitLinear::~PlotFitLinear()
{
}
