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

	FitModuleDialog();

	~FitModuleDialog() override;

	JIndex GetFilterIndex();

private:

	JIndex itsFilterIndex;

// begin JXLayout

	JXTextMenu*   itsFilterMenu;
	JXTextButton* itsReloadButton;
	JXTextButton* itsOKButton;

// end JXLayout

private:

	void	BuildWindow();
};


/******************************************************************************
 GetFilterIndex

 ******************************************************************************/

inline JIndex
FitModuleDialog::GetFilterIndex()
{
	return itsFilterIndex;
}

#endif
