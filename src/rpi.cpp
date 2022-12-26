#include "rpi.h"
#include "rpi_bare/rpi_cfg_bare.h"
#include "rpi_bare/rpi_spi_bare.h"
#include "display/display_driver.h"

namespace {
    bool init_spi()
    {        
        rpi::spi::Config<rpi::RPiBplus, rpi::RPiBplus::SPI0_Pins>::init();
        rpi::spi::Control<rpi::RPiBplus, rpi::RPiBplus::SPI0_Pins>::configure_all();
        rpi::spi::Control<rpi::RPiBplus, rpi::RPiBplus::SPI0_Pins>::set_clock_divider(128);
        return true;
    }

    void finish_spi()
    {
        rpi::spi::Config<rpi::RPiBplus, rpi::RPiBplus::SPI0_Pins>::end();
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