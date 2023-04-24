/******************************************************************************
 History.h

	Interface for the History class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_History
#define _H_History

#include <jx-af/jx/JXTEBase.h>

class JString;
class JXXFontMenu;
class JXFontSizeMenu;
class JXPrintPlainTextDialog;
class JXToolBar;

class History : public JXTEBase
{
public:

	History(JXMenuBar* menuBar,
			JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);

	~History() override;

	JSize	GetTabCharCount() const;
	void	SetTabCharCount(const JSize charCount);

	void	LoadDefaultToolBar(JXToolBar* toolBar);

protected:

	void		Receive(JBroadcaster* sender, const Message& message) override;
	JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const override;
	void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight) override;

private:

	JSize		itsTabCharCount;


	// owned by the menu bar

	JXXFontMenu*	itsFontMenu;	// nullptr if no fonts are available
	JXFontSizeMenu*	itsSizeMenu;	// nullptr if fonts other than Courier are available

private:

	void	AdjustFont();
	void	AdjustTabWidth();

	static std::weak_ordering
		CompareFontNames(JString * const &, JString * const &);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFontWillChange;

	class FontWillChange : public JBroadcaster::Message
	{
	public:

		FontWillChange()
			:
			JBroadcaster::Message(kFontWillChange)
			{ };
	};
};


/******************************************************************************
 Tab width (# of characters)

 ******************************************************************************/

inline JSize
History::GetTabCharCount()
	const
{
	return itsTabCharCount;
}

inline void
History::SetTabCharCount
	(
	const JSize charCount
	)
{
	itsTabCharCount = charCount;
	AdjustTabWidth();
}


#endif
