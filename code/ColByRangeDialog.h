/******************************************************************************
 ColByRangeDialog.h

	Interface for the ColByRangeDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_ColByRangeDialog
#define _H_ColByRangeDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXTextMenu;
class JXFloatInput;
class JXIntegerInput;

class ColByRangeDialog : public JXModalDialogDirector
{
public:

	ColByRangeDialog(const JSize count);

	~ColByRangeDialog() override;

	void GetDestination(JIndex* dest);

	void GetValues(JFloat* beg, JFloat* end, JInteger* count);

	bool IsAscending();

protected:

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex	itsDestCol;
	bool	itsIsAscending;

// begin JXLayout

	JXFloatInput*   itsBeginning;
	JXFloatInput*   itsEnd;
	JXIntegerInput* itsCount;
	JXTextMenu*     itsDestMenu;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
