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
        /*
        ADS1115<I2C> ads1115;
        if (ads1115.exists())
        {
            tools::format_to(to_display, "ADS:\n");
            {
                tools::format_to(to_display, "Cnv:{:x}\n", ads1115.get_conv_dbg());
                r.show();
                Timer::delay_ms(2000);
            }
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
            {
                if (auto res = ads1115.set_hi_threshold(1234))
                    tools::format_to(to_display, "New Hi:{:x}\n", ads1115.get_hi_threshold());
                else
                    tools::format_to(to_display, "Failed hi:{}\n", res);

                r.show();
                Timer::delay_ms(2000);
            }
            Timer::delay_ms(10000);
            for(int i = 0; i < 20; ++i)
            {
                r.clear();
                to_display.p = {0,0};
                tools::format_to(to_display, "ADS({}):{}", i, ads1115.read_single_raw());
                r.show();
                Timer::delay_ms(2000);
            }
        }else
        {
            tools::format_to(to_display, "Didn't find ADS\nat {}", (uint8_t)ads1115.get_addr());
        }
        */
        I2C::Device ads1115(0x48);
        auto c = ads1115.communicate();
        //tools::format_to(to_display, "Reading regs\n");
        //for(int i = 0; i < 4; ++i)
        //{
        //    if (auto res = c.write(uint8_t(i)))
        //    {
        //        tools::format_to(to_display, "{}:{:x}\n", i, c.read<uint16_t>());
        //        r.show();
        //        Timer::delay_ms(2000);
        //    }else
        //    {
        //        tools::format_to(to_display, "Failed: {}", res);
        //        break;
        //    }
        //}
        //r.show();
        //Timer::delay_ms(10000);

        uint8_t reg = 0x01;//config
        tools::format_to(to_display, "Preparing...\n");
        r.show();
        Timer::delay_ms(2000);

        {
            tools::format_to(to_display, "Hi:{:x}\n", c.read_from_reg<uint16_t>(0b11));
            r.show();
            Timer::delay_ms(2000);
        }
        {
            if (auto res = c.write_to_reg_i16(0b11, 1234))
            {
                tools::format_to(to_display, "Hi changed\n");
                r.show();
                Timer::delay_ms(2000);
                tools::format_to(to_display, "New Hi:{:x}\n", c.read_from_reg<uint16_t>(0b11));
                r.show();
                Timer::delay_ms(2000);
            }else
            {
                tools::format_to(to_display, "Hi W:{}\n", res);
                r.show();
                Timer::delay_ms(2000);
            }
            Timer::delay_ms(20000);
        }

        ADS1115<I2C>::Config cfg;
        cfg.m_bits.conversion = 1;
        cfg.m_bits.mode = ADS1115<I2C>::Config::Mode::SingleShot;
        uint8_t buf[4];
        buf[0] = reg;
        buf[1] = uint8_t(cfg.m_dw >> 8);
        buf[2] = uint8_t(cfg.m_dw & 0xff);
        buf[3] = reg;
        tools::format_to(to_display, "Cfg bytes:{}\n", buf);
        r.show();
        Timer::delay_ms(2000);
        if (auto res = c.write(buf, 4); res || res.error().transferred >= 3)
        {
            tools::format_to(to_display, "config done\n");
            r.show();
            Timer::delay_ms(2000);
            reg = 0x0;
            if (auto res = c.write(&reg, 1))
            {
                tools::format_to(to_display, "changed reg to 0\n");
                r.show();
                Timer::delay_ms(2000);
                if (auto res = c.read<int16_t>())
                    tools::format_to(to_display, "ADC conv:\n{:x}", *res);
                else
                    tools::format_to(to_display, "Failed to read:\n{}", res);
            }
            else
                tools::format_to(to_display, "Failed to\nchange reg");
        }
        else
            tools::format_to(to_display, "Failed to write\nconfig:{}", res);
    }
    tools::format_to(to_display, "\nFinished");
    r.show();
    Timer::delay_ms(20000);
    /*{
        tools::format_to(to_display, "Scanning i2c:");
        auto statusPoint = to_display.p;
        tools::format_to(to_display, "\n");
        r.show();
        statusPoint.x += 2;
        constexpr size_t stride = 4;
        size_t n = 0;
        I2C::Init i2cInit;
        uint8_t dummy = 0;
        for(uint8_t i = 1; i < 127; ++i)
        {
            r.draw_uint(statusPoint, i);
            r.show_part(statusPoint.x,statusPoint.y,28,9);
            I2C::set_slave_addr(i);
            if (I2C::write(nullptr, 0))
            {
                tools::format_to(to_display, "{:x} ", i);
                if ((++n) % stride == 0)
                    tools::format_to(to_display, "\n");
                r.show();
            }
            Timer::delay_ms(200);
        }
        if (n % stride != 0)
            tools::format_to(to_display, "\n");
        tools::format_to(to_display, "Found: {}", n);
        r.show();
        Timer::delay_ms(20000);
    }
    */

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
