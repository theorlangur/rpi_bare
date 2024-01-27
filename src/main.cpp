#include "rpi.h"
#include "rpi_cfg.h"
#include "display/display_driver.h"
#include "display/display_render.h"
#include "display/display_font.h"
#include "rpi_bare/rpi_bare.h"
#include "rpi_bare/rpi_cfg_bare.h"
#include "rpi_bare/rpi_timers_bare.h"

#include "display/icons/display_icons_misc.h"

extern "C" void kernel_main()
{
    using Timer = rpi::timers::Sys<rpi::RPiBplus>;
    rpi::Init initRpi;
    if (!initRpi)
        return;
    display::Init initDisplay;
    if (!initDisplay)
        return;

    display::font::init();
    display::icons::misc::init();
    //Timer::delay_ms(10000);

    const auto &symTrizub = display::icons::misc::symTrizub;

    display::clear();
    display::render_symbol({0,0}, symTrizub, {5,2}, {7,9});
    display::font::draw_char({10,0}, '0');
    display::render_line({0, display::kDisplayHeight / 2}, {display::kDisplayWidth - 1, display::kDisplayHeight / 2});
    display::render_line({display::kDisplayWidth / 2, 0}, {display::kDisplayWidth / 2, display::kDisplayHeight - 1});
    display::render_line({0, 0}, {display::kDisplayWidth - 1, display::kDisplayHeight - 1});
    display::render_line({0, display::kDisplayHeight - 1}, {display::kDisplayWidth - 1, 0});
    display::show();
    Timer::delay_ms(10000);
    for(uint8_t y = 1; y < 8; ++y)
    {
        display::clear_part(0, y - 1, 7, 9);
        display::render_symbol({0,y}, symTrizub, {5,2}, {7,9});
        display::font::draw_char({10,0}, '0' + y);
        display::show();
        //display::show_part(0, y, 7, 8);
        Timer::delay_ms(10000);
    }
    display::font::draw_hex({0,0}, (uint32_t)&kernel_main);
    display::font::draw_hex({0,8}, (uint32_t)&symTrizub);
    display::font::draw_hex({0,16}, (uint32_t)&display::DisplayMemory);
    display::font::draw_str({0,24}, "Hello World!");
    display::show();
    Timer::delay_ms(20000);

    uint8_t x = 64, y = 32;

    display::clear();
    display::show();
    Timer::delay_ms(2000);

    bool keyUp = false, keyDown = false, keyRight = false, keyLeft = false;

    display::tools::Point pos{2, 0}, prev;
    int8_t dx = 1, dy = 1;
    prev = pos;

    while(true)
    {
        Timer::delay_ms(5);
        if (display::is_pressed(display::Pins::KEY1))
        {
            break;
        }

        display::clear_part(prev.x, prev.y, symTrizub.size.w, symTrizub.size.h);
        display::render_symbol(pos, symTrizub);
        display::show_part(pos.x, pos.y, symTrizub.size.w, symTrizub.size.h);
        if ((dx > 0 && ((pos.x + dx) + symTrizub.size.w >= display::kDisplayWidth)) || ((dx < 0) && !pos.x))
            dx = -dx;

        if (((pos.y + dy) + symTrizub.size.h >= display::kDisplayHeight) || ((dy < 0) && !pos.y))
            dy = -dy;

        prev = pos;
        pos.x += dx;
        pos.y += dy;
    }
}

#ifndef PI_BARE
int main()
{
    kernel_main();
    return 0;
}
#endif