#ifndef RPI_GPIO_BARE_H_
#define RPI_GPIO_BARE_H_

#include "rpi_bare.h"
#include "rpi_timers_bare.h"

namespace rpi
{
    namespace gpio
    {
        template<class RPi>
        inline BARECONSTEXPR volatile uint32_t* fsel_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_gpio + RPi::off_gpio_fsel0); }
        template<class RPi>
        inline BARECONSTEXPR volatile uint32_t* set_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_gpio + RPi::off_gpio_set0); }
        template<class RPi>
        inline BARECONSTEXPR volatile uint32_t* clr_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_gpio + RPi::off_gpio_clr0); }
        template<class RPi>
        inline BARECONSTEXPR volatile uint32_t* lev_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_gpio + RPi::off_gpio_lev0); }
        template<class RPi>
        inline BARECONSTEXPR volatile uint32_t* pud_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_gpio + RPi::off_gpio_pud); }
        template<class RPi>
        inline BARECONSTEXPR volatile uint32_t* pudclk_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_gpio + RPi::off_gpio_pudclk); }


        template<auto gpio, class RPi>
        struct Pin
        {
            static void set_pud(PUD p)
            {
                auto addrpud = pud_base_addr<RPi>();
                auto addrpudclk = pudclk_base_addr<RPi>() + (uint32_t(gpio) >> 5);
                *addrpud = (uint32_t)p;
                //tools::delay_at_least_cycles(150);
                rpi::timers::Sys<RPi>::delay_microseconds(10);
                *addrpudclk = tools::set_bits<uint32_t(gpio) % 32, 1>(0, 1);
                rpi::timers::Sys<RPi>::delay_microseconds(10);
                //tools::delay_at_least_cycles(150);
                *addrpud = 0;
                *addrpudclk = 0;
            }

            static void select(F f)
            {
                constexpr uint32_t kFuncMask = 0b111;
                auto addr = fsel_base_addr<RPi>() + uint8_t(gpio) / 10;
                constexpr uint8_t shift = (uint8_t(gpio) % 10) * 3;
                *addr = (*addr & ~(kFuncMask << shift)) | (uint32_t(f) << shift);
            }

            static void set()
            {
                tools::mem_barrier m;
                auto addr = set_base_addr<RPi>() + (uint8_t(gpio) >> 5);
                constexpr uint8_t shift = uint8_t(gpio) & 0b11111;
                *addr = 1 << shift;
            }

            static void clr()
            {
                tools::mem_barrier m;
                auto addr = clr_base_addr<RPi>() + (uint8_t(gpio) >> 5);
                constexpr uint8_t shift = uint8_t(gpio) & 0b11111;
                *addr = 1 << shift;
            }
            
            static bool get()
            {
                auto addr = lev_base_addr<RPi>() + (uint8_t(gpio) >> 5);
                constexpr uint8_t shift = uint8_t(gpio) & 0b11111;
                return (*addr >> shift) & 1;
            }
        };

        template<class RPi>
        struct PinRT
        {            
            uint8_t gpio;
            template<class T>
            PinRT(T g): gpio(uint8_t(g)){}

            void set_pud(PUD p)
            {
                tools::mem_barrier m;
                auto addrpud = pud_base_addr<RPi>();
                auto addrpudclk = pudclk_base_addr<RPi>() + (gpio >> 5);
                *addrpud = (uint32_t)p;
                //tools::delay_at_least_cycles(150);
                rpi::timers::Sys<RPi>::delay_microseconds(10);
                __sync_synchronize();
                *addrpudclk = tools::set_bits<1>(gpio % 32, uint32_t(0), 1);
                rpi::timers::Sys<RPi>::delay_microseconds(10);
                //tools::delay_at_least_cycles(150);
                __sync_synchronize();
                *addrpud = 0;
                *addrpudclk = 0;
            }

            void select(F f)
            {
                tools::mem_barrier m;
                constexpr uint32_t kFuncMask = 0b111;
                auto addr = fsel_base_addr<RPi>() + gpio / 10;
                const uint8_t shift = (gpio % 10) * 3;
                *addr = (*addr & ~(kFuncMask << shift)) | (uint32_t(f) << shift);
            }

            void set(bool v)
            {
                if (v)
                    set();
                else
                    clr();
            }

            void set()
            {
                tools::mem_barrier m;
                auto addr = set_base_addr<RPi>() + (gpio >> 5);
                const uint8_t shift = gpio & 0b11111;
                *addr = 1 << shift;
            }

            void clr()
            {
                tools::mem_barrier m;
                auto addr = clr_base_addr<RPi>() + (gpio >> 5);
                const uint8_t shift = gpio & 0b11111;
                *addr = 1 << shift;
            }
            
            bool get()
            {
                auto addr = lev_base_addr<RPi>() + (gpio >> 5);
                const uint8_t shift = gpio & 0b11111;
                return (*addr >> shift) & 1;
            }
        };
    }
}

#endif