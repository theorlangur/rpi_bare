#ifndef DISPLAY_RENDER_H_
#define DISPLAY_RENDER_H_
#include "display_tools.h"

namespace display
{
    void render_symbol(display::tools::Point p, const display::tools::SymbolHeader &s);
    void render_symbol(display::tools::Point p, const display::tools::SymbolHeader &s, display::tools::Point off, display::tools::Dims sub);
}

#endif