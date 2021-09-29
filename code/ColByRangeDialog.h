/******************************************************************************
 ColByRangeDialog.h

	Interface for the ColByRangeDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_ColByRangeDialog
#define _H_ColByRangeDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextMenu;
class JXFloatInput;
class JXIntegerInput;

class ColByRangeDialog : public JXDialogDirector
{
public:

	ColByRangeDialog(JXWindowDirector* supervisor, const JSize count);

	virtual ~ColByRangeDialog();
	
	void GetDestination(JIndex* dest);
	
	void GetValues(JFloat* beg, JFloat* end, JInteger* count);
				
	bool IsAscending();
	
protected:

	virtual bool	OKToDeactivate() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	
private:

	JIndex 		itsDestCol;
	bool 	itsIsAscending;

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
