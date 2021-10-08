/******************************************************************************
 CreatePlotDialog.h

	Interface for the CreatePlotDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CreatePlotDialog
#define _H_CreatePlotDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextMenu;
class JXTextCheckbox;
class RaggedFloatTableData;
class DataDocument;
class JXInputField;
class JString;

class CreatePlotDialog : public JXDialogDirector
{
public:

	CreatePlotDialog(DataDocument* supervisor, RaggedFloatTableData* data,
						const JIndex startX, const JIndex startY,
						const JIndex startXErr, const JIndex startYErr);

	~CreatePlotDialog() override;

	void GetColumns(JIndex* startX, JIndex* startXErr,
					JIndex* startY, JIndex* startYErr);

	bool GetPlotIndex(JIndex* index);

	const JString& GetLabel();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	bool	OKToDeactivate() override;

private:

	JIndex itsStartX;
	JIndex itsStartXErr;
	JIndex itsStartY;
	JIndex itsStartYErr;
	JIndex itsPlotIndex;
	DataDocument*	itsTableDir;

// begin JXLayout

	JXInputField* itsLabelInput;
	JXTextMenu*   itsXMenu;
	JXTextMenu*   itsXErrMenu;
	JXTextMenu*   itsYErrMenu;
	JXTextMenu*   itsYMenu;
	JXTextMenu*   itsPlotsMenu;

// end JXLayout

private:

	void	BuildWindow(RaggedFloatTableData* data,
						const JIndex startX, const JIndex startY,
						const JIndex startXErr, const JIndex startYErr);
};

#endif
