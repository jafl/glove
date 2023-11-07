/*********************************************************************************
 PlotFitQuad2.cpp

	PlotFitQuad2 class.

	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "PlotFitQuad2.h"
#include <jx-af/j2dplot/J2DPlotWidget.h>
#include <jx-af/j2dplot/J2DPlotDataBase.h>

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

PlotFitQuad2::PlotFitQuad2
	(
	J2DPlotWidget*		plot,
	J2DPlotDataBase*	fitData,
	const JFloat		xMin,
	const JFloat		xMax
	)
	:
	PlotFitBase(plot, fitData, xMin, xMax)
{
	JPlotFitQuad2X();
}

PlotFitQuad2::PlotFitQuad2
	(
	J2DPlotWidget* plot,
	J2DPlotDataBase* fitData,
	const JFloat xmin,
	const JFloat xmax,
	const JFloat ymin,
	const JFloat ymax
	)
	:
	PlotFitBase(plot, fitData, xmin, xmax, ymin, ymax)
{
	JPlotFitQuad2X();
}

void
PlotFitQuad2::JPlotFitQuad2X()
{
	SetParameterCount(3);
	SetHasGoodnessOfFit(true);
	itsFunctionName = "y = a + bx + cx^2";
	CalculateFirstPass();
	JVector p(3);
	p.SetItem(1, itsAParameter);
	p.SetItem(2, itsBParameter);
	p.SetItem(3, itsCParameter);
	GenerateFit(p, itsChi2Start);
}


/*********************************************************************************
 Destructor

 ********************************************************************************/

PlotFitQuad2::~PlotFitQuad2()
{
}

/*********************************************************************************
 GetYValue

 ********************************************************************************/

bool
PlotFitQuad2::GetYValue
	(
	const JFloat	x,
	JFloat*			y
	)
	const
{
	*y = itsAParameter + itsBParameter*x + itsCParameter*x*x;
	return true;
}


/*********************************************************************************
 GetParameterName

 ********************************************************************************/

bool
PlotFitQuad2::GetParameterName
	(
	const JIndex index,
	JString* name
	)
	const
{
	if ((index > 3) || (index < 1))
	{
		return false;
	}
	if (index == 1)
	{
		*name = "a";
	}
	else if (index == 2)
	{
		*name = "b";
	}
	else if (index == 3)
	{
		*name = "c";
	}
	return true;
}

/*********************************************************************************
 GetParameter

 ********************************************************************************/

bool
PlotFitQuad2::GetParameter
	(
	const JIndex index,
	JFloat* value
	)
	const
{
	if ((index > 3) || (index < 1))
	{
		return false;
	}
	if (index == 1)
	{
		*value = itsAParameter;
	}
	else if (index == 2)
	{
		*value = itsBParameter;
	}
	else if (index == 3)
	{
		*value = itsCParameter;
	}
	return true;
}

/*********************************************************************************
 GetParameterError

 ********************************************************************************/

bool
PlotFitQuad2::GetParameterError
	(
	const JIndex index,
	JFloat* value
	)
	const
{
	const J2DPlotDataBase* data = GetData();
	if (!data->HasXErrors() && !data->HasYErrors())
	{
		return false;
	}
	if (index == 1)
	{
		*value = itsAErrParameter;
	}
	else if (index == 2)
	{
		*value = itsBErrParameter;
	}
	else if (index == 3)
	{
		*value = itsCErrParameter;
	}
	return true;
}

/*********************************************************************************
 GetFunctionString

 ********************************************************************************/

JString
PlotFitQuad2::GetFunctionString()
	const
{
	return itsFunctionName;
}

/*********************************************************************************
 GetFitFunctionString

 ********************************************************************************/

JString
PlotFitQuad2::GetFitFunctionString()
	const
{
	return itsFunctionName;
}


/*********************************************************************************
 CalculateFirstPass

 ********************************************************************************/

