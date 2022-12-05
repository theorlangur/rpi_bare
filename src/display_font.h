#ifndef DISPLAY_FONT_H_
#define DISPLAY_FONT_H_
#include "display_driver.h"

namespace hex
{
    constexpr static uint8_t kH = 5;
    constexpr static uint8_t kW = 4;

    void draw_line(uint8_t x, uint8_t y, char x0, char x1, char x2, char x3)
    {
        display::set_point(x    , y, x0 == '*');
        display::set_point(x + 1, y, x1 == '*');
        display::set_point(x + 2, y, x2 == '*');
        display::set_point(x + 3, y, x3 == '*');
    }

    void draw_num_0(uint8_t x, uint8_t y)
    {
        draw_line(x, y + 0, '.', '*', '*', '.');
        draw_line(x, y + 1, '*', '.', '.', '*');
        draw_line(x, y + 2, '*', '.', '.', '*');
        draw_line(x, y + 3, '*', '.', '.', '*');
        draw_line(x, y + 4, '.', '*', '*', '.');
    }

    void draw_num_1(uint8_t x, uint8_t y)
    {
        draw_line(x, y + 0, '.', '.', '.', '*');
        draw_line(x, y + 1, '.', '.', '*', '*');
        draw_line(x, y + 2, '.', '*', '.', '*');
        draw_line(x, y + 3, '.', '.', '.', '*');
        draw_line(x, y + 4, '.', '.', '.', '*');
    }
        /*
        draw_line(x, y + 0, '.', '.', '.', '.');
        draw_line(x, y + 1, '.', '.', '.', '.');
        draw_line(x, y + 2, '.', '.', '.', '.');
        draw_line(x, y + 3, '.', '.', '.', '.');
        draw_line(x, y + 4, '.', '.', '.', '.');
        */

    void draw_num_2(uint8_t x, uint8_t y)
    {
        draw_line(x, y + 0, '.', '*', '*', '.');
        draw_line(x, y + 1, '*', '.', '.', '*');
        draw_line(x, y + 2, '.', '.', '*', '.');
        draw_line(x, y + 3, '.', '*', '.', '.');
        draw_line(x, y + 4, '*', '*', '*', '*');
    }

    void draw_num_3(uint8_t x, uint8_t y)
    {
        draw_line(x, y + 0, '*', '*', '*', '*');
        draw_line(x, y + 1, '.', '.', '*', '.');
        draw_line(x, y + 2, '.', '*', '*', '.');
        draw_line(x, y + 3, '*', '.', '.', '*');
        draw_line(x, y + 4, '.', '*', '*', '.');
    }

    void draw_num_4(uint8_t x, uint8_t y)
    {
        draw_line(x, y + 0, '.', '*', '.', '*');
        draw_line(x, y + 1, '*', '.', '.', '*');
        draw_line(x, y + 2, '*', '*', '*', '*');
        draw_line(x, y + 3, '.', '.', '.', '*');
        draw_line(x, y + 4, '.', '.', '.', '*');
    }

    void draw_num_5(uint8_t x, uint8_t y)
    {
        draw_line(x, y + 0, '*', '*', '*', '*');
        draw_line(x, y + 1, '*', '.', '.', '.');
        draw_line(x, y + 2, '.', '*', '*', '.');
        draw_line(x, y + 3, '.', '.', '.', '*');
        draw_line(x, y + 4, '*', '*', '*', '.');
    }

    void draw_num_6(uint8_t x, uint8_t y)
    {
        draw_line(x, y + 0, '.', '*', '*', '.');
        draw_line(x, y + 1, '*', '.', '.', '.');
        draw_line(x, y + 2, '*', '*', '*', '.');
        draw_line(x, y + 3, '*', '.', '.', '*');
        draw_line(x, y + 4, '.', '*', '*', '.');
    }

    void draw_num_7(uint8_t x, uint8_t y)
    {
        draw_line(x, y + 0, '*', '*', '*', '*');
        draw_line(x, y + 1, '.', '.', '.', '*');
        draw_line(x, y + 2, '.', '.', '*', '.');
        draw_line(x, y + 3, '.', '*', '.', '.');
        draw_line(x, y + 4, '.', '*', '.', '.');
    }

    void draw_num_8(uint8_t x, uint8_t y)
    {
        draw_line(x, y + 0, '.', '*', '*', '.');
        draw_line(x, y + 1, '*', '.', '.', '*');
        draw_line(x, y + 2, '.', '*', '*', '.');
        draw_line(x, y + 3, '*', '.', '.', '*');
        draw_line(x, y + 4, '.', '*', '*', '.');
    }

