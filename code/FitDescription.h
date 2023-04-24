/******************************************************************************
 FitDescription.h

	Interface for the FitDescription class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_FitDescription
#define _H_FitDescription

#include "FitBase.h"
#include "VarList.h"
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JString.h>

class FitDescription : public FitBase
{
public:

	enum FitType
	{
		kPolynomial = 1,
		kNonLinear,
		kModule,
		kBLinear,
		kBQuadratic,
		kBExp,
		kBPower
	};

public:

	FitDescription(const FitType type, const JString& form = JString::empty,
					 const JString& name = JString::empty);

	static bool	Create(std::istream& is, FitDescription** fd);

	~FitDescription() override;

	FitType	GetType() const;
	void	SetType(const FitType type);

	bool	GetParameterName(const JIndex index, JString* name) const override;
	bool	GetParameter(const JIndex index, JFloat* value) const override;

	JString	GetFitFunctionString() const override;
	void	SetFitFunctionString(const JString& form);

	const JString&	GetFnName() const;

	bool	RequiresStartValues() const;
	bool	CanUseStartValues() const;

	VarList*	GetVarList();

	virtual void	WriteSetup(std::ostream& os); // must call base class first!

	static std::weak_ordering
		CompareFits(FitDescription * const &, FitDescription * const &);

protected:

	void	SetFnName(const JString& name);
	void	DoesRequireStartValues(const bool require);
	void	SetCanUseStartValues(const bool use);

private:

	VarList*	itsVarList;
	FitType		itsType;
	JString		itsFnForm;
	JString		itsFnName;
	bool		itsRequiresStartValues;
	bool		itsCanUseStartValues;
};

/******************************************************************************
 GetType (public)

 ******************************************************************************/

inline FitDescription::FitType
FitDescription::GetType()
	const
{
	return itsType;
}

inline void
FitDescription::SetType
	(
	const FitType type
	)
{
	itsType	= type;
}

/******************************************************************************
 GetFitFunctionString (public)

 ******************************************************************************/

inline JString
FitDescription::GetFitFunctionString()
	const
{
	return itsFnForm;
}

inline void
FitDescription::SetFitFunctionString
	(
	const JString& form
	)
{
	itsFnForm = form;
}

/******************************************************************************
 GetFnName (public)

 ******************************************************************************/

inline const JString&
FitDescription::GetFnName()
	const
{
	return itsFnName;
}

 // protected

inline void
FitDescription::SetFnName
	(
	const JString& name
	)
{
	itsFnName = name;
}

/******************************************************************************
 RequiresStartValues (public)

 ******************************************************************************/

inline bool
FitDescription::RequiresStartValues()
	const
{
	return itsRequiresStartValues;
}

// protected

inline void
FitDescription::DoesRequireStartValues
	(
	const bool require
	)
{
	itsRequiresStartValues = require;
}

/******************************************************************************
 CanUseStartValues (public)

 ******************************************************************************/

inline bool
FitDescription::CanUseStartValues()
	const
{
	return itsCanUseStartValues;
}

// protected

inline void
FitDescription::SetCanUseStartValues
	(
	const bool use
	)
{
	itsCanUseStartValues = use;
}

/******************************************************************************
 GetVarList (public)

 ******************************************************************************/

inline VarList*
FitDescription::GetVarList()
{
	return itsVarList;
}

#endif
