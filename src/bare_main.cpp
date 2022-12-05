#include "rpi_bare/rpi_bare.h"
#include "rpi_bare/rpi_spi_bare.h"
#include "rpi_bare/rpi_cfg_bare.h"


extern "C" void kernel_main()
{
    using spi_cfg = rpi::spi::Config<rpi::RPiBplus>;
    using spi_ctrl = rpi::spi::Control<rpi::RPiBplus>;
    using spi_tx = rpi::spi::Transfer<rpi::RPiBplus>;
    spi_cfg::init();
    spi_ctrl::configure_all();

    spi_tx::transfer_byte(0x30);

    spi_cfg::end();
    //rpi::gpio::PinBplus<0>::set_pud(rpi::gpio::PUD::PullUp);
}