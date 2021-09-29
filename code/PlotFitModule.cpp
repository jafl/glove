/******************************************************************************
 PlotFitModule.cpp

	BASE CLASS = public PlotFitBase

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include <PlotFitModule.h>
#include "DLFitModule.h"

#include <jx-af/jcore/JVector.h>

#include <jx-af/jcore/jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

PlotFitModule::PlotFitModule
	(
	J2DPlotWidget* 	plot, 
	J2DPlotDataBase* 	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	PlotFitBase(plot, fitData, xMin, xMax),
	itsModule(nullptr),
	itsErrors(nullptr)
{
}

PlotFitModule::PlotFitModule
	(
	J2DPlotWidget* plot, 
	J2DPlotDataBase* fitData,
	const JFloat xmin, 
	const JFloat xmax,
	const JFloat ymin, 
	const JFloat ymax
	)
	:
	PlotFitBase(plot, fitData, xmin, xmax, ymin, ymax),
	itsModule(nullptr),
	itsErrors(nullptr)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

PlotFitModule::~PlotFitModule()
{
//	jdelete itsModule; 		we don't own it
	jdelete itsErrors;
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
PlotFitModule::SetCurrentParameters
	(
	const JVector& p
	)
{
	assert(itsModule != nullptr);

	itsModule->SetCurrentParameters(p);
	*itsParameters	= p;
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
PlotFitModule::SetErrors
	(
	const JVector& p
	)
{
	*itsErrors	= p;
}

/******************************************************************************
 FunctionN (virtual protected)

 ******************************************************************************/

JFloat
PlotFitModule::FunctionN
	(
	const JFloat x
	)
{
	assert(itsModule != nullptr);
	return itsModule->Function(x);
}

/******************************************************************************
 FunctionN (virtual protected)

 ******************************************************************************/

JFloat
PlotFitModule::FunctionNPrimed
	(
	const JFloat x
	)
{
	assert(itsModule != nullptr);
	if (!itsModule->HasFPrimed())
	{
		return PlotFitBase::FunctionNPrimed(x);
	}
	return itsModule->FPrimed(x);
}

/******************************************************************************
 SetFitModule (public)

 ******************************************************************************/

void
PlotFitModule::SetFitModule
	(
	DLFitModule* fit
	)
{
	itsModule	= fit;
	const JSize count	= itsModule->GetParameterCount();
	itsParameters		= jnew JVector(count);
	assert(itsParameters != nullptr);
	SetParameterCount(count);
	itsErrors	= jnew JVector(count);
	assert(itsErrors != nullptr);
	SetFunctionString(itsModule->GetFunctionalForm());
}

/******************************************************************************
 SetInitialParameters (public)

 ******************************************************************************/

void
PlotFitModule::SetInitialParameters
	(
	const JVector& p
	)
{
	GenerateFit(p, 0);
}

/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

bool
PlotFitModule::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	PlotFitModule* th	= const_cast< PlotFitModule* >(this);
	*y	= th->itsModule->Function(x);
	return true;
}


/*********************************************************************************
 GetParameterName
 

 ********************************************************************************/

bool
PlotFitModule::GetParameterName
	(
	const JIndex index, 
	JString* name
	)
	const
{
	assert(itsModule != nullptr);
	if (index > itsModule->GetParameterCount() || index < 1)
	{
		return false;
	}
	*name	= itsModule->GetParameterName(index);
	return true;
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

bool
PlotFitModule::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	if (index > itsParameters->GetDimensionCount() || index < 1)
	{
		return false;
	}
	*value	= itsParameters->GetElement(index);
	return true;
}

/*********************************************************************************
 GetParameterError
 

 ********************************************************************************/

bool
PlotFitModule::GetParameterError
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
	if (index > itsErrors->GetDimensionCount())
	{
		return false;
	}
	*value	= itsErrors->GetElement(index);
	return true;		
}
