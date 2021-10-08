/******************************************************************************
 ModuleFitDescription.h

	Interface for the ModuleFitDescription class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_ModuleFitDescription
#define _H_ModuleFitDescription

// Superclass Header
#include <FitDescription.h>

class DLFitModule;

class ModuleFitDescription : public FitDescription
{
public:

	ModuleFitDescription(DLFitModule* fit);
	~ModuleFitDescription() override;

	DLFitModule*	GetFitModule();

private:

	DLFitModule*	itsModule;
};

/******************************************************************************
 GetFitModule (public)

 ******************************************************************************/

inline DLFitModule*
ModuleFitDescription::GetFitModule()
{
	return itsModule;
}

#endif
