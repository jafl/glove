/******************************************************************************
 BuiltinFitDescription.cpp

	BASE CLASS = public FitDescription

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include <BuiltinFitDescription.h>
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
		GetVarList()->AddVariable(JString("a0", JString::kNoCopy), 0);
		GetVarList()->AddVariable(JString("a1", JString::kNoCopy), 0);
		SetFnName(JGetString("LinearName::BuiltinFitDescription"));
		SetFitFunctionString(JString("a0 + a1 * x", JString::kNoCopy));
	}
	else if (type == FitDescription::kBExp)
	{
		SetParameterCount(2);
		GetVarList()->AddVariable(JString("a", JString::kNoCopy), 0);
		GetVarList()->AddVariable(JString("b", JString::kNoCopy), 0);
		SetFnName(JGetString("ExponentialName::BuiltinFitDescription"));
		SetFitFunctionString(JString("a*e^(b*x)", JString::kNoCopy));
	}
	else if (type == FitDescription::kBPower)
	{
		SetParameterCount(2);
		GetVarList()->AddVariable(JString("a", JString::kNoCopy), 0);
		GetVarList()->AddVariable(JString("b", JString::kNoCopy), 0);
		SetFnName(JGetString("PowerLawName::BuiltinFitDescription"));
		SetFitFunctionString(JString("a*x^b", JString::kNoCopy));
	}

}

/******************************************************************************
 Destructor

 *****************************************************************************/

BuiltinFitDescription::~BuiltinFitDescription()
{
}
