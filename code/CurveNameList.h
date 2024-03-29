/******************************************************************************
 CurveNameList.h

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_CurveNameList
#define _H_CurveNameList

#include <jx-af/jx/JXEditTable.h>
#include <jx-af/j2dplot/J2DPlotWidget.h>

class JXInputField;
class PlotDirector;

class CurveNameList : public JXEditTable
{
public:

	CurveNameList(PlotDirector* dir, J2DPlotWidget* plot,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~CurveNameList() override;

	bool	GetCurrentCurveIndex(JIndex* index);
	void	SetCurrentCurveIndex(const JIndex index);

	void	HandleKeyPress(const JUtf8Character& c,
						   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	JXInputField*	CreateXInputField(const JPoint& cell,
									  const JCoordinate x, const JCoordinate y,
									  const JCoordinate w, const JCoordinate h) override;
	void			PrepareDeleteXInputField() override;
	bool			ExtractInputData(const JPoint& cell) override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	JPtrArray<JString>*	itsNameList;	// we don't own the strings
	JCoordinate			itsMinColWidth;
	JXInputField*		itsInput;
	J2DPlotWidget*		itsPlot;
	PlotDirector*		itsDir;

private:

	void	AdjustColWidth();

public:

	static const JUtf8Byte* kCurveSelected;

	class CurveSelected : public JBroadcaster::Message
	{
	public:

		CurveSelected(const JIndex index)
			:
			JBroadcaster::Message(kCurveSelected),
			itsIndex(index)
			{ };

		JIndex
		GetIndex() const
		{
			return itsIndex;
		};

	private:

		JIndex itsIndex;
	};

};

#endif
