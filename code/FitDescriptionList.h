/******************************************************************************
 FitDescriptionList.h

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_FitDescriptionList
#define _H_FitDescriptionList

#include <jx-af/jx/JXEditTable.h>
#include <jx-af/j2dplot/J2DPlotWidget.h>

class JXInputField;
class JXImage;

class FitDescriptionList : public JXEditTable
{
public:

	FitDescriptionList(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~FitDescriptionList() override;

	bool	GetCurrentFitIndex(JIndex* index);
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

	// not owned

	JXImage*	itsBuiltInIcon;
	JXImage*	itsNonLinearIcon;
	JXImage*	itsPolyIcon;
	JXImage*	itsExecutableIcon;

private:

	void	SyncWithManager();
	void	AdjustColWidth();

public:

	static const JUtf8Byte* kFitSelected;
	static const JUtf8Byte* kFitInitiated;

	class FitSelected : public JBroadcaster::Message
	{
		public:

			FitSelected(const JIndex index)
				:
				JBroadcaster::Message(kFitSelected),
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

	class FitInitiated : public JBroadcaster::Message
	{
		public:

			FitInitiated(const JIndex index)
				:
				JBroadcaster::Message(kFitInitiated),
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
