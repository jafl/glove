/*********************************************************************************
 PlotFitPowerLaw.cpp

	PlotFitPowerLaw class.

	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "PlotFitPowerLaw.h"
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

PlotFitPowerLaw::PlotFitPowerLaw
	(
	J2DPlotWidget*	plot,
	J2DPlotDataBase*	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	PlotFitBase(plot, fitData, xMin, xMax)
{
	JPlotFitPowerLawX();
}

PlotFitPowerLaw::PlotFitPowerLaw
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
	JPlotFitPowerLawX();
}

void
PlotFitPowerLaw::JPlotFitPowerLawX()
{
	itsAParm	= 0;
	itsBParm	= 0;
	itsAErr	= 0;
	itsBErr	= 0;
	SetFunctionString(JString("a*x^b", JString::kNoCopy));
	SetParameterCount(2);
	SetHasGoodnessOfFit(true);
}


/*********************************************************************************
 Destructor

 ********************************************************************************/

PlotFitPowerLaw::~PlotFitPowerLaw()
{
}

/******************************************************************************
 GenerateFit (public)

 ******************************************************************************/

void
PlotFitPowerLaw::GenerateFit()
{
	CalculateFirstPass();
	JVector parms(2);
	parms.SetItem(1, itsAParm);
	parms.SetItem(2, itsBParm);
	PlotFitBase::GenerateFit(parms, itsChi2Start);
}

/*********************************************************************************
 GetYValue

 ********************************************************************************/

bool
PlotFitPowerLaw::GetYValue
	(
	const JFloat	x,
	JFloat*			y
	)
	const
{
	*y =  itsAParm * pow(x, itsBParm);
	return true;
}

/*********************************************************************************
 GetParameterName

 ********************************************************************************/

bool
PlotFitPowerLaw::GetParameterName
	(
	const JIndex index,
	JString* name
	)
	const
{
	if (index == 1)
	{
		*name = "a";
	}
	else if (index == 2)
	{
		*name = "b";
	}
	else
	{
		return false;
	}
	return true;
}

/*********************************************************************************
 GetParameter

 ********************************************************************************/

bool
PlotFitPowerLaw::GetParameter
	(
	const JIndex index,
	JFloat* value
	)
	const
{
	if (index == 1)
	{
		*value = itsAParm;
	}
	else if (index == 2)
	{
		*value = itsBParm;
	}
	else
	{
		return false;
	}
	return true;
}

/*********************************************************************************
 GetParameterError

 ********************************************************************************/

bool
PlotFitPowerLaw::GetParameterError
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
		*value = itsAErr;
	}
	else if (index == 2)
	{
		*value = itsBErr;
	}
	else
	{
		return false;
	}
	return true;
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
PlotFitPowerLaw::SetCurrentParameters
	(
	const JVector& p
	)
{
	itsAParm	= p.GetItem(1);
	itsBParm	= p.GetItem(2);
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
PlotFitPowerLaw::SetErrors
	(
	const JVector& p
	)
{
	itsAErr	= p.GetItem(1);
	itsBErr	= p.GetItem(2);
}

/*********************************************************************************
 FunctionN

 ********************************************************************************/

JFloat
PlotFitPowerLaw::FunctionN
	(
	const JFloat x
	)
{
	return itsAParm * pow(x, itsBParm);
}

/*********************************************************************************
 FunctionNPrimed

 ********************************************************************************/

JFloat
PlotFitPowerLaw::FunctionNPrimed
	(
	const JFloat x
	)
{
	return itsAParm * itsBParm * pow(x, itsBParm - 1);
}

/*********************************************************************************
 CalculateFirstPass

 ********************************************************************************/

void
PlotFitPowerLaw::CalculateFirstPass()
{
	J2DDataPoint point;
	const JSize count	= GetRealElementCount();
	JSize rcount		= 0;
	for (JIndex i = 1; i <= count; i++)
	{
		point	= GetRealElement(i);
		if (point.y > 0)
		{
			rcount++;
		}
	}
	JMatrix odata(rcount, 2, 1.0);
	JVector yData(rcount);

	rcount	= 0;
	for (JIndex i = 1; i <= count; i++)
	{
		point	= GetRealElement(i);
		if (point.y > 0)
		{
			rcount++;
			JFloat yerr = point.yerr;
			if (yerr == 0)
			{
				yerr = 1;
			}
			else
			{
				yerr	= log((point.y - point.yerr)/point.y);
			}
			odata.SetItem(rcount, 1, 1/(yerr*yerr));
			odata.SetItem(rcount, 2, log(point.x)/(yerr*yerr));
			yData.SetItem(rcount, log(point.x)/(yerr*yerr));
		}
	}
/*
	for (JIndex i = 1; i <= rcount; i++)
	{
		do
		{
			point = GetRealElement(i);
		}
		while (point.y == 0);

		JFloat yerr = point.yerr;
		if (yerr == 0)
		{
			yerr = 1;
		}
		else
		{
			yerr	= log((point.y - point.yerr)/point.y);
		}
		odata.SetItem(i, 1, 1/(yerr*yerr));
		odata.SetItem(i, 2, log(point.x)/(yerr*yerr));
		yData.SetItem(i, log(point.x)/(yerr*yerr));
	}
*/
	JMatrix tData = odata.Transpose();
	JMatrix lData = tData * odata;
	JMatrix rData = tData * yData;
	JMatrix parms(2,1);
	JGaussianElimination(lData, rData, &parms);
	JVector eparms(2);
	eparms.SetItem(1, exp(parms.GetItem(1,1)));
	eparms.SetItem(2, exp(parms.GetItem(2,1)));
	SetCurrentParameters(eparms);

	itsChi2Start = 0;
	for (JIndex i = 1; i <= count; i++)
	{
//		do
//			{
			point = GetRealElement(i);
//			}
//		while (point.y == 0);
		JFloat yerr = point.yerr;
		if (yerr == 0)
		{
			yerr = 1;
		}
		itsChi2Start += pow(point.y - FunctionN(point.x),2)/(yerr*yerr);
	}

}
