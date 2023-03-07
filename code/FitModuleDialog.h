/******************************************************************************
 FitModuleDialog.h

	Interface for the FitModuleDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_FitModuleDialog
#define _H_FitModuleDialog

#include <jx-af/jx/JXModalDialogDirector.h>

class JXTextMenu;
class JXTextButton;

class FitModuleDialog : public JXModalDialogDirector
{
public:

	FitModuleDialog(JXWindowDirector* supervisor);

	~FitModuleDialog() override;

	JIndex GetFilterIndex();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

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
