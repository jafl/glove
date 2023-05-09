/******************************************************************************
 CreatePlotDialog.h

	Interface for the CreatePlotDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CreatePlotDialog
#define _H_CreatePlotDialog

#include "CreatePlotDialogBase.h"

class RaggedFloatTableData;

class CreatePlotDialog : public CreatePlotDialogBase
{
public:

	CreatePlotDialog(DataDocument* supervisor, RaggedFloatTableData* data,
					 const JIndex startX, const JIndex startY,
					 const JIndex startXErr, const JIndex startYErr);

	~CreatePlotDialog() override;

	void GetColumns(JIndex* startX, JIndex* startXErr,
					JIndex* startY, JIndex* startYErr) override;

private:

	DataDocument* itsTableDir;

	JIndex itsStartX;
	JIndex itsStartXErr;
	JIndex itsStartY;
	JIndex itsStartYErr;

// begin JXLayout

	JXTextMenu* itsXMenu;
	JXTextMenu* itsXErrMenu;
	JXTextMenu* itsYErrMenu;
	JXTextMenu* itsYMenu;

// end JXLayout

private:

	void	BuildWindow(RaggedFloatTableData* data,
						const JIndex startX, const JIndex startY,
						const JIndex startXErr, const JIndex startYErr);
};

#endif