void
PlotFitQuad2::CalculateFirstPass()
{
	JArray<JFloat> yAdjError;

	J2DDataPoint point;
	JSize rcount = GetRealElementCount();
	for (JIndex i=1; i<= rcount; i++)
	{
		J2DDataPoint point = GetRealElement(i);
		JFloat newVal = 1;
		if (point.yerr != 0)
		{
			newVal = point.yerr;
		}
		yAdjError.AppendItem(newVal);
	}

	JMatrix odata(rcount, 3, 1.0);
	JVector yData(rcount);
	for (JIndex i=1; i<= rcount; i++)
	{
		point = GetRealElement(i);
		JFloat yerr = yAdjError.GetItem(i);
		odata.SetItem(i, 1, 1/(yerr*yerr));
		odata.SetItem(i, 2, point.x/(yerr*yerr));
		odata.SetItem(i, 3, point.x*point.x/(yerr*yerr));
		yData.SetItem(i, point.y/(yerr*yerr));
	}
	JMatrix tData = odata.Transpose();
	JMatrix lData = tData * odata;
	JMatrix rData = tData * yData;
	JMatrix parms(3,1);
	JGaussianElimination(lData, rData, &parms);

	JFloat tempa, tempb, tempc, det;

	for (JIndex k=1; k<= 4; k++)
	{
		JFloat Y = 0, X = 0, X2 = 0, YX = 0, X3 = 0, YX2 = 0, X4 = 0, Sig = 0;
		for (JIndex i=1; i<= rcount; i++)
		{
			point = GetRealElement(i);
			JFloat yerr = yAdjError.GetItem(i);
			Y += point.y/(yerr*yerr);
			X += point.x/(yerr*yerr);
			X2 += point.x*point.x/(yerr*yerr);
			YX += point.y*point.x/(yerr*yerr);
			X3 += point.x*point.x*point.x/(yerr*yerr);
			YX2 += point.x*point.x*point.y/(yerr*yerr);
			X4 += point.x*point.x*point.x*point.x/(yerr*yerr);
			Sig += 1/(yerr*yerr);
		}
		JFloat cv1 = 0, cv2 = 0, cv3 = 0;
		for (JIndex i=1; i<= rcount; i++)
		{
			point = GetRealElement(i);
			JFloat syi = yAdjError.GetItem(i);
			JFloat yi = point.y;
			JFloat xi = point.x;
			for (JIndex j = 1; j <= rcount; j++)
			{
				point = GetRealElement(j);
				JFloat syj = yAdjError.GetItem(j);
				JFloat yj = point.y;
				JFloat xj = point.x;
				cv1 += xi*xj*xj*(xi*yj-yi*xj)/(syi*syi*syj*syj);
				cv2 += (xi*xj*xj*(yi - yj))/(syi*syi*syj*syj);
				cv3 += (xi*xj*xj*(xj - xi))/(syi*syi*syj*syj);
			}
		}
		det = Sig*(X2*X4-X3*X3) + X*(X3*X2-X*X4) + X2*(X*X3-X2*X2);
		tempa = (Y*(X2*X4-X3*X3) + X*(X3*YX2-YX*X4) + X2*(YX*X3-X2*YX2))/det;
		tempb = (Sig*(YX*X4-YX2*X3) + Y*(X3*X2-X*X4) + X2*(X*YX2-YX*X2))/det;
		tempc = (Sig*cv1 + X*cv2 + Y*cv3)/det;

		for (JIndex i=1; i<=rcount; i++)
		{
			J2DDataPoint point = GetRealElement(i);
			JFloat newVal =
				sqrt(point.yerr*point.yerr + (tempb+2*tempc*point.x)*(tempb+2*tempc*point.x)*point.xerr*point.xerr);
			if (newVal == 0)
			{
				newVal = 1;
			}
			yAdjError.SetItem(i, newVal);
		}
	}
//	itsAParameter	= tempa;
//	itsBParameter	= tempb;
//	itsCParameter	= tempc;
	itsAParameter	= parms.GetItem(1, 1);
	itsBParameter	= parms.GetItem(2, 1);
	itsCParameter	= parms.GetItem(3, 1);

	itsChi2Start = 0;
	for (JIndex i=1; i<= rcount; i++)
	{
		point = GetRealElement(i);
		JFloat yerr = yAdjError.GetItem(i);
		itsChi2Start += pow(point.y - tempa - tempb*point.x - tempc*point.x*point.x,2)/(yerr*yerr);
	}

	itsAErrParameter = 0;
	itsBErrParameter = 0;
	itsCErrParameter = 0;
}

/*********************************************************************************
 FunctionN

 ********************************************************************************/

JFloat
PlotFitQuad2::FunctionN
	(
	const JFloat x
	)
{
	return itsAParameter + itsBParameter*x + itsCParameter*x*x;
}

/*********************************************************************************
 FunctionNPrimed

 ********************************************************************************/

JFloat
PlotFitQuad2::FunctionNPrimed
	(
	const JFloat x
	)
{
	return 2*itsCParameter*x + itsBParameter;
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
PlotFitQuad2::SetCurrentParameters
	(
	const JVector& p
	)
{
	itsAParameter	= p.GetItem(1);
	itsBParameter	= p.GetItem(2);
	itsCParameter	= p.GetItem(3);
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
PlotFitQuad2::SetErrors
	(
	const JVector& p
	)
{
	itsAErrParameter	= p.GetItem(1);
	itsBErrParameter	= p.GetItem(2);
	itsCErrParameter	= p.GetItem(3);
}
