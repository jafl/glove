/******************************************************************************
 GetDelimiterDialog.h

	Interface for the GetDelimiterDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GetDelimiterDialog
#define _H_GetDelimiterDialog

#include <jx-af/jx/JXDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXTextMenu;
class JXRadioGroup;
class JXTextButton;
class JXStaticText;
class JString;
class JXInputField;
class JXIntegerInput;
class JXTextCheckbox;

class GetDelimiterDialog : public JXDialogDirector, public JPrefObject
{
public:

	enum DelimiterType
	{
		kWhiteSpace = 1,
		kSpace,
		kTab,
		kChar
	};

public:

	GetDelimiterDialog(JXWindowDirector* supervisor, const JString& text);

	virtual ~GetDelimiterDialog();

	DelimiterType	GetDelimiterType();
	JUtf8Byte		GetCharacter();
	bool			IsSkippingLines();
	JSize			GetSkipLineCount();
	bool			HasComments();
	const JString&	GetCommentString();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

private:

	JXStaticText*	itsFileText;

// begin JXLayout

	JXRadioGroup*   itsRG;
	JXInputField*   itsCharInput;
	JXTextCheckbox* itsSkipCB;
	JXTextCheckbox* itsCommentCB;
	JXIntegerInput* itsSkipCountInput;
	JXInputField*   itsCommentInput;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
