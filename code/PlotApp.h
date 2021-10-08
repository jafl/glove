/******************************************************************************
 PlotApp.h

	Interface for the PlotApp class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_PlotApp
#define _H_PlotApp

#include <jx-af/jx/JXApplication.h>

class PlotApp : public JXApplication
{
public:

	PlotApp(int* argc, char* argv[], bool* displayAbout, JString* prevVersStr);

	~PlotApp() override;

	bool				GetImportModulePath(const JIndex index, JString* path);
	JPtrArray<JString>*	GetImportModules();
	void				ReloadImportModules();

	bool				GetExportModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* GetExportModules();
	void				ReloadExportModules();

	bool				GetDataModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* GetDataModules();
	void				ReloadDataModules();

	bool				GetFitModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* GetFitModules();
	void				ReloadFitModules();

	bool				GetCursorModulePath(const JIndex index, JString* path);
	JPtrArray<JString>* GetCursorModules();
	void				ReloadCursorModules();

	const JPtrArray<JString>&	GetModulePath() const;

	void NewFile();
	void OpenFile(const JString& filename);
	void DisplayAbout(const JString& prevVersStr = JString::empty);

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

protected:

	void	DirectorClosed(JXDirector* theDirector) override;

private:

	JIndex				itsDirNumber;

	JPtrArray<JString>* itsModulePath;

	JPtrArray<JString>*	itsDataModules;
	JPtrArray<JString>*	itsCursorModules;
	JPtrArray<JString>*	itsExportModules;
	JPtrArray<JString>*	itsImportModules;
	JPtrArray<JString>*	itsFitModules;

	JArray<JIndex>*		itsDataPathIndex;
	JArray<JIndex>*		itsCursorPathIndex;
	JArray<JIndex>*		itsExportPathIndex;
	JArray<JIndex>*		itsImportPathIndex;
	JArray<JIndex>*		itsFitPathIndex;

};

#endif
