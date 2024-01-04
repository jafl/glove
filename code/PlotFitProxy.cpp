/******************************************************************************
 PlotFitProxy.cpp

	BASE CLASS = class PlotFitFunction

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include "PlotFitProxy.h"
#include "VarList.h"
#include <jx-af/j2dplot/J2DPlotData.h>
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JFunction.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

PlotFitProxy::PlotFitProxy
	(
	PlotFitFunction*	fit,
	J2DPlotWidget*		plot,
	J2DPlotDataBase*		fitData
	)
	:
	PlotFitFunction(plot, fitData, 0, 0),
	itsErrors(nullptr),
	itsHasGOF(fit->HasParameterErrors()),
	itsFnString(fit->GetFitFunctionString())
{
	if (itsHasGOF)
	{
		fit->GetGoodnessOfFitName(&itsGOFName);
		fit->GetGoodnessOfFit(&itsGOF);
	}
	itsParms = jnew VarList();
	itsParms->AddVariable(JGetString("DefaultVarName::global"), 0);
	const JSize count	= fit->GetParameterCount();
	SetParameterCount(count);
	for (JIndex i = 1; i <= count; i++)
	{
		JString name;
		bool ok	= fit->GetParameterName(i, &name);
		assert(ok);
		JFloat value;
		ok	= fit->GetParameter(i, &value);
		assert(ok);
		itsParms->AddVariable(name, value);
	}

	if (fit->HasParameterErrors())
	{
		SetHasParameterErrors(true);
		itsErrors	= jnew JArray<JFloat>;
		assert(itsErrors != nullptr);
		for (JIndex i = 1; i <= count; i++)
		{
			JFloat value;
			bool ok	= fit->GetParameterError(i, &value);
			assert(ok);
			itsErrors->AppendItem(value);
		}
	}

	J2DPlotData* diff	= fit->GetDiffData();
	J2DPlotData* data;
	if (J2DPlotData::Create(&data, diff->GetXData(), diff->GetYData(), false))
	{
		const JArray<JFloat>* xerrors;
		if (diff->GetXPErrorData(&xerrors))
		{
			data->SetXErrors(*xerrors);
		}
		const JArray<JFloat>* yerrors;
		if (diff->GetYPErrorData(&yerrors))
		{
			data->SetYErrors(*yerrors);
		}
		SetDiffData(data);
	}
	JFloat xMin;
	JFloat xMax;
	fit->GetXRange(&xMin, &xMax);
	SetXRange(xMin, xMax);
	itsFn	= nullptr;

	JExprParser p(itsParms);

	const bool ok = p.Parse(itsFnString, &itsFn);
	assert( ok );

	SetHasGoodnessOfFit(itsHasGOF);
}

PlotFitProxy::PlotFitProxy
	(
	J2DPlotWidget*		plot,
	J2DPlotDataBase*		fitData,
	std::istream&			is
	)
	:
	PlotFitFunction(plot, fitData, 0, 0),
	itsErrors(nullptr)
{
	is >> JBoolFromString(itsHasGOF);
	if (itsHasGOF)
	{
		is >> itsGOFName;
		is >> itsGOF;
	}

	itsParms = jnew VarList();
	itsParms->AddVariable(JGetString("DefaultVarName::global"), 0);

	JSize count;
	is >> count;
	for (JIndex i = 1; i <= count; i++)
	{
		JString name;
		is >> name;
		JFloat value;
		is >> value;
		itsParms->AddVariable(name, value);
	}

	SetParameterCount(count);

	bool hasParameterErrors;
	is >> JBoolFromString(hasParameterErrors);

	if (hasParameterErrors)
	{
		SetHasParameterErrors(true);
		itsErrors = jnew JArray<JFloat>;
		assert(itsErrors != nullptr);
		for (JIndex i=1; i<=count; i++)
		{
			JFloat value;
			is >> value;
			itsErrors->AppendItem(value);
		}
	}

	JFloat xMin;
	JFloat xMax;

	is >> xMin;
	is >> xMax;

	SetXRange(xMin, xMax);

	is >> itsFnString;

	itsFn = nullptr;

	JExprParser p(itsParms);

	const bool ok = p.Parse(itsFnString, &itsFn);
	assert( ok );

	SetHasGoodnessOfFit(itsHasGOF);
	GenerateDiffData();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

PlotFitProxy::~PlotFitProxy()
{
	jdelete itsParms;
	jdelete itsErrors;
}

/******************************************************************************
 WriteData

 *****************************************************************************/

