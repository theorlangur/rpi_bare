#include "rpi.h"
#include "rpi_cfg.h"
#include "rpi_bare/rpi_cfg_bare.h"
#include "rpi_bare/rpi_spi_bare.h"
#include "display/display_driver.h"

//#include <algorithm>

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
