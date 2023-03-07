/******************************************************************************
 ColByIncDialog.h

	Interface for the ColByIncDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_ColByIncDialog
#define _H_ColByIncDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXTextMenu;
class JXFloatInput;
class JXIntegerInput;
class JXRadioGroup;

class ColByIncDialog : public JXModalDialogDirector
{
public:

	enum
	{
		kAscending = 1,
		kDescending
	};

public:

	ColByIncDialog(JXWindowDirector* supervisor, const JSize count);

	~ColByIncDialog() override;

	void GetDestination(JIndex* source);

	void GetValues(JFloat* beg, JFloat* inc, JInteger* count);

	bool IsAscending();

protected:

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex itsDestCol;

// begin JXLayout

	JXFloatInput*   itsBeginning;
	JXFloatInput*   itsInc;
	JXIntegerInput* itsCount;
	JXTextMenu*     itsDestMenu;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
