/******************************************************************************
 ModuleFitDescription.cpp

	BASE CLASS = public FitDescription

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include "ModuleFitDescription.h"
#include "DLFitModule.h"
#include <jx-af/jcore/jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

ModuleFitDescription::ModuleFitDescription
	(
	DLFitModule* fit
	)
	:
	FitDescription(FitDescription::kModule, fit->GetFunctionalForm(), fit->GetFitName()),
	itsModule(fit)
{
	const JSize count	= fit->GetParameterCount();
	SetParameterCount(count);
	for (JIndex i = 1; i <= count; i++)
	{
		GetVarList()->AddVariable(itsModule->GetParameterName(i), 0);
	}
	DoesRequireStartValues(true);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

ModuleFitDescription::~ModuleFitDescription()
{
}
