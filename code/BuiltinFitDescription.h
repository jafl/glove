/******************************************************************************
 BuiltinFitDescription.h

	Interface for the BuiltinFitDescription class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_BuiltinFitDescription
#define _H_BuiltinFitDescription

// Superclass Header
#include <FitDescription.h>

class BuiltinFitDescription : public FitDescription
{
public:

	BuiltinFitDescription(const FitType type);
	~BuiltinFitDescription() override;
};

#endif
