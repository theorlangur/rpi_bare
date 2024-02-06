#ifndef DISPLAY_FORMATTER_H_
#define DISPLAY_FORMATTER_H_
#include "display_font.h"
#include "../tools/formatter.h"

namespace display
{
    template<class Display>
    struct DisplayFormatter
    {
        Display &d;
        display::tools::Point p;

        void operator()(char c)
        {
            p = d.draw_char(p, c, 1);
        }

        void operator()(std::string_view const& sv)
        {
            p = d.draw_str(p, sv);
        }
    };
    template<class D>
    DisplayFormatter(D &, display::tools::Point)->DisplayFormatter<D>;
}

#endif
