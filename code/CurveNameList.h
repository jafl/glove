/******************************************************************************
 CurveNameList.h

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_CurveNameList
#define _H_CurveNameList

#include <jx-af/jx/JXEditTable.h>
#include <jx-af/j2dplot/J2DPlotWidget.h>

class JXInputField;
class PlotDir;

class CurveNameList : public JXEditTable
{
public:

	CurveNameList(PlotDir* dir, J2DPlotWidget* plot,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~CurveNameList();

	bool		GetCurrentCurveIndex(JIndex* index);
	void			SetCurrentCurveIndex(const JIndex index);

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual void			PrepareDeleteXInputField() override;
	virtual bool		ExtractInputData(const JPoint& cell) override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	JPtrArray<JString>*	itsNameList;	// we don't own the strings
	JCoordinate			itsMinColWidth;
	JXInputField*		itsInput;
	J2DPlotWidget*		itsPlot;
	PlotDir*			itsDir;

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
