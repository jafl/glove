/******************************************************************************
 HistoryDir.h

	Interface for the HistoryDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_HistoryDir
#define _H_HistoryDir

#include <jx-af/jx/JXDocument.h>

class History;
class JXTextMenu;
class JXMenuBar;

class HistoryDir : public JXDocument
{
public:

	HistoryDir(JXDirector* supervisor);

	~HistoryDir() override;

	void	AppendText(const JString& text, const bool show = true);
	void	Print();
	void	WriteData(std::ostream& os);
	void	ReadData(std::istream& is);
	bool	NeedsSave() const override;
	void	SafetySave(const JXDocumentManager::SafetySaveReason reason) override;

protected:

	bool	OKToClose() override;
	bool	OKToRevert() override;
	bool	CanRevert() override;
	void	DiscardChanges() override;

private:

	JXTextMenu*	itsFileMenu;

// begin JXLayout

	JXMenuBar* itsMenuBar;
	History*   itsHistory;

// end JXLayout

private:

	void BuildWindow();
	void UpdateFileMenu();
	void HandleFileMenu(const JIndex index);

public:

	static const JUtf8Byte* kSessionChanged;

	class SessionChanged : public JBroadcaster::Message
	{
	public:

		SessionChanged()
			:
			JBroadcaster::Message(kSessionChanged)
			{ };
	};

};

#endif
