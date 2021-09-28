/******************************************************************************
 GLChiSqLabel.h

	Interface for the GLChiSqLabel class

	Copyright (C) 2000 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLChiSqLabel
#define _H_GLChiSqLabel

#include <jx-af/jx/JXDecorRect.h>

class GLChiSqLabel : public JXDecorRect
{
public:

	GLChiSqLabel(JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	virtual ~GLChiSqLabel();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
};

#endif
