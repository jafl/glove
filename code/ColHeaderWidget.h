/******************************************************************************
 ColHeaderWidget.h

	Interface for the ColHeaderWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_ColHeaderWidget
#define _H_ColHeaderWidget

#include "jx-af/jx/JXColHeaderWidget.h"

class RaggedFloatTable;

class ColHeaderWidget : public JXColHeaderWidget
{
public:

	ColHeaderWidget(RaggedFloatTable* table, JXScrollbarSet* scrollbarSet,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	~ColHeaderWidget() override;

protected:

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

private:

	RaggedFloatTable*	itsTable;		// we don't own this
};

#endif