    void draw_num_9(uint8_t x, uint8_t y)
    {
        draw_line(x, y + 0, '.', '*', '*', '.');
        draw_line(x, y + 1, '*', '.', '.', '*');
        draw_line(x, y + 2, '.', '*', '*', '*');
        draw_line(x, y + 3, '.', '.', '.', '*');
        draw_line(x, y + 4, '.', '*', '*', '.');
    }

    void draw_num_a(uint8_t x, uint8_t y)
    {

        draw_line(x, y + 0, '.', '*', '*', '.');
        draw_line(x, y + 1, '*', '.', '.', '*');
        draw_line(x, y + 2, '*', '*', '*', '*');
        draw_line(x, y + 3, '*', '.', '.', '*');
        draw_line(x, y + 4, '*', '.', '.', '*');
    }

    void draw_num_b(uint8_t x, uint8_t y)
    {

        draw_line(x, y + 0, '*', '*', '*', '.');
        draw_line(x, y + 1, '*', '.', '.', '*');
        draw_line(x, y + 2, '*', '*', '*', '.');
        draw_line(x, y + 3, '*', '.', '.', '*');
        draw_line(x, y + 4, '*', '*', '*', '.');
    }

    void draw_num_c(uint8_t x, uint8_t y)
    {

        draw_line(x, y + 0, '.', '*', '*', '.');
        draw_line(x, y + 1, '*', '.', '.', '*');
        draw_line(x, y + 2, '*', '.', '.', '.');
        draw_line(x, y + 3, '*', '.', '.', '*');
        draw_line(x, y + 4, '.', '*', '*', '.');
    }

    void draw_num_d(uint8_t x, uint8_t y)
    {

        draw_line(x, y + 0, '*', '*', '*', '.');
        draw_line(x, y + 1, '*', '.', '.', '*');
        draw_line(x, y + 2, '*', '.', '.', '*');
        draw_line(x, y + 3, '*', '.', '.', '*');
        draw_line(x, y + 4, '*', '*', '*', '.');
    }

    void draw_num_e(uint8_t x, uint8_t y)
    {

        draw_line(x, y + 0, '*', '*', '*', '.');
        draw_line(x, y + 1, '*', '.', '.', '.');
        draw_line(x, y + 2, '*', '*', '*', '.');
        draw_line(x, y + 3, '*', '.', '.', '.');
        draw_line(x, y + 4, '*', '*', '*', '.');
    }

    void draw_num_f(uint8_t x, uint8_t y)
    {

        draw_line(x, y + 0, '*', '*', '*', '*');
        draw_line(x, y + 1, '*', '.', '.', '.');
        draw_line(x, y + 2, '*', '*', '*', '.');
        draw_line(x, y + 3, '*', '.', '.', '.');
        draw_line(x, y + 4, '*', '.', '.', '.');
    }

    void draw_num_x(uint8_t x, uint8_t y)
    {

        draw_line(x, y + 0, '.', '.', '.', '.');
        draw_line(x, y + 1, '.', '.', '.', '.');
        draw_line(x, y + 2, '*', '.', '*', '.');
        draw_line(x, y + 3, '.', '*', '.', '.');
        draw_line(x, y + 4, '*', '.', '*', '.');
    }

    void draw_num_err(uint8_t x, uint8_t y)
    {

        draw_line(x, y + 0, '.', '*', '*', '.');
        draw_line(x, y + 1, '*', '*', '*', '*');
        draw_line(x, y + 2, '*', '*', '*', '*');
        draw_line(x, y + 3, '.', '*', '*', '.');
        draw_line(x, y + 4, '.', '*', '*', '.');
    }

    void draw_num(uint8_t x, uint8_t y, char c)
    {
        switch(c)
        {
            case 0: draw_num_0(x, y); break;
            case 1: draw_num_1(x, y); break;
            case 2: draw_num_2(x, y); break;
            case 3: draw_num_3(x, y); break;
            case 4: draw_num_4(x, y); break;
            case 5: draw_num_5(x, y); break;
            case 6: draw_num_6(x, y); break;
            case 7: draw_num_7(x, y); break;
            case 8: draw_num_8(x, y); break;
            case 9: draw_num_9(x, y); break;
            case 10: draw_num_a(x, y); break;
            case 11: draw_num_b(x, y); break;
            case 12: draw_num_c(x, y); break;
            case 13: draw_num_d(x, y); break;
            case 14: draw_num_e(x, y); break;
            case 15: draw_num_f(x, y); break;
            case 'x': draw_num_x(x, y); break;
            default: draw_num_err(x, y); break;
        }
    }

    void draw_hex32(uint8_t x, uint8_t y, uint32_t v)
    {
        draw_num(x, y, 0);
        draw_num(x + kW + 1, y, 'x');
        x = x + (kW + 1) * 2;
        for(uint8_t i = 0; i < 8; ++i, v>>=4)
            draw_num(x + (kW + 1) * (7 - i), y, v & 0x0f);
    }
};

#endif