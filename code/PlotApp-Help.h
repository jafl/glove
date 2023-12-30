// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_PlotApp_Help
#define _H_PlotApp_Help

static const JUtf8Byte* kHelpMenuStr =
"* %i __About::PlotApp %l"
"|* %i HelpTOC::PlotApp"
"|* %i HelpSpecific::PlotApp %l"
"|* %i __Changes::PlotApp"
"|* %i __Credits::PlotApp"
;

#include "PlotApp-Help-enum.h"

#ifndef _H_jx_af_image_jx_jx_help_toc
#define _H_jx_af_image_jx_jx_help_toc
#include <jx-af/image/jx/jx_help_toc.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_help_specific
#define _H_jx_af_image_jx_jx_help_specific
#include <jx-af/image/jx/jx_help_specific.xpm>
#endif

static void ConfigureHelpMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts("#h");
	}
	menu->SetItemImage(kHelpTOCCmd + offset, jx_help_toc);
	menu->SetItemImage(kHelpWindowCmd + offset, jx_help_specific);
};

#endif
