#include "display_driver.h"
#include "../rpi_bare/rpi_cfg_bare.h"
#include "../rpi_bare/rpi_gpio_bare.h"
#include "../rpi_bare/rpi_spi_bare.h"
#include "../rpi_bare/rpi_timers_bare.h"

namespace display {
    template<auto P>
    using RPiPin = rpi::gpio::PinBplus<P>;
    using Timer = rpi::timers::Sys<rpi::RPiBplus>;
    using PinRT = rpi::gpio::PinRT<rpi::RPiBplus>;
    using SPI = rpi::spi::Transfer<rpi::RPiBplus>;

    uint8_t DisplayMemory[kDisplayMemorySize]={0};

    Init::Init(){valid = init();}
    Init::~Init(){finish();}

    void init_gpio_pins()
    {
        RPiPin<Pins::OLED_RST>::select(rpi::gpio::F::Out);
        RPiPin<Pins::OLED_DC>::select(rpi::gpio::F::Out);
        RPiPin<Pins::OLED_CS>::select(rpi::gpio::F::Out);

        RPiPin<Pins::KEY_UP>::select(rpi::gpio::F::In);
        RPiPin<Pins::KEY_UP>::set_pud(rpi::gpio::PUD::PullUp);

        RPiPin<Pins::KEY_DOWN>::select(rpi::gpio::F::In);
        RPiPin<Pins::KEY_DOWN>::set_pud(rpi::gpio::PUD::PullUp);

        RPiPin<Pins::KEY_LEFT>::select(rpi::gpio::F::In);
        RPiPin<Pins::KEY_LEFT>::set_pud(rpi::gpio::PUD::PullUp);

        RPiPin<Pins::KEY_RIGHT>::select(rpi::gpio::F::In);
        RPiPin<Pins::KEY_RIGHT>::set_pud(rpi::gpio::PUD::PullUp);

        RPiPin<Pins::KEY_PRESS>::select(rpi::gpio::F::In);

        RPiPin<Pins::KEY1>::select(rpi::gpio::F::In);
        RPiPin<Pins::KEY1>::set_pud(rpi::gpio::PUD::PullUp);

        RPiPin<Pins::KEY2>::select(rpi::gpio::F::In);
        RPiPin<Pins::KEY2>::set_pud(rpi::gpio::PUD::PullUp);

        RPiPin<Pins::KEY3>::select(rpi::gpio::F::In);
        RPiPin<Pins::KEY3>::set_pud(rpi::gpio::PUD::PullUp);
    }

