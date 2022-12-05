#include "rpi.h"
#include "rpi_bare/rpi_cfg_bare.h"
#include "rpi_bare/rpi_spi_bare.h"
#include "display_driver.h"

namespace {
    bool init_spi()
    {        
        rpi::spi::Config<rpi::RPiBplus>::init();
        rpi::spi::Control<rpi::RPiBplus>::configure_all();
        rpi::spi::Control<rpi::RPiBplus>::set_clock_divider(128);
        /*
        if (bcm2835_spi_begin())                                         //Start spi interface, set spi pin for the reuse function
        {
            bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);     //High first transmission
            bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                  //spi mode 0
            bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128);  //Frequency
            bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                     //set CE0
            bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);     //enable cs0
            return true;
        }
        return false;
        */
        return true;
    }

    void finish_spi()
    {
        rpi::spi::Config<rpi::RPiBplus>::end();
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