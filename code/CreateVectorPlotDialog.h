/******************************************************************************
 CreateVectorPlotDialog.h

	Interface for the CreateVectorPlotDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CreateVectorPlotDialog
#define _H_CreateVectorPlotDialog

#include "CreatePlotDialogBase.h"

class RaggedFloatTableData;

class CreateVectorPlotDialog : public CreatePlotDialogBase
{
public:

	CreateVectorPlotDialog(DataDocument* supervisor, RaggedFloatTableData* data,
							const JIndex startX, const JIndex startY,
							const JIndex startX2, const JIndex startY2);

	~CreateVectorPlotDialog() override;

	void GetColumns(JIndex* X1, JIndex* X2,
					JIndex* Y1, JIndex* Y2) override;

private:

	DataDocument* itsTableDir;

	JIndex itsStartX1;
	JIndex itsStartX2;
	JIndex itsStartY1;
	JIndex itsStartY2;

// begin JXLayout

	JXTextMenu* itsX1Menu;
	JXTextMenu* itsY1Menu;
	JXTextMenu* itsX2Menu;
	JXTextMenu* itsY2Menu;

// end JXLayout

private:

	void	BuildWindow(RaggedFloatTableData* data,
						const JIndex startX, const JIndex startY,
						const JIndex startXErr, const JIndex startYErr);
};

#endif
