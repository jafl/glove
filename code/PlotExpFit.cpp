/*********************************************************************************
 PlotExpFit.cpp

	PlotExpFit class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "PlotExpFit.h"
#include <jx-af/j2dplot/J2DPlotWidget.h>
#include <jx-af/j2dplot/J2DPlotDataBase.h>
#include <jx-af/j2dplot/J2DPlotData.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jAssert.h>

/*********************************************************************************
 Constructor


 ********************************************************************************/

PlotExpFit::PlotExpFit
	(
	J2DPlotWidget*	plot,
	J2DPlotDataBase*	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	PlotLinearFit(plot, fitData, xMin, xMax, false)
{
	SetFunctionName(JString("y = a Exp(bx)", JString::kNoCopy));
	itsXData = jnew JArray<JFloat>;
	itsYData = jnew JArray<JFloat>;
	itsHasXErrors = false;
	itsHasYErrors = false;
	if (fitData->HasXErrors())
	{
		itsXErrData = jnew JArray<JFloat>;
		itsHasXErrors = true;
	}
	if (fitData->HasYErrors())
	{
		itsYErrData = jnew JArray<JFloat>;
		itsHasYErrors = true;
	}

	AdjustData();
	CreateData();
	GenerateFit();
	GenerateDiffData();
	AdjustDiffData();
}

/*********************************************************************************
 Destructor


 ********************************************************************************/

PlotExpFit::~PlotExpFit()
{
}

/*********************************************************************************
 GetYValue


 ********************************************************************************/

bool
PlotExpFit::GetYValue
	(
	const JFloat	x,
	JFloat*			y
	)
	const
{
	JFloat a, b;
	GetParameter(1, &a);
	GetParameter(2, &b);
	*y = a * exp(b*x);
	return true;
}


/*********************************************************************************
 GetYRange


 ********************************************************************************/

bool
PlotExpFit::GetYRange
	(
	const JFloat	xMin,
	const JFloat	xMax,
	const bool		xLinear,
	JFloat*			yMin,
	JFloat*			yMax
	)
	const
{
	JFloat y1, y2;
	GetYValue(xMin, &y1);
	GetYValue(xMax, &y2);

	*yMin = JMin(y1, y2);
	*yMax = JMax(y1, y2);
	return true;
}

/*********************************************************************************
 GetDataToFit


 ********************************************************************************/

const J2DPlotDataBase*
PlotExpFit::GetDataToFit()
	const
{
	return itsAdjustedData;
}

/*********************************************************************************
 AdjustData


 ********************************************************************************/

void
PlotExpFit::AdjustData()
{
	const J2DPlotDataBase* data = GetData();
	itsXData->RemoveAll();
	itsYData->RemoveAll();
	if (data->HasYErrors())
	{
		itsYErrData->RemoveAll();
	}
	if (data->HasXErrors())
	{
		itsXErrData->RemoveAll();
	}
	J2DDataPoint point;
	JSize count = data->GetElementCount();
	JSize i;
	for (i = 1; i <= count; i++)
	{
		data->GetElement(i, &point);
		if (point.y > 0)
		{
			itsXData->AppendElement(point.x);
			itsYData->AppendElement(log(point.y));
			if (itsHasYErrors)
			{
				itsYErrData->AppendElement(log((point.y - point.yerr)/point.y));
			}
			if (itsHasXErrors)
			{
				itsXErrData->AppendElement(point.xerr);
			}
		}
	}
}

/*********************************************************************************
 CreateData


 ********************************************************************************/

void
PlotExpFit::CreateData()
{
	J2DPlotData::Create(&itsAdjustedData, *itsXData, *itsYData, true);
	if (itsHasYErrors)
	{
		itsAdjustedData->SetYErrors(*itsYErrData);
	}
	if (itsHasXErrors)
	{
		itsAdjustedData->SetXErrors(*itsXErrData);
	}
}

/*********************************************************************************
 GetParameter


 ********************************************************************************/

bool
PlotExpFit::GetParameter
	(
	const JIndex index,
	JFloat* value
	)
	const
{
	JFloat linVal;
	bool success = PlotLinearFit::GetParameter(index, &linVal);
	if (index == 1)
	{
		*value = exp(linVal);
	}
	else
	{
		*value = linVal;
	}
	return success;
}

/*********************************************************************************
 GetParameterError


 ********************************************************************************/

bool
PlotExpFit::GetParameterError
	(
	const JIndex index,
	JFloat* value
	)
	const
{
	JFloat linVal;
	JFloat aVal;
	bool success = PlotLinearFit::GetParameterError(index, &linVal);
	PlotLinearFit::GetParameter(index, &aVal);
	if (index == 1)
	{
		*value = exp(aVal) - exp(aVal - linVal);
	}
	else
	{
		*value = linVal;
	}
	return success;
}

/*********************************************************************************
 AdjustDiffData


 ********************************************************************************/

void
PlotExpFit::AdjustDiffData()
{
	JFloat B;
	GetParameter(2, &B);
	J2DPlotDataBase* pwd = GetDiffData();
	J2DDataPoint dataD;
	J2DDataPoint data;
	const JSize count = pwd->GetElementCount();
	for (JSize i = 1; i <= count; i++)
	{
		pwd->GetElement(i, &dataD);
		GetData()->GetElement(i, &data);
		JFloat yerr;
		if (GetData()->HasYErrors() || itsHasXErrors)
		{
			yerr = sqrt(data.yerr*data.yerr + B*B*data.y*data.y*data.xerr*data.xerr);
			AdjustDiffDataValue(i, yerr);
		}
	}
}
