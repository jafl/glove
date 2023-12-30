// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_RaggedFloatTable_Data
#define _H_RaggedFloatTable_Data

static const JUtf8Byte* kDataMenuStr =
"* %i Plot::RaggedFloatTable"
"|* %i PlotVector::RaggedFloatTable %l"
"|* %i Transform::RaggedFloatTable"
"|* %i __GenerateColumnByRange::RaggedFloatTable"
"|* %i __GenerateColumnByIncrement::RaggedFloatTable %l"
"|* %i __DataModule::RaggedFloatTable"
;

#include "RaggedFloatTable-Data-enum.h"

#ifndef _H_plotdata
#define _H_plotdata
#include "plotdata.xpm"
#endif
#ifndef _H_plotvectordata
#define _H_plotvectordata
#include "plotvectordata.xpm"
#endif
#ifndef _H_transform
#define _H_transform
#include "transform.xpm"
#endif

static void ConfigureDataMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#d");
	}
	menu->SetItemImage(kPlotCmd + offset, plotdata);
	menu->SetItemImage(kPlotVectorCmd + offset, plotvectordata);
	menu->SetItemImage(kTransCmd + offset, transform);
};

#endif
