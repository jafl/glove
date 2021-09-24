/******************************************************************************
 GLNonLinearFitDescription.h

	Interface for the GLNonLinearFitDescription class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GLNonLinearFitDescription
#define _H_GLNonLinearFitDescription

// Superclass Header
#include <GLFitDescription.h>

class GLNonLinearFitDescription : public GLFitDescription
{
public:

	GLNonLinearFitDescription(const JString& name, 
							  const JString& function, 
							  const JString& fPrimed, 
							  const JPtrArray<JString>& vars);
	GLNonLinearFitDescription(std::istream& is);
								  
	virtual ~GLNonLinearFitDescription();

	JString		GetFunctionString() const;
	JString		GetFunctionPrimedString() const;

	virtual void	WriteSetup(std::ostream& os); // must call base class first!

private:

	JString		itsFunction;
	JString		itsFPrimed;
};

/******************************************************************************
 GetFunctionString (public)

 ******************************************************************************/

inline JString
GLNonLinearFitDescription::GetFunctionString()
	const
{
	return itsFunction;
}

/******************************************************************************
 GetFunctionPrimedString (public)

 ******************************************************************************/

inline JString
GLNonLinearFitDescription::GetFunctionPrimedString()
	const
{
	return itsFPrimed;
}

#endif
