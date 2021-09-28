/*********************************************************************************
 GLPlotFitQuad.cpp
 
	GLPlotFitQuad class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "GLPlotFitQuad.h"
#include "jx-af/j2dplot/J2DPlotWidget.h"
#include "jx-af/j2dplot/J2DPlotDataBase.h"

#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JMatrix.h>
#include <jx-af/jcore/JVector.h>

#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jAssert.h>

/*********************************************************************************
 Constructor 
 

 ********************************************************************************/

GLPlotFitQuad::GLPlotFitQuad
	(
	J2DPlotWidget* 	plot, 
	J2DPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	GLPlotFitLinearEq(plot, fitData, xMin, xMax)
{
	JPlotFitQuadX(plot, fitData);
}

GLPlotFitQuad::GLPlotFitQuad
	(
	J2DPlotWidget* plot, 
	J2DPlotDataBase* fitData,
	const JFloat xmin, 
	const JFloat xmax,
	const JFloat ymin, 
	const JFloat ymax
	)
	:
	GLPlotFitLinearEq(plot, fitData, xmin, xmax, ymin, ymax)
{
	JPlotFitQuadX(plot, fitData);
}

void
GLPlotFitQuad::JPlotFitQuadX
	(
	J2DPlotWidget* plot, 
	J2DPlotDataBase* fitData
	)
{
	JArray<JIndex> powers;
	powers.AppendElement(0);
	powers.AppendElement(1);
	powers.AppendElement(2);
	InitializePolynomial(powers);
}


/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
GLPlotFitQuad::~GLPlotFitQuad()
{
}

