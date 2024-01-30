#ifndef DISPLAY_FONT_H_
#define DISPLAY_FONT_H_
#include "display_tools.h"
#include "display_render.h"

namespace display
{
    namespace font
    {
        extern tools::SymbolHeader g_Chars[256];

        void init();

        template<class DisplayDriver>
        class FontRenderer: public BasicRenderer<DisplayDriver>
        {
        public:
            FontRenderer(DisplayDriver &d):BasicRenderer<DisplayDriver>(d){}

            display::tools::Point draw_char(display::tools::Point p, char c)
            {
                const auto &s = g_Chars[c];
                this->render_symbol(p, s);
                p.x += s.size.w;
                return p;
            }
            display::tools::Point draw_str(display::tools::Point p, const char *pStr)
            {
                if (!pStr)
                    return p;

                while(*pStr)
                {
                    auto const& s = g_Chars[*pStr];
                    this->render_symbol(p, s);
                    p.x += s.size.w + 1;
                    ++pStr;
                }
                return p;
            }

            display::tools::Point draw_hex(display::tools::Point p, uint32_t v)
            {
                auto off = draw_str(p, "0x");
                for(size_t i = 0; i < 8; ++i, ++off.x)
                {
                    uint8_t val = (v >> ((7 - i) * 4)) & 0x0f;
                    if (val >= 0 && val <= 9)
                        off = draw_char(off, '0' + val);
                    else
                        off = draw_char(off, 'a' + (val - 10));
                }

                return off;
            }
        };
    }
}


#endif
