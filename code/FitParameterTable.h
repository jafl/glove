/******************************************************************************
 FitParameterTable.h

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_FitParameterTable
#define _H_FitParameterTable

#include <jx-af/jx/JXEditTable.h>
#include <jx-af/j2dplot/J2DPlotWidget.h>

class ParmColHeaderWidget;
class JXFloatInput;
class JPagePrinter;
class FitDescription;

class FitParameterTable : public JXEditTable
{
public:

	FitParameterTable(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~FitParameterTable();

	void	SetColHeaderWidget(ParmColHeaderWidget* widget);
	void	SetFitDescription(const FitDescription& fit);
	void	SetValue(const JIndex index, const JFloat value, const JFloat error);
	void	ClearValues();
	void	CopyParmValuesToStart();
	void	GetValueString(JString* text);

	void		ShowStartCol(const bool show = true);
	bool	IsShowingStartCol() const;

	const JArray<JFloat>&	GetStartValues() const;

	void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

	bool		BeginEditingStartValues();
	void			PrintOnPage(JPagePrinter& p, JCoordinate* height);

protected:

	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	void			PrepareDeleteXInputField() override;
	bool		ExtractInputData(const JPoint& cell) override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	JPtrArray<JString>*	itsNameList;	
	JArray<JFloat>*		itsStartValues;
	JArray<JFloat>*		itsFitValues;
	JArray<JFloat>*		itsErrorValues;
	JCoordinate			itsMinColWidth;
	JXFloatInput*		itsInput;
	bool				itsHasStartValues;

	ParmColHeaderWidget*	itsColHeaderWidget;

private:

	void	AdjustColWidth();
	void	AdjustColWidth(const JSize availabeWidth);

public:

	static const JUtf8Byte* kValueChanged;

	class ValueChanged : public JBroadcaster::Message
	{
	public:

		ValueChanged(const JIndex index, const JFloat value)
			:
			JBroadcaster::Message(kValueChanged),
			itsIndex(index),
			itsValue(value)
			{ };

		JIndex
		GetIndex() const
		{
			return itsIndex;
		};

		JFloat
		GetValue() const
		{
			return itsValue;
		};

	private:

		JIndex itsIndex;
		JFloat itsValue;
	};

};

/******************************************************************************
 IsShowingStartCol (public)

 ******************************************************************************/

inline bool
FitParameterTable::IsShowingStartCol()
	const
{
	return itsHasStartValues;
}


#endif
