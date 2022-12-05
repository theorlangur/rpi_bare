#ifndef DISPLAY_FONT_H_
#define DISPLAY_FONT_H_
#include "display_tools.h"

namespace display
{
    namespace font
    {
        extern tools::SymbolHeader g_Chars[256];

        void init();

        display::tools::Point draw_char(display::tools::Point p, char c);
        display::tools::Point draw_str(display::tools::Point p, const char *pStr);

        display::tools::Point draw_hex(display::tools::Point p, uint32_t v);
    }
}


#endif