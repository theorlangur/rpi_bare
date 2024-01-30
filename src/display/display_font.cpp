#include "display_font.h"
#include "display_tools.h"
#include "fonts/display_font_sys_5x7.h"

namespace display::font
{
    tools::SymbolHeader g_Chars[256];

    constinit const auto sUnknown = display::tools::CreateSymbol(
        "*****\n"
        "*...*\n"
        "**.**\n"
        "*.*.*\n"
        "**.**\n"
        "*...*\n"
        "*****"
        );
    
    void init()
    {
        //1. init by default with unkown
        for(auto &s : g_Chars)
            s = sUnknown;

        display::font::f5x7::init(g_Chars);
    }
}
