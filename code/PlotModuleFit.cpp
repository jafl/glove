/*********************************************************************************
 PlotModuleFit.cpp
 
	PlotModuleFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "PlotModuleFit.h"
#include "jx-af/j2dplot/J2DPlotWidget.h"
#include "jx-af/j2dplot/J2DPlotDataBase.h"
#include "VarList.h"
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jglobals.h>
#include <jx-af/jcore/jAssert.h>

/*********************************************************************************
 Constructor 
 

 ********************************************************************************/

PlotModuleFit::PlotModuleFit
	(
	J2DPlotWidget* 		plot, 
	J2DPlotDataBase* 		fitData,
	const JFloat		xMin,
	const JFloat		xMax,
	JPtrArray<JString>*	names, 
	JArray<JFloat>* 	values,
	JFunction*			function,
	VarList* 			list, 
	const JSize 		parmscount,	
	const bool 		errors, 
	const bool 		gof
	)
	:
	PlotFitFunction(plot, fitData, xMin, xMax)
{
	itsUsingRange = false;
	JPlotModuleFitX(plot, fitData, names, values, function, list, parmscount, errors, gof);
}

PlotModuleFit::PlotModuleFit
	(
	J2DPlotWidget* 		plot, 
	J2DPlotDataBase* 		fitData,
	JPtrArray<JString>*	names, 
	JArray<JFloat>* 	values,
	JFunction*			function,
	VarList* 			list,
	const JFloat 		xmin, 
	const JFloat 		xmax,
	const JFloat 		ymin, 
	const JFloat 		ymax,
	const JSize 		parmscount,	
	const bool 		errors, 
	const bool 		gof
	)
	:
	PlotFitFunction(plot, fitData, xmin, xmax)
{
	if (xmax > xmin)
	{
		itsRangeXMax = xmax;
		itsRangeXMin = xmin;
	}
	else
	{
		itsRangeXMax = xmin;
		itsRangeXMin = xmax;
	}
	if (ymax > ymin)
	{
		itsRangeYMax = ymax;
		itsRangeYMin = ymin;
	}
	else
	{
		itsRangeYMax = ymin;
		itsRangeYMin = ymax;
	}
	itsUsingRange = true;
	JPlotModuleFitX(plot, fitData, names, values, function, list, parmscount, errors, gof);
}

PlotModuleFit::PlotModuleFit
	(
	J2DPlotWidget* 	plot, 
	J2DPlotDataBase* 	fitData,
	std::istream& 	is
	)
	:
	PlotFitFunction(plot, fitData, 0, 10)
{
	JPtrArray<JString>* names = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	JArray<JFloat>* values = jnew JArray<JFloat>;
	int count;
	is >> count;
	for (int i = 1; i <= count; i++)
	{
		JString* name = jnew JString();
		JFloat value;
		is >> *name;
		names->Append(name);
		is >> value;
		values->AppendElement(value);
	}
	int parmscount;
	is >> parmscount;
	bool errors;
	is >> JBoolFromString(errors);
	bool gof;
	is >> JBoolFromString(gof);
	JString fstring;
	is >> fstring;
	VarList* list = jnew VarList;
	list->AddVariable(JGetString("DefaultVarName::global"), 0);
	for (int i = 1; i <= parmscount; i++)
	{
		JSize index = i;
		if (errors)
		{
			index = i * 2 - 1;
		}
		JString parm(*(names->GetElement(index)));
		JFloat value = values->GetElement(index);
		list->AddVariable(parm, value);
	}

	JExprParser p(list);

	JFunction* f;
	const bool ok = p.Parse(fstring, &f);
	assert( ok );

	JPlotModuleFitX(plot, fitData, names, values, f, list, parmscount, errors, gof);	
}

void
PlotModuleFit::JPlotModuleFitX
	(
	J2DPlotWidget* 		plot, 
	J2DPlotDataBase* 		fitData,
	JPtrArray<JString>*	names, 
	JArray<JFloat>* 	values,
	JFunction*			function,
	VarList* 			list,
	const JSize 		parmscount,	
	const bool 		errors, 
	const bool 		gof
	)
{
	SetHasParameterErrors(errors);
	SetParameterCount(parmscount);
	SetHasGoodnessOfFit(gof);
	itsNames = names;
	itsValues = values;
	itsFunction = function;
	itsList = list;
	itsFunctionName = function->Print();
	GenerateDiffData();
}


/*********************************************************************************
 Destructor
 

 ********************************************************************************/
 
PlotModuleFit::~PlotModuleFit()
{
	itsNames->DeleteAll();
	jdelete itsNames;
	jdelete itsValues;
	jdelete itsFunction;
	jdelete itsList;
}

/*********************************************************************************
 GetElement
 

 ********************************************************************************/

