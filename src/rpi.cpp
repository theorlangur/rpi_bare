#include "rpi.h"
#include "rpi_bare/rpi_cfg_bare.h"
#include "rpi_bare/rpi_spi_bare.h"
#include "display/display_driver.h"

#include <algorithm>

namespace {
    bool init_spi()
    {        
        rpi::spi::Config<rpi::RPiBplus, rpi::RPiBplus::SPI1_Pins>::init();
        rpi::spi::Control<rpi::RPiBplus, rpi::RPiBplus::SPI1_Pins>::set_clock_divider(128);
        rpi::spi::Control<rpi::RPiBplus, rpi::RPiBplus::SPI1_Pins>::configure_all(rpi::spi::Chip::CS2);
        //rpi::spi::Control<rpi::RPiBplus, rpi::RPiBplus::SPI1_Pins>::set_speed_div_from_freq(1900'000);

        int a = std::min(1, 3);
        return true;
    }

    void finish_spi()
    {
        rpi::spi::Config<rpi::RPiBplus, rpi::RPiBplus::SPI1_Pins>::end();
    }
}

namespace rpi {
    bool init()
    {
        if (!rpi::RPiBplus::init())
            return false;
        display::init_gpio_pins();

        if (!init_spi())
            return false;

        return true;
    }

    void finish()
    {
        finish_spi();
        rpi::RPiBplus::close();
    }

    Init::Init(){valid = init();}
    Init::~Init(){finish();}
}