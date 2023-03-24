/*********************************************************************************
 PlotFitExp.cpp

	PlotFitExp class.

	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "PlotFitExp.h"
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

PlotFitExp::PlotFitExp
	(
	J2DPlotWidget*	plot,
	J2DPlotDataBase*	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	PlotFitBase(plot, fitData, xMin, xMax)
{
	JPlotFitExpX(plot, fitData);
}

PlotFitExp::PlotFitExp
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
	JPlotFitExpX(plot, fitData);
}

void
PlotFitExp::JPlotFitExpX
	(
	J2DPlotWidget* plot,
	J2DPlotDataBase* fitData
	)
{
	itsAParm	= 0;
	itsBParm	= 0;
	itsAErr	= 0;
	itsBErr	= 0;
	SetFunctionString(JString("a*e^(b*x)", JString::kNoCopy));
	SetParameterCount(2);
	SetHasGoodnessOfFit(true);
}


/*********************************************************************************
 Destructor


 ********************************************************************************/

PlotFitExp::~PlotFitExp()
{
}

/******************************************************************************
 GenerateFit (public)

 ******************************************************************************/

void
PlotFitExp::GenerateFit()
{
	CalculateFirstPass();
	JVector parms(2);
	parms.SetElement(1, itsAParm);
	parms.SetElement(2, itsBParm);
	PlotFitBase::GenerateFit(parms, itsChi2Start);
}


/*********************************************************************************
 GetYValue


 ********************************************************************************/

bool
PlotFitExp::GetYValue
	(
	const JFloat	x,
	JFloat*			y
	)
	const
{
	*y =  itsAParm * exp(itsBParm * x);
	return true;
}

/*********************************************************************************
 GetParameterName


 ********************************************************************************/

bool
PlotFitExp::GetParameterName
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
PlotFitExp::GetParameter
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
PlotFitExp::GetParameterError
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
PlotFitExp::SetCurrentParameters
	(
	const JVector& p
	)
{
	itsAParm	= p.GetElement(1);
	itsBParm	= p.GetElement(2);
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
PlotFitExp::SetErrors
	(
	const JVector& p
	)
{
	itsAErr	= p.GetElement(1);
	itsBErr	= p.GetElement(2);
}

/*********************************************************************************
 FunctionN

 ********************************************************************************/

JFloat
PlotFitExp::FunctionN
	(
	const JFloat x
	)
{
	return itsAParm * exp(itsBParm * x);
}

/*********************************************************************************
 FunctionNPrimed

 ********************************************************************************/

JFloat
PlotFitExp::FunctionNPrimed
	(
	const JFloat x
	)
{
	return itsAParm * itsBParm * exp(itsBParm * x);
}

/*********************************************************************************
 CalculateFirstPass


 ********************************************************************************/

void
PlotFitExp::CalculateFirstPass()
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
			odata.SetElement(rcount, 1, 1/(yerr*yerr));
			odata.SetElement(rcount, 2, log(point.x)/(yerr*yerr));
			yData.SetElement(rcount, log(point.x)/(yerr*yerr));
		}
	}

	JMatrix tData = odata.Transpose();
	JMatrix lData = tData * odata;
	JMatrix rData = tData * yData;
	JMatrix parms(2,1);
	JGaussianElimination(lData, rData, &parms);
	JVector eparms(2);
	eparms.SetElement(1, exp(parms.GetElement(1,1)));
	eparms.SetElement(2, parms.GetElement(2,1));
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
