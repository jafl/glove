/******************************************************************************
 GLModuleFitDescription.h

	Interface for the GLModuleFitDescription class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GLModuleFitDescription
#define _H_GLModuleFitDescription

// Superclass Header
#include <GLFitDescription.h>

class GLDLFitModule;

class GLModuleFitDescription : public GLFitDescription
{
public:

	GLModuleFitDescription(GLDLFitModule* fit);
	virtual ~GLModuleFitDescription();

	GLDLFitModule*		GetFitModule();

private:

	GLDLFitModule*		itsModule;
};

/******************************************************************************
 GetFitModule (public)

 ******************************************************************************/

inline GLDLFitModule*
GLModuleFitDescription::GetFitModule()
{
	return itsModule;
}

#endif
