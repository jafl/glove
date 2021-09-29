/******************************************************************************
 CreateVectorPlotDialog.h

	Interface for the CreateVectorPlotDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CreateVectorPlotDialog
#define _H_CreateVectorPlotDialog

#include <jx-af/jx/JXDialogDirector.h>

class JXTextMenu;
class RaggedFloatTableData;
class DataDocument;
class JXInputField;
class JString;

class CreateVectorPlotDialog : public JXDialogDirector
{
public:

	CreateVectorPlotDialog(DataDocument* supervisor, RaggedFloatTableData* data,
						const JIndex startX, const JIndex startY,
						const JIndex startX2, const JIndex startY2);

	virtual ~CreateVectorPlotDialog();

	void GetColumns(JIndex* X1, JIndex* Y1,
					JIndex* X2, JIndex* Y2);

	bool GetPlotIndex(JIndex* index);

	const JString& GetLabel();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual bool	OKToDeactivate() override;

private:

	JIndex itsStartX1;
	JIndex itsStartX2;
	JIndex itsStartY1;
	JIndex itsStartY2;
	JIndex itsPlotIndex;
	DataDocument*	itsTableDir;

// begin JXLayout

	JXTextMenu*   itsX1Menu;
	JXTextMenu*   itsY1Menu;
	JXTextMenu*   itsX2Menu;
	JXTextMenu*   itsY2Menu;
	JXTextMenu*   itsPlotsMenu;
	JXInputField* itsLabelInput;

// end JXLayout

private:

	void	BuildWindow(RaggedFloatTableData* data,
						const JIndex startX, const JIndex startY,
						const JIndex startXErr, const JIndex startYErr);
};

#endif
