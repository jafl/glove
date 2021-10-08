/******************************************************************************
 RowHeaderWidget.h

	Interface for the RowHeaderWidget class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_RowHeaderWidget
#define _H_RowHeaderWidget

#include "jx-af/jx/JXRowHeaderWidget.h"

class RaggedFloatTable;

class RowHeaderWidget : public JXRowHeaderWidget
{
public:

	RowHeaderWidget(RaggedFloatTable* table,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	~RowHeaderWidget() override;

protected:

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
private:

	RaggedFloatTable*		itsTable;		// we don't own this
};

#endif
