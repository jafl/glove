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

	ColByIncDialog(const JSize count);

	~ColByIncDialog() override;

	void GetDestination(JIndex* source);

	void GetValues(JFloat* beg, JFloat* inc, JInteger* count);

	bool IsAscending();

protected:

	bool	OKToDeactivate() override;

private:

	JIndex itsDestCol;

// begin JXLayout

	JXTextMenu*     itsDestMenu;
	JXFloatInput*   itsBeginning;
	JXFloatInput*   itsInc;
	JXIntegerInput* itsCount;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
