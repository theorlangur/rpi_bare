#include "display/display_driver.h"
#include "display/display_render.h"
#include "display/display_font.h"
#include "rpi_bare/rpi_bare.h"
#include "rpi_bare/rpi_cfg_bare.h"
#include "rpi_bare/rpi_timers_bare.h"
#include "rpi_bare/rpi_i2c_bare.h"

#include "tools/formatter.h"
#include "rpi_bare/rpi_i2c_fmt.h"

#include "display/icons/display_icons_misc.h"
#include "display/display_formatter.h"
#include "drivers/i2c/ads1115.h"

#if defined(PI_BARE_FAKE)
#include <stdio.h>
#include <string>

struct ConsoleOutput
{
    display::tools::Point p;
    void operator()(char c)
    {
        char b[2];
        b[0] = c;
        b[1] = 0;
        printf("%s", b);
    }

    void operator()(std::string_view const& sv)
    {
        std::string s(sv);
        printf("%s", s.c_str());
        //puts(s.c_str());
    }
};
#endif

using RPi = rpi::RPiBplus;
using I2CPins = rpi::RPiBplus::I2C1_Pins;
using I2C = rpi::i2c::I2C<RPi, I2CPins>;
using SPI = rpi::RPiBplus::SPI0_Pins;
constexpr auto CS = SPI::Chip::CS0;
using Timer = rpi::timers::Sys<RPi>;
using D = DisplaySH1106<SPI>;

template<tools::FormatDestination Dest>
std::expected<size_t, tools::FormatError> format_value_to(Dest &&dst, std::string_view const& fmtStr, ADS1115<I2C>::Error const& e)
{
    return tools::format_to(std::forward<Dest>(dst), "ADC:{};I2C:{}", e.code, e.i2c_error.code);
}

extern "C" void kernel_main()
{
    //using SPI = rpi::RPiBplus::SPI1_Pins;
    //constexpr auto CS = SPI::Chip::CS2;


    /*I2C::Init i2cInit;
    uint8_t xxx = 0;
    auto wr = I2C::write(&xxx, 1);
    if (wr.has_value())
    {
        //success
    }*/

    Timer::TimeTest ttRpiInit;
    RPi::Init rpiInit;
    ttRpiInit.mark();
    Timer::TimeTest ttCostructDisplayInit;
    D d;
    ttCostructDisplayInit.mark();
    //the order here is important
    //1. GPIO init
    Timer::TimeTest ttDisplayGPIOInit;
    d.init_gpio_pins();
    ttDisplayGPIOInit.mark();
    //2. SPI init
    Timer::TimeTest ttSPIInit;
    rpi::spi::SPIInit<RPi, SPI> spiInit(CS);
    ttSPIInit.mark();
    //3. Display init
    Timer::TimeTest ttDisplayInit;
#if !defined(PI_BARE_FAKE)
    d.init();
#endif
    ttDisplayInit.mark();

    Timer::TimeTest ttFontsInit;
    display::font::init();
    ttFontsInit.mark();
    Timer::TimeTest ttIconsInit;
    display::icons::misc::init();
    ttIconsInit.mark();
    //Timer::delay_ms(10000);

    const auto &symTrizub = display::icons::misc::symTrizub;

    using Renderer = display::font::FontRenderer<decltype(d)>;
    Renderer r(d);

    auto to_display = display::DisplayFormatter{r, {0,0}};
    r.clear();
    {
        I2C::Init i2cInit;
        ADS1115<I2C> ads1115;
        if (ads1115.exists())
        {
            tools::format_to(to_display, "ADS:\n");
            {
                tools::format_to(to_display, "Cfg:{:x}\n", ads1115.get_config());
                r.show();
                Timer::delay_ms(2000);
            }
            {
                tools::format_to(to_display, "Lo:{:x}\n", ads1115.get_lo_threshold());
                r.show();
                Timer::delay_ms(2000);
            }
            {
                tools::format_to(to_display, "Hi:{:x}\n", ads1115.get_hi_threshold());
                r.show();
                Timer::delay_ms(2000);
            }
            float t = 0.5f;
            for(int i = 0; i < 20; ++i)
            {
                r.clear();
                to_display.p = {0,0};
                //tools::format_to(to_display, "ADS({}):{}", i, ads1115.read_single_raw());
                //t *= to_display.p.x;
                //if (to_display.p.y == 0)
                    //t /= to_display.p.y + 2;
                //tools::format_to(to_display, "\nfloat:{}", t);
                tools::format_to(to_display, "ADS({}):{}", i, ads1115.read_single());
                r.show();
                Timer::delay_ms(2000);
            }
            tools::format_to(to_display, "T:{}", t);
        }else
        {
            //tools::format_to(to_display, "Test: {}\n", 1.5f);
            tools::format_to(to_display, "Didn't find ADS\nat {}", (uint8_t)ads1115.get_addr());
        }
    }
    tools::format_to(to_display, "\nFinished");
    r.show();
    Timer::delay_ms(20000);

    r.clear();
    Timer::TimeTest ttPrint;
     /*r.print_all({0,0}
            , "Init:\n",
            "RPI:",ttRpiInit.measured(),
            "\nDispCTR:",ttCostructDisplayInit.measured(),
            "\nDispGPIO:", ttDisplayGPIOInit.measured(),
            "\nSPI:",ttSPIInit.measured(),
            "\nDisp:",ttDisplayInit.measured(),
            "\nFonts:",ttFontsInit.measured(),
            "\nIcons:",ttIconsInit.measured()
            );
            */
    auto syms = tools::format_to(to_display, 
    "Init:"
    "\nRPI:{}"
    "\nDispCTR:{:x}"
    "\nDispGPIO:{}"
    "\nSPI:{:x}"
    "\nDisp:{}"
    "\nFonts:{:x}"
    "\nIcons:{}"
    , ttRpiInit.measured()
    , (uint16_t)ttCostructDisplayInit.measured()
    , ttDisplayGPIOInit.measured()
    , (uint8_t)ttSPIInit.measured()
    , ttDisplayInit.measured()
    , (uint16_t)ttFontsInit.measured()
    , ttIconsInit.measured()
    );
    ttPrint.mark();
    /*r.render_symbol({0,0}, symTrizub, {5,2}, {7,9});
    r.draw_char({10,0}, '0');
    r.render_line({0, D::kDisplayHeight / 2}, {D::kDisplayWidth - 1, D::kDisplayHeight / 2});
    r.render_line({D::kDisplayWidth / 2, 0}, {D::kDisplayWidth / 2, D::kDisplayHeight - 1});
    r.render_line({0, 0}, {D::kDisplayWidth - 1, D::kDisplayHeight - 1});
    r.render_line({0, D::kDisplayHeight - 1}, {D::kDisplayWidth - 1, 0});
    */
    r.show();
    Timer::delay_ms(20000);
    r.clear();
    to_display.p = {0,0};
    tools::format_to(to_display, "Prev printing took:\n{:x}us\n{} symbols printed", (uint16_t)ttPrint.measured(), syms);
    //r.print_all({0,0} , "Previous printing took:\n" ,ttPrint.measured(), " us");
    r.show();
    Timer::delay_ms(20000);

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
