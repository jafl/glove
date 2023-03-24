/******************************************************************************
 CreatePlotDialogBase.h

	Copyright @ 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_CreatePlotDialogBase
#define _H_CreatePlotDialogBase

#include <jx-af/jx/JXModalDialogDirector.h>

class DataDocument;
class JXTextMenu;
class JXInputField;

class CreatePlotDialogBase : public JXModalDialogDirector
{
public:

	CreatePlotDialogBase();

	~CreatePlotDialogBase() override;

	virtual void GetColumns(JIndex* w, JIndex* x,
							JIndex* y, JIndex* z) = 0;

	bool GetPlotIndex(JIndex* index) const;

	const JString& GetLabel();

protected:

	void	SetObjects(DataDocument* tableDir, JXInputField* labelInput,
					   JXTextMenu* plotMenu);

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex			itsPlotIndex;
	JXTextMenu*		itsPlotMenu;
	JXInputField*	itsLabelInput;
};


/******************************************************************************
 GetPlotIndex

 ******************************************************************************/

inline bool
CreatePlotDialogBase::GetPlotIndex
	(
	JIndex* index
	)
	const
{
	if (itsPlotIndex == 1)
	{
		return false;
	}

	*index = itsPlotIndex - 1;
	return true;
}

#endif
