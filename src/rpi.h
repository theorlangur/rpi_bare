#ifndef SPI_H_
#define SPI_H_

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