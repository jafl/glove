/******************************************************************************
 ChooseFileImportDialog.h

	Interface for the ChooseFileImportDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_ChooseFileImportDialog
#define _H_ChooseFileImportDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXTextMenu;
class JXTextButton;
class JXStaticText;
class DataDocument;
class JString;

class ChooseFileImportDialog : public JXModalDialogDirector, public JPrefObject
{
public:

	ChooseFileImportDialog(DataDocument* supervisor, const JString& filename);

	~ChooseFileImportDialog() override;

	JIndex			GetFilterIndex();
	const JString&	GetFileText();

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

	JIndex			itsFilterIndex;
	DataDocument*	itsDir;

// begin JXLayout

	JXTextMenu*   itsFilterMenu;
	JXTextButton* itsReloadButton;
	JXTextButton* okButton;
	JXStaticText* itsFileText;

// end JXLayout

private:

	void	BuildWindow(const JString& filename);
};

#endif
