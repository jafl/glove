/******************************************************************************
 VarTable.h

	Interface for the VarTable class

	Copyright (C) 2000 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_VarTable
#define _H_VarTable

#include <jx-af/jx/JXEditTable.h>

class JXTextMenu;
class JXExprInput;
class VarList;

class VarTable : public JXEditTable
{
public:

	enum
	{
		kNameColumn = 1,
		kFnColumn   = 2
	};

public:

	VarTable(VarList* varList, 
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~VarTable();

	void	NewConstant();
	void	RemoveSelectedConstant();

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual bool		ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	VarList*		itsVarList;		// not owned
	JXExprInput*	itsTextInput;	// nullptr unless editing
	JString*		itsOrigText;	// used while itsTextInput != nullptr

private:

	void	CreateRow(const JIndex rowIndex);
	void	AdjustColWidths();
};

#endif
