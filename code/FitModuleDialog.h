/******************************************************************************
 FitModuleDialog.h

	Interface for the FitModuleDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_FitModuleDialog
#define _H_FitModuleDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextMenu;
class JXTextButton;

class FitModuleDialog : public JXDialogDirector
{
public:

	FitModuleDialog(JXWindowDirector* supervisor);

	virtual ~FitModuleDialog();

	JIndex GetFilterIndex();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex itsFilterIndex;

// begin JXLayout

	JXTextMenu*   itsFilterMenu;
	JXTextButton* itsOKButton;
	JXTextButton* itsReloadButton;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
