/******************************************************************************
 PolyFitDescription.h

	Interface for the PolyFitDescription class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_PolyFitDescription
#define _H_PolyFitDescription

// Superclass Header
#include <FitDescription.h>



class PolyFitDescription : public FitDescription
{
public:

	PolyFitDescription(const JString& name, const JArray<JIndex>& powers);
	PolyFitDescription(std::istream& is);

	~PolyFitDescription() override;

	void	GetPowers(JArray<JIndex>* powers) const;

	void	WriteSetup(std::ostream& os) override; // must call base class first!

private:

	JArray<JIndex>*	itsPowers;

private:

	void	PolyFitDescriptionX();
};

#endif
