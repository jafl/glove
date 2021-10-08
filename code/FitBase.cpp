/*********************************************************************************
 FitBase.cpp

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#include "FitBase.h"
#include "jx-af/jcore/jAssert.h"

/*********************************************************************************
 Constructor


 ********************************************************************************/

FitBase::FitBase()
{
	itsParameterCount = 0;
	itsHasParameterErrors = false;
	itsHasGOF = false;
}


FitBase::FitBase
	(
	const JSize paramCount,
	const bool errors,
	const bool gof
	)
	:
	itsParameterCount(paramCount),
	itsHasParameterErrors(errors),
	itsHasGOF(gof)
{
}

/*********************************************************************************
 Destructor


 ********************************************************************************/

FitBase::~FitBase()
{

}

/*********************************************************************************
 SetHasParameterErrors


 ********************************************************************************/

void
FitBase::SetHasParameterErrors
	(
	const bool errors
	)
{
	itsHasParameterErrors = errors;
}

/*********************************************************************************
 SetParameterCount


 ********************************************************************************/

void
FitBase::SetParameterCount
	(
	const JSize count
	)
{
	itsParameterCount = count;
}

/*********************************************************************************
 SetHasGoodnessOfFit


 ********************************************************************************/

void
FitBase::SetHasGoodnessOfFit
	(
	const bool gof
	)
{
	itsHasGOF = gof;

}