    void config()
    {
        switch_oled(false);//--turn off oled panel
        set_column(kWidthPadding);
        send_command(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
        send_command(0x81);//--set contrast control register
        send_command(0xA0);//--Set SEG/Column Mapping
        send_command(0xC0);//Set COM/Row Scan Direction
        send_command(0xA6);//--set normal display
        send_command(0xA8);//--set multiplex ratio(1 to 64)
        send_command(0x3F);//--1/64 duty
        send_command(0xD3);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
        send_command(0x00);//-not offset
        send_command(0xd5);//--set display clock divide ratio/oscillator frequency
        send_command(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
        send_command(0xD9);//--set pre-charge period
        send_command(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
        send_command(0xDA);//--set com pins hardware configuration
        send_command(0x12);
        send_command(0xDB);//--set vcomh
        send_command(0x40);//Set VCOM Deselect Level
        send_command(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
        send_command(0x02);//
        send_command(0xA4);// Disable Entire Display On (0xa4/0xa5)
        send_command(0xA6);// Disable Inverse Display On (0xa6/a7)
    }

    bool init()
    {
        //init_gpio_pins();
        reset();
        config();
        Timer::delay_ms(200);
        switch_oled(true);
        return true;
    }

    void operator <<(Pins p, bool v)
    {
        PinRT(p).set(v);
    }

    bool operator >>(Pins p, bool &v)
    {
        bool prev = v;
        v = PinRT(p).get();
        return v != prev;
    }

    void reset()
    {
        Pins::OLED_RST << true;
        Timer::delay_ms(100);
        Pins::OLED_RST << false;
        Timer::delay_ms(100);
        Pins::OLED_RST << true;
        Timer::delay_ms(100);
    }

    void finish()
    {

    }

    bool is_pressed(Pins pin)
    {
        return PinRT(pin).get() == false;
    }

    void switch_oled(bool on)
    {
        send_command(0xAE | on);
    }

    void set_page(uint8_t page)
    {
        send_command(0xB0 | page);
    }

    void set_column(uint8_t column)
    {
        send_command(0x00 | (column & 0x0f));
        send_command(0x10 | (column >> 4));
    }

    void send_command(uint8_t v)
    {
        Pins::OLED_DC << false;
        SPI::transfer_byte(v);
    }

    void send_data(uint8_t v)
    {
        Pins::OLED_DC << true;
        SPI::transfer_byte(v);
    }

    void send_data(uint8_t *pData, size_t s)
    {
        Pins::OLED_DC << true;
        //SPI::send(pData, s);
        for(size_t i = 0; i < s; ++i)
            SPI::transfer_byte(pData[i]);
    }

    void set_point(uint8_t x, uint8_t y, bool on)
    {
        auto &b = DisplayMemory[x + (y/8)*kDisplayWidth];
        if (on) b |= 1 << (y % 8);
        else b &= ~(1 << (y % 8));
    }

    void set_bitmap(uint8_t x, uint8_t y, const uint8_t *pData, uint8_t w, uint8_t h)
    {
        uint8_t r = y / 8;
        uint8_t endX = x + w;
        if (endX > kDisplayWidth) endX = kDisplayWidth;
        uint8_t endY = y + h;
        if (endY > kDisplayHeight) endY = kDisplayHeight;
        for(uint8_t iy = y; iy < endY; iy = ((iy + 8) & 0b11111000) )
        {
            uint8_t mask = ~((1 << (iy % 8)) - 1);
            if (!(iy % 8) && ((iy + 8) > endY))
                mask &= (1 << (endY % 8)) - 1;
            uint8_t off = iy - y;
            const uint8_t offRest8 = off % 8;
            for(uint8_t ix = x; ix < endX; ++ix)
            {
                auto &b = DisplayMemory[ix + (iy/8)*kDisplayWidth];
                b &= ~mask;//clear target bits
                uint8_t src1 = pData[(ix - x) + (off/8)*w];
                src1 >>= offRest8;
                src1 <<= iy % 8;
                b |= mask & src1;

                uint8_t nextOff = (off + 8) & 0b11111000;
                if (offRest8 && (nextOff <= h))
                {
                    src1 = pData[(ix - x) + (nextOff/8)*w];
                    uint8_t bit = offRest8;
                    src1 &= (1 << bit) - 1;
                    src1 <<= 8 - bit;
                    b |= mask & src1;
                }
            }
        }
    }

    void set_bitmap_sub(uint8_t x, uint8_t y, const uint8_t *pData, uint8_t w, uint8_t h, uint8_t off_x, uint8_t off_y, uint8_t sub_w, uint8_t sub_h)
    {
        if (off_x >= w) return;
        if (off_y >= h) return;

        if (off_x == 0 && off_y == 0 && sub_w == w && sub_h == h)
            return set_bitmap(x, y, pData, w, h);

        if ((off_x + sub_w) > w) sub_w = w - off_x;
        if ((off_y + sub_h) > h) sub_h = h - off_y;

        uint8_t r = y / 8;
        uint8_t endX = x + sub_w;
        if (endX > kDisplayWidth) endX = kDisplayWidth;
        uint8_t endY = y + sub_h;
        if (endY > kDisplayHeight) endY = kDisplayHeight;
        bool combinedSrc = (off_y % 8) > 0;
        const uint8_t offYRow = off_y / 8;
        const uint8_t remOffY = off_y % 8;
        const uint8_t invRemOffY = 8 - remOffY;
        const uint8_t srcEnd = off_y + sub_h;

        auto getSrcByte = [&](uint8_t x, uint8_t y)
        {
            uint8_t src1 = pData[x + (offYRow + (y/8))*w];
            if (combinedSrc)
            {
                src1 >>= remOffY;
                const uint8_t nextByte = (y/8 + offYRow + 1);
                const uint8_t next8Bits = nextByte*8;
                if (next8Bits < h)
                {
                    src1 |= pData[x + nextByte*w] << invRemOffY;
                    if ((next8Bits + remOffY) > srcEnd)
                    {
                        uint8_t extraBits = (next8Bits + remOffY) - srcEnd;
                        src1 <<= extraBits;
                        src1 >>= extraBits;
                    }
                }
            }
            return src1;
        };
        for(uint8_t iy = y; iy < endY; iy = ((iy + 8) & 0b11111000) )
        {
            uint8_t mask = ~((1 << (iy % 8)) - 1);
            if (!(iy % 8) && ((iy + 8) > endY))
                mask &= (1 << (endY % 8)) - 1;
            uint8_t off = iy - y;
            const uint8_t offRest8 = off % 8;
            for(uint8_t ix = x; ix < endX; ++ix)
            {
                const uint8_t srcOffX = ix - x + off_x;
                auto &b = DisplayMemory[ix + (iy/8)*kDisplayWidth];
                b &= ~mask;//clear target bits
                uint8_t src1 = getSrcByte(srcOffX, off);
                src1 >>= offRest8;
                src1 <<= iy % 8;
                b |= mask & src1;

                uint8_t nextOff = (off + 8) & 0b11111000;
                if (offRest8 && (nextOff < sub_h))
                {
                    src1 = getSrcByte(srcOffX, nextOff);
                    uint8_t bit = offRest8;
                    src1 &= (1 << bit) - 1;
                    src1 <<= 8 - bit;
                    b |= mask & src1;
                }
            }
        }
    }

    void show()
    {
        uint8_t *pData = DisplayMemory;
        for(uint8_t page = 0; page < 8; ++page)
        {
            set_page(page);
            set_column(kWidthPadding);

            send_data(pData, kDisplayWidth);
            pData += kDisplayWidth;
        }
    }

    void show_part(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
    {
        uint8_t *pData = DisplayMemory;
        uint8_t page_max = (y + h + 7) / 8;
        if (page_max > 8) page_max = 8;
        uint8_t end_column = x + w;
        if (end_column > kDisplayWidth) end_column = kDisplayWidth;
        pData += kDisplayWidth * (y / 8);
        for(uint8_t page = y / 8; page < page_max; ++page)
        {
            set_page(page);
            set_column(kWidthPadding + x);

            send_data(pData + x, end_column - x);
            pData += kDisplayWidth;
        }
    }

    void clear_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
    {
        uint8_t r = y / 8;
        uint8_t endX = x + w;
        if (endX > kDisplayWidth) endX = kDisplayWidth;
        uint8_t endY = y + h;
        if (endY > kDisplayHeight) endY = kDisplayHeight;
        for(uint8_t iy = y; iy < endY; iy = ((iy + 8) & 0b11111000) )
        {
            uint8_t mask = ~((1 << (iy % 8)) - 1);
            if (!(iy % 8) && ((iy + 8) > endY))
                mask &= (1 << (endY % 8)) - 1;
            for(uint8_t ix = x; ix < endX; ++ix)
            {
                auto &b = DisplayMemory[ix + (iy/8)*kDisplayWidth];
                b &= ~mask;//clear target bits
            }
        }
    }

    void clear_part(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
    {
        clear_rect(x, y, w, h);
        show_part(x, y, w, h);
    }

    void clear()
    {
        for(auto& m : DisplayMemory)
            m = 0;
    }
}