/******************************************************************************
 BuiltinFitDescription.cpp

	BASE CLASS = public FitDescription

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include "BuiltinFitDescription.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

BuiltinFitDescription::BuiltinFitDescription
	(
	const FitType type
	)
	:
	FitDescription(type)
{
	if (type == FitDescription::kBLinear)
	{
		SetParameterCount(2);
		GetVarList()->AddVariable("a0", 0);
		GetVarList()->AddVariable("a1", 0);
		SetFnName(JGetString("LinearName::BuiltinFitDescription"));
		SetFitFunctionString("a0 + a1 * x");
	}
	else if (type == FitDescription::kBExp)
	{
		SetParameterCount(2);
		GetVarList()->AddVariable("a", 0);
		GetVarList()->AddVariable("b", 0);
		SetFnName(JGetString("ExponentialName::BuiltinFitDescription"));
		SetFitFunctionString("a*e^(b*x)");
	}
	else if (type == FitDescription::kBPower)
	{
		SetParameterCount(2);
		GetVarList()->AddVariable("a", 0);
		GetVarList()->AddVariable("b", 0);
		SetFnName(JGetString("PowerLawName::BuiltinFitDescription"));
		SetFitFunctionString("a*x^b");
	}

}

/******************************************************************************
 Destructor

 *****************************************************************************/

BuiltinFitDescription::~BuiltinFitDescription()
{
}
