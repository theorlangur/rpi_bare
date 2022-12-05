#include "display_render.h"

namespace display
{
    void render_symbol(display::tools::Point p, const display::tools::SymbolHeader &s)
    {
        display::set_bitmap(p.x, p.y, s.pData, s.size.w, s.size.h);
    }
}