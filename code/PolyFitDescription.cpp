/******************************************************************************
 PolyFitDescription.cpp

	BASE CLASS = public FitDescription

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include <PolyFitDescription.h>

#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentSetupVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

PolyFitDescription::PolyFitDescription
	(
	const JString&			name,
	const JArray<JIndex>&	powers
	)
	:
	FitDescription(kPolynomial, JString::empty, name)
{
	itsPowers	= jnew JArray<JIndex>(powers);
	assert(itsPowers != nullptr);

	PolyFitDescriptionX();
}

PolyFitDescription::PolyFitDescription
	(
	std::istream& is
	)
	:
	FitDescription(kPolynomial, JString::empty, JString::empty)
{
	itsPowers	= jnew JArray<JIndex>;
	assert(itsPowers != nullptr);

	JFileVersion version;
	is >> version;
	if (version > kCurrentSetupVersion)
	{
		return;
	}

	JSize count;
	is >> count;
	for (JIndex i = 1; i <= count; i++)
	{
		JIndex power;
		is >> power;
		itsPowers->AppendElement(power);
	}

	PolyFitDescriptionX();
}

void
PolyFitDescription::PolyFitDescriptionX()
{
	const JSize count	= itsPowers->GetElementCount();
	SetParameterCount(count);

	JString form;
	for (JIndex i = 1; i <= count; i++)
	{
		JString parm = "a" + JString((JUInt64) i - 1);
		GetVarList()->AddVariable(parm, 0);
		JString xTerm;
		JUInt64 power = itsPowers->GetElement(i);
		if (power > 0)
		{
			xTerm = " * x";
		}
		if (power > 1)
		{
			xTerm += "^" + JString(power);
		}
		form += parm + xTerm;
		if (i != count)
		{
			form += " + ";
		}
	}
	SetFitFunctionString(form);
}


/******************************************************************************
 Destructor

 *****************************************************************************/

PolyFitDescription::~PolyFitDescription()
{
	jdelete itsPowers;
}

/******************************************************************************
 GetPowers (public)

 ******************************************************************************/

void
PolyFitDescription::GetPowers
	(
	JArray<JIndex>* powers
	)
	const
{
	*powers	= *itsPowers;
}

/******************************************************************************
 WriteSetup (public)

 ******************************************************************************/

void
PolyFitDescription::WriteSetup
	(
	std::ostream& os
	)
{
	FitDescription::WriteSetup(os);

	os << ' ' << kCurrentSetupVersion << ' ' ;

	const JSize count	= itsPowers->GetElementCount();
	os << ' ' << count << ' ';
	for (JIndex i = 1; i <= count; i++)
	{
		os << ' ' << itsPowers->GetElement(i) << ' ';
	}

}
