#include "display/display_driver.h"
#include "display/display_render.h"
#include "display/display_font.h"
#include "rpi_bare/rpi_bare.h"
#include "rpi_bare/rpi_cfg_bare.h"
#include "rpi_bare/rpi_timers_bare.h"
#include "rpi_bare/rpi_i2c_bare.h"

#include "display/icons/display_icons_misc.h"

extern "C" void kernel_main()
{
    using RPi = rpi::RPiBplus;
    //using SPI = rpi::RPiBplus::SPI1_Pins;
    //constexpr auto CS = SPI::Chip::CS2;
    using SPI = rpi::RPiBplus::SPI0_Pins;
    constexpr auto CS = SPI::Chip::CS0;
    using Timer = rpi::timers::Sys<RPi>;
    using D = DisplaySH1106<SPI>;

    using I2CPins = rpi::RPiBplus::I2C1_Pins;
    using I2C = rpi::i2c::I2C<RPi, I2CPins>;

    I2C::Init i2cInit;
    uint8_t xxx = 0;
    auto wr = I2C::write(&xxx, 1);
    if (std::holds_alternative<uint32_t>(wr))
    {
        //success
    }

    RPi::Init rpiInit;
    D d;
    //the order here is important
    //1. GPIO init
    d.init_gpio_pins();
    //2. SPI init
    rpi::spi::SPIInit<RPi, SPI> spiInit(CS);
    //3. Display init
    d.init();

    display::font::init();
    display::icons::misc::init();
    //Timer::delay_ms(10000);

    const auto &symTrizub = display::icons::misc::symTrizub;

    using Renderer = display::font::FontRenderer<decltype(d)>;
    Renderer r(d);

    r.clear();
    r.render_symbol({0,0}, symTrizub, {5,2}, {7,9});
    r.draw_char({10,0}, '0');
    r.render_line({0, D::kDisplayHeight / 2}, {D::kDisplayWidth - 1, D::kDisplayHeight / 2});
    r.render_line({D::kDisplayWidth / 2, 0}, {D::kDisplayWidth / 2, D::kDisplayHeight - 1});
    r.render_line({0, 0}, {D::kDisplayWidth - 1, D::kDisplayHeight - 1});
    r.render_line({0, D::kDisplayHeight - 1}, {D::kDisplayWidth - 1, 0});
    r.show();

    for(uint8_t y = 1; y < 8; ++y)
    {
        r.clear_part(0, y - 1, 7, 9);
        r.render_symbol({0,y}, symTrizub, {5,2}, {7,9});
        r.draw_char({10,0}, '0' + y);
        r.show();
        //display::show_part(0, y, 7, 8);
        Timer::delay_ms(3000);
    }
    r.draw_hex({0,0}, (uint32_t)(size_t)&kernel_main);
    r.draw_hex({0,8}, (uint32_t)(size_t)&symTrizub);
    r.draw_hex({0,16}, (uint32_t)(size_t)&d);
    r.draw_str({0,24}, "Hello World!");
    r.show();
    Timer::delay_ms(20000);

    uint8_t x = 64, y = 32;

    r.clear();
    r.show();
    Timer::delay_ms(2000);

    bool keyUp = false, keyDown = false, keyRight = false, keyLeft = false;

    display::tools::Point pos{2, 0}, prev;
    int8_t dx = 1, dy = 1;
    prev = pos;

    while(true)
    {
        Timer::delay_ms(5);
        if (d.is_pressed(D::Pins::KEY1))
        {
            break;
        }

        r.clear_part(prev.x, prev.y, symTrizub.size.w, symTrizub.size.h);
        r.render_symbol(pos, symTrizub);
        r.show_part(pos.x, pos.y, symTrizub.size.w, symTrizub.size.h);
        if ((dx > 0 && ((pos.x + dx) + symTrizub.size.w >= D::kDisplayWidth)) || ((dx < 0) && !pos.x))
            dx = -dx;

        if (((pos.y + dy) + symTrizub.size.h >= D::kDisplayHeight) || ((dy < 0) && !pos.y))
            dy = -dy;

        prev = pos;
        pos.x += dx;
        pos.y += dy;
    }
}

#if !defined(PI_BARE) || defined(PI_BARE_FAKE)
int main()
{
    kernel_main();
    return 0;
}
#endif
