/******************************************************************************
 GetDelimiterDialog.h

	Interface for the GetDelimiterDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GetDelimiterDialog
#define _H_GetDelimiterDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>

class JXRadioGroup;
class JXTextButton;
class JXStaticText;
class JXCharInput;
class JXIntegerInput;
class JXTextCheckbox;

class GetDelimiterDialog : public JXModalDialogDirector, public JPrefObject
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

	GetDelimiterDialog(const JString& text);

	~GetDelimiterDialog() override;

	DelimiterType	GetDelimiterType();
	JUtf8Byte		GetCharacter();
	bool			IsSkippingLines();
	JSize			GetSkipLineCount();
	bool			HasComments();
	const JString&	GetCommentString();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

// begin JXLayout

	JXRadioGroup*   itsRG;
	JXTextCheckbox* itsSkipCB;
	JXTextCheckbox* itsCommentCB;
	JXStaticText*   itsFileText;
	JXCharInput*    itsCharInput;
	JXIntegerInput* itsSkipCountInput;
	JXCharInput*    itsCommentInput;

// end JXLayout

private:

	void	BuildWindow();
};

#endif
