#ifndef SPI_H_
#define SPI_H_

#ifndef SPI_TO_USE
#define SPI_TO_USE rpi::RPiBplus::SPI0_Pins
#endif

namespace rpi
{
    bool init();
    void finish();

    struct Init
    {
        Init();
        ~Init();

        operator bool() const { return valid; }
    private:
        bool valid;
    };
}

#endif