/******************************************************************************
 ChiSqLabel.h

	Interface for the ChiSqLabel class

	Copyright (C) 2000 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_ChiSqLabel
#define _H_ChiSqLabel

#include <jx-af/jx/JXDecorRect.h>

class ChiSqLabel : public JXDecorRect
{
public:

	ChiSqLabel(JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	virtual ~ChiSqLabel();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
};

#endif
