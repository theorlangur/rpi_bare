//#include "display_driver.h"
//#include "../rpi_cfg.h"
#include "../rpi_bare/rpi_cfg_bare.h"
#include "../rpi_bare/rpi_gpio_bare.h"
#include "../rpi_bare/rpi_spi_bare.h"
#include "../rpi_bare/rpi_timers_bare.h"

namespace display {
    template<auto P>
    using RPiPin = rpi::gpio::PinBplus<P>;
    using Timer = rpi::timers::Sys<rpi::RPiBplus>;
    using PinRT = rpi::gpio::PinRT<rpi::RPiBplus>;
    using SPI = rpi::spi::Transfer<rpi::RPiBplus, SPI_TO_USE>;

    uint8_t DisplayMemory[kDisplayMemorySize]={0};

    Init::Init(){valid = init();}
    Init::~Init(){finish();}
}
