/******************************************************************************
 PlotFitNonLinear.cpp

	BASE CLASS = public PlotFitBase

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include "PlotFitNonLinear.h"
#include "VarList.h"
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/JVector.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

PlotFitNonLinear::PlotFitNonLinear
	(
	J2DPlotWidget*	plot,
	J2DPlotDataBase*	fitData,
	const JFloat	xMin,
	const JFloat	xMax
	)
	:
	PlotFitBase(plot, fitData, xMin, xMax),
	itsVarList(nullptr),
	itsFunction(nullptr),
	itsFPrimed(nullptr),
	itsErrors(nullptr)
{
}

PlotFitNonLinear::PlotFitNonLinear
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
	itsVarList(nullptr),
	itsFunction(nullptr),
	itsFPrimed(nullptr),
	itsErrors(nullptr)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

PlotFitNonLinear::~PlotFitNonLinear()
{
//	jdelete itsVarList;		we don't own it
	jdelete itsFunction;
	jdelete itsFPrimed;
	jdelete itsErrors;
}

/******************************************************************************
 SetCurrentParameters (virtual protected)

 ******************************************************************************/

void
PlotFitNonLinear::SetCurrentParameters
	(
	const JVector& p
	)
{
	assert(itsVarList != nullptr);

	const JSize count	= p.GetDimensionCount();
	for (JIndex i = 1; i <= count; i++)
	{
		itsVarList->SetValue(i + 1, p.GetItem(i));
	}
}

/******************************************************************************
 SetErrors (virtual protected)

 ******************************************************************************/

void
PlotFitNonLinear::SetErrors
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
PlotFitNonLinear::FunctionN
	(
	const JFloat x
	)
{
	assert(itsVarList != nullptr);
	assert(itsFunction != nullptr);
	itsVarList->SetValue(1, x);
	JFloat y = 0;
	itsFunction->Evaluate(&y);
	return y;
}

/******************************************************************************
 FunctionNPrimed (virtual protected)

 ******************************************************************************/

JFloat
PlotFitNonLinear::FunctionNPrimed
	(
	const JFloat x
	)
{
	if (itsFPrimed == nullptr)
	{
		return PlotFitBase::FunctionNPrimed(x);
	}
	assert(itsVarList != nullptr);
	assert(itsFunction != nullptr);
	itsVarList->SetValue(1, x);
	JFloat y = 0;
	itsFPrimed->Evaluate(&y);
	return y;
}

/******************************************************************************
 SetVarList (public)

 ******************************************************************************/

void
PlotFitNonLinear::SetVarList
	(
	VarList* list
	)
{
	itsVarList	= list;
	const JSize count	= list->GetVariableCount() - 1;
	SetParameterCount(count);
	itsErrors	= jnew JVector(count);
}

/******************************************************************************
 SetFunction (public)

 ******************************************************************************/

void
PlotFitNonLinear::SetFunction
	(
	const JString& function
	)
{
	assert(itsVarList != nullptr);
	SetFunctionString(function);

	JExprParser p(itsVarList);

	const bool ok = p.Parse(function, &itsFunction);
	assert(ok);
}

/******************************************************************************
 SetFPrimed (public)

 ******************************************************************************/

void
PlotFitNonLinear::SetFPrimed
	(
	const JString& fPrimed
	)
{
	assert(itsVarList != nullptr);

	JExprParser p(itsVarList);

	const bool ok = p.Parse(fPrimed, &itsFPrimed);
	assert(ok);
}

/******************************************************************************
 SetInitialParameters (public)

 ******************************************************************************/

void
PlotFitNonLinear::SetInitialParameters
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
PlotFitNonLinear::GetYValue
	(
	const JFloat	x,
	JFloat*			y
	)
	const
{
	PlotFitNonLinear* th = const_cast< PlotFitNonLinear* >(this);
	*y	= th->FunctionN(x);
	return true;
}


/*********************************************************************************
 GetParameterName

 ********************************************************************************/

bool
PlotFitNonLinear::GetParameterName
	(
	const JIndex index,
	JString* name
	)
	const
{
	*name = itsVarList->GetVariableName(index + 1);
	return true;
}

/*********************************************************************************
 GetParameter

 ********************************************************************************/

bool
PlotFitNonLinear::GetParameter
	(
	const JIndex index,
	JFloat* value
	)
	const
{
	bool ok	= itsVarList->GetNumericValue(index + 1, 1, value);
	return ok;
}

/*********************************************************************************
 GetParameterError

 ********************************************************************************/

bool
PlotFitNonLinear::GetParameterError
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
	*value	= itsErrors->GetItem(index);
	return true;
}
