/******************************************************************************
 ParmColHeaderWidget.h

	Interface for the ParmColHeaderWidget class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_ParmColHeaderWidget
#define _H_ParmColHeaderWidget


#include <jx-af/jx/JXColHeaderWidget.h>

class JPagePrinter;

class ParmColHeaderWidget : public JXColHeaderWidget
{
public:

public:

	ParmColHeaderWidget(JXTable* table, JXScrollbarSet* scrollbarSet,
						  JXContainer* enclosure,
						  const HSizingOption hSizing, const VSizingOption vSizing,
						  const JCoordinate x, const JCoordinate y,
						  const JCoordinate w, const JCoordinate h);

	~ParmColHeaderWidget() override;

	void	PrintOnPage(JPagePrinter& p);
};

#endif
