/******************************************************************************
 NonLinearFitDescription.h

	Interface for the NonLinearFitDescription class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_NonLinearFitDescription
#define _H_NonLinearFitDescription

#include "FitDescription.h"

class NonLinearFitDescription : public FitDescription
{
public:

	NonLinearFitDescription(const JString& name,
							  const JString& function,
							  const JString& fPrimed,
							  const JPtrArray<JString>& vars);
	NonLinearFitDescription(std::istream& is);

	~NonLinearFitDescription() override;

	JString	GetFunctionString() const;
	JString	GetFunctionPrimedString() const;

	void	WriteSetup(std::ostream& os) override; // must call base class first!

private:

	JString	itsFunction;
	JString	itsFPrimed;
};

/******************************************************************************
 GetFunctionString (public)

 ******************************************************************************/

inline JString
NonLinearFitDescription::GetFunctionString()
	const
{
	return itsFunction;
}

/******************************************************************************
 GetFunctionPrimedString (public)

 ******************************************************************************/

inline JString
NonLinearFitDescription::GetFunctionPrimedString()
	const
{
	return itsFPrimed;
}

#endif