void
PlotModuleFit::GetElement
	(
	const JIndex index, 
	J2DDataPoint* data
	)
	const
{
	JFloat range = itsCurrentXMax - itsCurrentXMin;
	JFloat x = itsCurrentXMin + (index-1)*range/itsCurrentStepCount;
	JFloat y;
	itsList->SetValue(1, x);
	itsFunction->Evaluate(&y);
	data->x = x;
	data->y = y;
}

/*********************************************************************************
 GetYValue
 

 ********************************************************************************/

bool
PlotModuleFit::GetYValue
	(
	const JFloat 	x,
	JFloat*			y
	)
	const
{
	itsList->SetValue(1, x);
	itsFunction->Evaluate(y);
	return true;
}

/*********************************************************************************
 UpdateFunction
 

 ********************************************************************************/

void
PlotModuleFit::UpdateFunction
	(
	const JFloat xmin, 
	const JFloat xmax, 
	const JSize steps
	)
{
	itsCurrentXMin = xmin;
	itsCurrentXMax = xmax;
	itsCurrentStepCount = steps;
	SetElementCount(steps+1);
}

/*********************************************************************************
 GetParameterName
 

 ********************************************************************************/

bool
PlotModuleFit::GetParameterName
	(
	const JIndex index, 
	JString* name
	)
	const
{
	if ((index > GetParameterCount()) || (index < 1))
	{
		return false;
	}
	
	*name = itsList->GetVariableName(index + 1);
	return true;
}

/*********************************************************************************
 GetParameter
 

 ********************************************************************************/

bool
PlotModuleFit::GetParameter
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	if ((index > GetParameterCount()) || (index < 1))
	{
		return false;
	}
	return itsList->GetNumericValue(index + 1, 1, value);
}

/*********************************************************************************
 GetParameterError
 

 ********************************************************************************/

bool
PlotModuleFit::GetParameterError
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	const J2DPlotDataBase* data = GetDataToFit();
	if (!data->HasXErrors() && !data->HasYErrors())
	{
		return false;
	}
	if (!HasParameterErrors())
	{
		return false;
	}
	JIndex arrayIndex = index * 2;
	*value = itsValues->GetElement(arrayIndex);
	return true;
}

/*********************************************************************************
 GetGoodnessOfFitName
 

 ********************************************************************************/

bool
PlotModuleFit::GetGoodnessOfFitName
	(
	JString* name
	)
	const
{
	if (!HasGoodnessOfFit())
	{
		return false;
	}
	JIndex arrayIndex = itsNames->GetElementCount();
	*name = *(itsNames->GetElement(arrayIndex));
	return true;
}

/*********************************************************************************
 GetGoodnessOfFit
 

 ********************************************************************************/

bool
PlotModuleFit::GetGoodnessOfFit
	(
	JFloat* value
	)
	const
{
	if (!HasGoodnessOfFit())
	{
		return false;
	}
	JIndex arrayIndex = itsValues->GetElementCount();
	*value = itsValues->GetElement(arrayIndex);
	
	return true;
}

/*********************************************************************************
 GetFunctionString
 

 ********************************************************************************/

JString
PlotModuleFit::GetFunctionString()
	const
{
	return itsFunctionName;
}

/*********************************************************************************
 GetFitFunctionString
 

 ********************************************************************************/

JString
PlotModuleFit::GetFitFunctionString()
	const
{
	return itsFunctionName;
}

/*********************************************************************************
 DataElementValid
 

 ********************************************************************************/

bool
PlotModuleFit::DataElementValid
	(
	const JIndex index
	)
{
	const J2DPlotDataBase* data = GetDataToFit();
	J2DDataPoint point;
	data->GetElement(index, &point);
	
	if (itsUsingRange)
	{		
		if ((point.x >= itsRangeXMin) &&
			(point.x <= itsRangeXMax) &&
			(point.y >= itsRangeYMin) &&
			(point.y <= itsRangeYMax))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}

/******************************************************************************
 GetDataElement
 

 *****************************************************************************/

bool
PlotModuleFit::GetDataElement
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
	const J2DPlotDataBase* data = GetDataToFit();
	data->GetElement(index, point);
	return true;
}

/*********************************************************************************
 WriteData
 

 ********************************************************************************/

void
PlotModuleFit::WriteData
	(
	std::ostream& os
	)
{
	JSize count = itsNames->GetElementCount();
	os << count << " ";
	for (JSize i = 1; i <= count; i++)
	{
		os << *(itsNames->GetElement(i)) << " ";
		os << itsValues->GetElement(i) << " ";
	}
	os << GetParameterCount() << " ";
	os << JBoolToString(HasParameterErrors())
	   << JBoolToString(HasGoodnessOfFit()) << " ";
	os << GetFunctionString() << " ";
}
