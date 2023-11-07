/******************************************************************************
 FitDescription.cpp

	BASE CLASS = <NONE>

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include "FitDescription.h"
#include "PolyFitDescription.h"
#include "NonLinearFitDescription.h"

#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentSetupVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

FitDescription::FitDescription
	(
	const FitType	type,
	const JString&	form,
	const JString&	name
	)
	:
	FitBase(1),
	itsType(type),
	itsFnForm(form),
	itsFnName(name.IsEmpty() ? JGetString("DefaultLabel::global") : name),
	itsRequiresStartValues(false),
	itsCanUseStartValues(true)
{
	itsVarList	= jnew VarList();

	itsVarList->AddVariable(JGetString("DefaultVarName::global"), 0);
}

bool
FitDescription::Create
	(
	std::istream&		is,
	FitDescription**	fd
	)
{
	JFileVersion version;
	is >> version;
	if (version > kCurrentSetupVersion)
	{
		return false;
	}

	int type;
	is >> type;
	JString form;
	is >> form;
	JString name;
	is >> name;
	JSize count;
	is >> count;
	JPtrArray<JString> vars(JPtrArrayT::kDeleteAll);
	for (JIndex i = 1; i <= count; i++)
	{
		JString* var = jnew JString();
		is >> *var;
		vars.Append(var);
	}

	if (type == kPolynomial)
	{
		PolyFitDescription* pfd	= jnew PolyFitDescription(is);
		*fd	= pfd;
	}
	else if (type == kNonLinear)
	{
		NonLinearFitDescription* nfd	= jnew NonLinearFitDescription(is);
		*fd	= nfd;
		for (JIndex i = 1; i <= count; i++)
		{
			(*fd)->itsVarList->AddVariable(*(vars.GetItem(i)), 0);
		}
		(*fd)->SetParameterCount(count);
	}
	else
	{
		return false;
	}

	vars.DeleteAll();
	(*fd)->itsFnName	= name;
	(*fd)->itsFnForm	= form;

	return true;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

FitDescription::~FitDescription()
{
	jdelete itsVarList;
}

/******************************************************************************
 GetParameterName (virtual public)

 ******************************************************************************/

bool
FitDescription::GetParameterName
	(
	const JIndex	index,
	JString*		name
	)
	const
{
	*name	= itsVarList->GetVariableName(index + 1);
	return true;
}

/******************************************************************************
 GetParameter (virtual public)

 ******************************************************************************/

bool
FitDescription::GetParameter
	(
	const JIndex	index,
	JFloat*			value
	)
	const
{
	return false;
}

/******************************************************************************
 WriteSetup (virtual public)

 ******************************************************************************/

void
FitDescription::WriteSetup
	(
	std::ostream& os
	)
{
	os << ' ' << kCurrentSetupVersion << ' ';
	os << ' ' << (int)itsType << ' ';
	os << ' ' << itsFnForm << ' ';
	os << ' ' << itsFnName << ' ';

	const JSize count	= itsVarList->GetVariableCount() - 1;
	os << ' ' << count << ' ';
	for (JIndex i = 1; i <= count; i++)
	{
		os << ' ' << itsVarList->GetVariableName(i + 1) << ' ';
	}
}

/******************************************************************************
 CompareFontNames (static private)

	We sort by the first number, then by the second number: #x##

 ******************************************************************************/

std::weak_ordering
FitDescription::CompareFits
	(
	FitDescription* const & f1,
	FitDescription* const & f2
	)
{
	if (f1->GetType() == f2->GetType())
	{
		return JCompareStringsCaseInsensitive(const_cast<JString*>(&(f1->GetFnName())), const_cast<JString*>(&(f2->GetFnName())));
	}
	else if (f1->GetType() >= kBLinear)
	{
		return std::weak_ordering::less;
	}
	else if (f1->GetType() == kModule)
	{
		return std::weak_ordering::greater;
	}
	else if (f1->GetType() == kPolynomial && f2->GetType() >= kBLinear)
	{
		return std::weak_ordering::greater;
	}
	else if (f1->GetType() == kPolynomial)
	{
		return std::weak_ordering::less;
	}
	else if (f2->GetType() == kModule)
	{
		return std::weak_ordering::less;
	}
	else
	{
		return std::weak_ordering::greater;
	}
}
