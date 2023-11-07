/*********************************************************************************
 PlotFitFunction.cpp

	PlotFitFunction class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "PlotFitFunction.h"
#include <jx-af/j2dplot/J2DPlotWidget.h>
#include <jx-af/j2dplot/J2DPlotDataBase.h>
#include <jx-af/j2dplot/J2DPlotData.h>
#include <jx-af/j2dplot/J2DDataPoint.h>
#include <jx-af/jcore/jAssert.h>

/*********************************************************************************
 Constructor

 ********************************************************************************/

PlotFitFunction::PlotFitFunction
	(
	J2DPlotWidget*		plot,
	J2DPlotDataBase*	fitData,
	const JFloat		xMin,
	const JFloat		xMax
	)
	:
	J2DPlotFunctionBase(J2DPlotDataBase::kScatterPlot, plot, xMin, xMax),
	FitBase(),
	itsDiffData(nullptr)
{
	itsData = fitData;
	itsHasXErrors = false;
	itsHasYErrors = false;
	if (itsData->HasXErrors())
	{
		itsHasXErrors = true;
	}
	if (itsData->HasYErrors())
	{
		itsHasYErrors = true;
	}
}

/*********************************************************************************
 Destructor

 ********************************************************************************/

PlotFitFunction::~PlotFitFunction()
{
}

/*********************************************************************************
 GenerateDiffData (protected)

 ********************************************************************************/

void
PlotFitFunction::GenerateDiffData()
{
	JArray<JFloat> xdata;
	JArray<JFloat> ydata;
	JArray<JFloat> xerrdata;
	JArray<JFloat> yerrdata;
	if (!itsData->HasYErrors())
	{
		CalculateStdDev();
	}
	const JSize count = itsData->GetItemCount();
	J2DDataPoint data;
	for (JSize i = 1; i <= count; i++)
	{
		if (GetDataElement(i, &data))
		{
			JFloat fitY;
			GetYValue(data.x, &fitY);
			xdata.AppendItem(data.x);
			ydata.AppendItem(data.y - fitY);
			if (itsHasYErrors)
			{
				// Save the following for when it gets merged into PlotFitBase
//				if (itsHasXErrors)
//					{
//					JFloat b	= FunctionNPrimed(data.x);
//					JFloat err	= sqrt(data.yerr * data.yerr + b * b * data.xerr * data.xerr);
//					yerrdata->AppendItem(err);
//					}
//				else
//					{
					yerrdata.AppendItem(data.yerr);
//					}
			}
			else
			{
				yerrdata.AppendItem(itsStdDev);
			}
			if (itsHasXErrors)
			{
				xerrdata.AppendItem(data.xerr);
			}
		}
	}
	J2DPlotData::Create(&itsDiffData, xdata, ydata, false);
	itsDiffData->SetYErrors(yerrdata);
	if (itsHasXErrors)
	{
		itsDiffData->SetXErrors(xerrdata);
	}
}

/*********************************************************************************
 GetDiffData

 ********************************************************************************/

J2DPlotData*
PlotFitFunction::GetDiffData()
	const
{
	assert(itsDiffData != nullptr);
	return itsDiffData;
}

/******************************************************************************
 SetDiffData (protected)

 ******************************************************************************/

void
PlotFitFunction::SetDiffData
	(
	J2DPlotData* data
	)
{
	if (itsDiffData != nullptr)
	{
		jdelete itsDiffData;
	}
	itsDiffData	= data;
}


/*********************************************************************************
 CalculateStdDev (protected)

 ********************************************************************************/

void
PlotFitFunction::CalculateStdDev()
{
	JSize count = itsData->GetItemCount();
	J2DDataPoint data;
	JFloat current = 0;
	for (JSize i = 1; i <= count; i++)
	{
		itsData->GetItem(i, &data);
		JFloat fitY;
		GetYValue(data.x, &fitY);
		current += (fitY - data.y)*(fitY - data.y);
	}
	itsStdDev = sqrt(current/(count - 2));
}

/*********************************************************************************
 AdjustDiffDataValue (protected)

 ********************************************************************************/

void
PlotFitFunction::AdjustDiffDataValue
	(
	const JIndex index,
	const JFloat value
	)
{
}

/*********************************************************************************
 DataElementValid

 ********************************************************************************/

bool
PlotFitFunction::DataElementValid
	(
	const JIndex index
	)
{
	return true;
}

/******************************************************************************
 GetDataElement

 *****************************************************************************/

bool
PlotFitFunction::GetDataElement
	(
	const JIndex index,
	J2DDataPoint* point
	)
{
	bool valid = DataElementValid(index);
	if (!valid)
	{
		return false;
	}
	const J2DPlotDataBase* data = GetData();
	data->GetItem(index, point);
	return true;
}
