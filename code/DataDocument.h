/******************************************************************************
 DataDocument.h

	Interface for the DataDocument class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_DataDocument
#define _H_DataDocument

#include <jx-af/jx/JXFileDocument.h>
#include <jx-af/jcore/JPtrArray-JString.h>

class PlotDirector;
class RaggedFloatTableData;
class RaggedFloatTable;
class JXTextMenu;
class JXToolBar;
class JXPSPrinter;
class JXModalDialogDirector;
class JXScrollbarSet;

class DataDocument : public JXFileDocument
{
public:

	DataDocument(JXDirector* supervisor, const JString& fileName,
					const bool onDisk);

	~DataDocument() override;

	void CreateNewPlot(	const JIndex type,
						const JArray<JFloat>& xCol, const JArray<JFloat>* xErrCol,
						const JArray<JFloat>& yCol, const JArray<JFloat>* yErrCol,
						const bool linked, const JString& label );

	void AddToPlot( const JIndex plotIndex, const JIndex type,
					const JArray<JFloat>& xCol, const JArray<JFloat>* xErrCol,
					const JArray<JFloat>& yCol, const JArray<JFloat>* yErrCol,
					const bool linked, const JString& label );

	void					GetPlotNames(JPtrArray<JString>& names);
	RaggedFloatTableData*	GetData();
	JString					GetInternalModuleName(const JIndex index) const;
	JSize					GetInternalModuleCount();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	DirectorClosed(JXDirector* theDirector) override;
	void	WriteTextFile(std::ostream& output, const bool safetySave) const override;
	void	DiscardChanges() override;

private:

	JXPSPrinter*			itsPrinter;
	RaggedFloatTableData*	itsData;
	RaggedFloatTable*		itsTable;			// owned by its enclosure
	JXTextMenu*				itsFileMenu;		// owned by the menu bar
	JXTextMenu*				itsExportMenu;
	JXTextMenu*				itsPrefsMenu;

	JString					itsCurrentFileName;

	JPtrArray<PlotDirector>*itsPlotWindows;
	JSize					itsPlotNumber;
	bool					itsListenToData;

	JXScrollbarSet*			itsScrollbarSet;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	void	BuildWindow();
	void	LoadFile(const JString& fileName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex item);

	void	UpdateExportMenu();
	void	HandleExportMenu(const JIndex item);

	void	HandlePrefsMenu(const JIndex index);

	bool	LoadNativeFile(std::istream& is);
	void	LoadDelimitedFile(const JString& fileText);

	void	ChooseFileFilter();
	void	ReadPlotData(std::istream& is, const JFloat gloveVersion);
};

/******************************************************************************
 GetData

 ******************************************************************************/

inline RaggedFloatTableData*
DataDocument::GetData()
{
	return itsData;
}

#endif
