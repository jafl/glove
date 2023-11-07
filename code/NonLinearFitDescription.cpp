/******************************************************************************
 NonLinearFitDescription.cpp

	BASE CLASS = public FitDescription

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include <NonLinearFitDescription.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentSetupVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

NonLinearFitDescription::NonLinearFitDescription
	(
	const JString&				name,
	const JString&				function,
	const JString&				fPrimed,
	const JPtrArray<JString>&	vars
	)
	:
	FitDescription(FitDescription::kNonLinear, function, name),
	itsFunction(function),
	itsFPrimed(fPrimed)
{
	DoesRequireStartValues(true);

	JSize count		= vars.GetItemCount();
	JIndex offset	= 0;
	if (count > 1 && *(vars.GetItem(1)) == JGetString("DefaultVarName::global"))
	{
		offset = 1;
		count--;
	}
	SetParameterCount(count);
	for (JIndex i = 1; i <= count; i++)
	{
		JString var	= *(vars.GetItem(i + offset));
		GetVarList()->AddVariable(var, 0);
	}
}

NonLinearFitDescription::NonLinearFitDescription
	(
	std::istream& is
	)
	:
	FitDescription(FitDescription::kNonLinear, JString::empty, JString::empty)
{
	JFileVersion version;
	is >> version;
	if (version > kCurrentSetupVersion)
	{
		return;
	}
	is >> itsFunction;
	is >> itsFPrimed;

	DoesRequireStartValues(true);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

NonLinearFitDescription::~NonLinearFitDescription()
{
}

/******************************************************************************
 WriteSetup (virtual public)

 ******************************************************************************/

void
NonLinearFitDescription::WriteSetup
	(
	std::ostream& os
	)
{
	FitDescription::WriteSetup(os);

	os << ' ' << kCurrentSetupVersion << ' ' ;
	os << ' ' << itsFunction << ' ';
	os << ' ' << itsFPrimed << ' ';
}
