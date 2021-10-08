/******************************************************************************
 ChooseFileImportDialog.h

	Interface for the ChooseFileImportDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_ChooseFileImportDialog
#define _H_ChooseFileImportDialog

#include <jx-af/jx/JXDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXTextMenu;
class JXTextButton;
class JXStaticText;
class DataDocument;
class JString;

class ChooseFileImportDialog : public JXDialogDirector, public JPrefObject
{
public:

	ChooseFileImportDialog(DataDocument* supervisor, const JString& filename);

	~ChooseFileImportDialog() override;

	JIndex			GetFilterIndex();
	const JString&	GetFileText();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

	JIndex			itsFilterIndex;
	JXStaticText*	itsFileText;
	DataDocument*	itsDir;

// begin JXLayout

	JXTextButton* itsReloadButton;
	JXTextMenu*   itsFilterMenu;

// end JXLayout

private:

	void	BuildWindow(const JString& filename);
};

#endif