void
PlotFitProxy::WriteData
	(
	std::ostream& os
	)
{
	os << JBoolToString(itsHasGOF) << ' ';
	if (itsHasGOF)
	{
		os << itsGOFName << ' ';
		os << itsGOF << ' ';
	}

	JSize count	= itsParms->GetVariableCount();
	os << count - 1;

	for (JIndex i = 2; i <= count; i++) // don't count 'x'
	{
		JString name	= itsParms->GetVariableName(i);
		os << name << ' ';
		JFloat value;
		itsParms->GetNumericValue(i, 1, &value);
		os << value << ' ';
	}

	os << JBoolToString(HasParameterErrors());

	if (HasParameterErrors())
	{
		for (JIndex i = 1; i < count; i++)
		{
			os << itsErrors->GetItem(i) << ' ';
		}
	}

	JFloat xMin;
	JFloat xMax;

	GetXRange(&xMin, &xMax);

	os << xMin << ' ';
	os << xMax << ' ';

	os << itsFnString << ' ';
}

/******************************************************************************
 GetParameterName (public)

 ******************************************************************************/

bool
PlotFitProxy::GetParameterName
	(
	const JIndex	index,
	JString*		name
	)
	const
{
	if (index >= itsParms->GetVariableCount())
	{
		return false;
	}
	*name	= itsParms->GetVariableName(index + 1);
	return true;
}

/******************************************************************************
 GetParameter (public)

 ******************************************************************************/

bool
PlotFitProxy::GetParameter
	(
	const JIndex	index,
	JFloat*		value
	)
	const
{
	return itsParms->GetNumericValue(index + 1, 1, value);
}

/******************************************************************************
 GetParameterError (public)

 ******************************************************************************/

bool
PlotFitProxy::GetParameterError
	(
	const JIndex	index,
	JFloat*			value
	)
	const
{
	if (!HasParameterErrors())
	{
		return false;
	}

	assert(itsErrors != nullptr);
	if (index > itsErrors->GetItemCount())
	{
		return false;
	}

	*value	= itsErrors->GetItem(index);
	return true;
}

/******************************************************************************
 GetGoodnessOfFitName (public)

 ******************************************************************************/

bool
PlotFitProxy::GetGoodnessOfFitName
	(
	JString* name
	)
	const
{
	if (!itsHasGOF)
	{
		return false;
	}
	*name	= itsGOFName;
	return true;
}

/******************************************************************************
 GetGoodnessOfFit (public)

 ******************************************************************************/

bool
PlotFitProxy::GetGoodnessOfFit
	(
	JFloat* value
	)
	const
{
	if (!itsHasGOF)
	{
		return false;
	}
	*value	= itsGOF;
	return true;
}

/******************************************************************************
 GetFitFunctionString (public)

 ******************************************************************************/

JString
PlotFitProxy::GetFitFunctionString()
	const
{
	return itsFnString;
}

/******************************************************************************
 GetFitFunctionString (public)

 ******************************************************************************/

JString
PlotFitProxy::GetFunctionString()
	const
{
	return itsFnString;
}

/******************************************************************************
 GetYValue (public)

 ******************************************************************************/

bool
PlotFitProxy::GetYValue
	(
	const JFloat	x,
	JFloat*		y
	)
	const
{
	itsParms->SetValue(1, x);
	return itsFn->Evaluate(y);
}
