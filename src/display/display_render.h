#ifndef DISPLAY_RENDER_H_
#define DISPLAY_RENDER_H_
#include "display_tools.h"
#include <utility>

template<class DisplayDriver>
class BasicRenderer
{
protected:
    DisplayDriver &m_Driver;
public:
    BasicRenderer(DisplayDriver &d):m_Driver(d){}

    void clear(auto &&...args) { m_Driver.clear(std::forward<decltype(args)>(args)...); }
    void clear_rect(auto &&...args) { m_Driver.clear_rect(std::forward<decltype(args)>(args)...); }
    void clear_part(auto &&...args) { m_Driver.clear_part(std::forward<decltype(args)>(args)...); }
    void show_part(auto &&...args) { m_Driver.show_part(std::forward<decltype(args)>(args)...); }
    void show(auto &&...args) { m_Driver.show(std::forward<decltype(args)>(args)...); }

    void render_symbol(display::tools::Point p, const display::tools::SymbolHeader &s)
    {
        m_Driver.set_bitmap(p.x, p.y, s.pData, s.size.w, s.size.h);
    }
    void render_symbol(display::tools::Point p, const display::tools::SymbolHeader &s, display::tools::Point off, display::tools::Dims sub)
    {
        m_Driver.set_bitmap_sub(p.x, p.y, s.pData, s.size.w, s.size.h, off.x, off.y, sub.w, sub.h);
    }
    void render_line(display::tools::Point p1, display::tools::Point p2)
    {
        if (display::tools::diff_abs(p2.y, p1.y) < display::tools::diff_abs(p2.x, p1.x))
        {
            if (p1.x > p2.x)
                plot_line_low(p2, p1);
            else
                plot_line_low(p1, p2);
        }else
        {
            if (p1.y > p2.y)
                plot_line_high(p2, p1);
            else
                plot_line_high(p1, p2);
        }
    }
private:
    void plot_line_low(display::tools::Point p1, display::tools::Point p2)
    {
        int16_t dx = p2.x - p1.x;
        int16_t dy = p2.y - p1.y;
        int16_t yi = 1;
        if (dy < 0)
        {
            yi = -1;
            dy = -dy;
        }
        int16_t D = (2 * dy) - dx;
        uint8_t y = p1.y;

        for(uint8_t x = p1.x; x <= p2.x; ++x)
        {
            m_Driver.set_point(x, y, true);
            if (D > 0)
            {
                y += yi;
                D += 2 * (dy - dx);
            }else
                D += 2 * dy;
        }
    }

    void plot_line_high(display::tools::Point p1, display::tools::Point p2)
    {
        int16_t dx = p2.x - p1.x;
        int16_t dy = p2.y - p1.y;
        int16_t xi = 1;
        if (dx < 0)
        {
            xi = -1;
            dx = -dx;
        }
        int16_t D = (2 * dx) - dy;
        uint8_t x = p1.x;

        for(uint8_t y = p1.y; y <= p2.y; ++y)
        {
            m_Driver.set_point(x, y, true);
            if (D > 0)
            {
                x += xi;
                D += 2 * (dx - dy);
            }else
                D += 2 * dx;
        }
    }
};

#endif
