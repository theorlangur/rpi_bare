#ifndef DISPLAY_FONT_H_
#define DISPLAY_FONT_H_
#include <string_view>
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

            display::tools::Point draw_char(display::tools::Point p, char c, uint8_t off = 0)
            {
                if (c == '\n')
                    return {0, uint8_t(p.y + g_Chars['A'].size.h + 1)};
                const auto &s = g_Chars[c];
                this->render_symbol(p, s);
                p.x += s.size.w + off;
                return p;
            }

            display::tools::Point draw_str(display::tools::Point p, const char *pStr)
            {
                if (!pStr)
                    return p;

                while(*pStr)
                {
                    if (*pStr != '\n')
                    {
                        auto const& s = g_Chars[*pStr];
                        this->render_symbol(p, s);
                        p.x += s.size.w + 1;
                    }else
                    {
                        p.x = 0;
                        p.y += g_Chars['A'].size.h + 1;
                    }
                    ++pStr;
                }
                return p;
            }

            display::tools::Point draw_str(display::tools::Point p, std::string_view const& sv)
            {
                for(char c : sv)
                {
                    if (c != '\n')
                    {
                        auto const& s = g_Chars[c];
                        this->render_symbol(p, s);
                        p.x += s.size.w + 1;
                    }else
                    {
                        p.x = 0;
                        p.y += g_Chars['A'].size.h + 1;
                    }
                }
                return p;
            }

            void draw_str_in_rect(display::tools::Rect r, const char *pStr)
            {
                if (!pStr) return;

                display::tools::Point p = r.p;
                while(*pStr)
                {
                    if (*pStr != '\n')
                    {
                        auto const& s = g_Chars[*pStr];
                        this->render_symbol(p, s);
                        p.x += s.size.w + 1;
                    }else
                    {
                        p.x = r.p.x;
                        p.y += g_Chars['A'].size.h + 1;
                    }
                    ++pStr;
                }
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

            display::tools::Point draw_uint(display::tools::Point p, uint32_t v)
            {
                uint8_t digits[16] = {'0'};
                uint8_t digit_count = v == 0 ? 1 : 0;
                while(v)
                {
                    digits[digit_count++] = '0' + v % 10;
                    v /= 10;
                }

                for(uint8_t i = 0; i < digit_count; ++i)
                    p = draw_char(p, digits[digit_count - i - 1], 1);
                return p;
            }

            display::tools::Point draw_int(display::tools::Point p, int32_t v)
            {
                if (v < 0)
                {
                    p = draw_char(p, '-');
                    ++p.x;
                    v = -v;
                }
                return draw_uint(p, (uint32_t)v);
            }

            template<class T>
            void print(display::tools::Point &p, T &&v)
            {
                using BareT = std::remove_reference_t<T>;
                if constexpr(std::is_convertible_v<BareT, const char*>)
                {
                    p = draw_str(p, v);
                }
                else if constexpr(std::is_integral_v<BareT> && std::is_signed_v<BareT>)
                {
                    p = draw_int(p, uint32_t(v));
                }
                else if constexpr(std::is_integral_v<BareT> && std::is_unsigned_v<BareT>)
                {
                    p = draw_uint(p, uint32_t(v));
                }else
                {
                    static_assert(sizeof(T) == 0, "No idea how to print T");
                }
            }

            template<class... T>
            auto print_all(display::tools::Point p, T &&...v)
            {
                return (print(p, v),...);
            }
        };
    }
}


#endif
