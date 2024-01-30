#ifndef RPI_CFG_BARE_H_
#define RPI_CFG_BARE_H_

#include "rpi_bare.h"

#ifndef PI_BARE
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace rpi
{
    struct RPiBplus
    {
    #ifdef PI_BARE
        static constexpr uint32_t io_base_addr     = 0x20000000;
    #else
        static constexpr uint32_t bare_io_base_addr     = 0x20000000;
        static constexpr uint32_t bare_io_size          = 0x01000000;
        static inline uint8_t *io_base_addr        = nullptr;
    #endif
        static constexpr uint32_t off_gpio         = 0x00200000;
        static constexpr uint32_t off_gpio_fsel0   = 0x0;
        static constexpr uint32_t off_gpio_set0    = 0x1c;
        static constexpr uint32_t off_gpio_clr0    = 0x28;
        static constexpr uint32_t off_gpio_lev0    = 0x34;
        static constexpr uint32_t off_gpio_pud     = 0x94;
        static constexpr uint32_t off_gpio_pudclk  = 0x98;

        static constexpr uint32_t off_spi0       = 0x00204000;
        static constexpr uint32_t off_spi0_cs    = 0x00;
        static constexpr uint32_t off_spi0_fifo  = 0x04;
        static constexpr uint32_t off_spi0_clk   = 0x08;
        static constexpr uint32_t off_spi0_dlen  = 0x0c;
        static constexpr uint32_t off_spi0_ltoh  = 0x10;
        static constexpr uint32_t off_spi0_dc    = 0x14;

        static constexpr uint32_t off_aux         = 0x00215000;
        static constexpr uint32_t off_aux_enables = 0x00000004;

        static constexpr uint32_t off_aux_spi1_cntl0 = 0x00000080;
        static constexpr uint32_t off_aux_spi1_cntl1 = 0x00000084;
        static constexpr uint32_t off_aux_spi1_stat  = 0x00000088;
        static constexpr uint32_t off_aux_spi1_peek  = 0x0000008c;
        static constexpr uint32_t off_aux_spi1_io    = 0x000000a0;
        static constexpr uint32_t off_aux_spi1_txhold= 0x000000b0;

        static constexpr uint32_t off_sys_timer     = 0x00003000;
        static constexpr uint32_t off_sys_timer_cs  = 0x00000000;
        static constexpr uint32_t off_sys_timer_clo = 0x00000004;
        static constexpr uint32_t off_sys_timer_chi = 0x00000008;

        static constexpr uint32_t g_SysFreqHz = 250'000'000;//250MHz

        struct SPI0_Pins
        {
            enum E : uint8_t
            {
                CE1_N = 7,
                CE0_N = 8,
                MISO = 9,
                MOSI = 10,
                SCLK = 11,
            };
            constexpr static gpio::F func = gpio::F::F0;

            enum class Chip : uint32_t
            {
                CS0 = 0,
                CS1 = 1,
                //Reserved
            };
        };


        struct SPI1_Pins
        {
            enum E : uint8_t
            {
                CE2_N = 16,
                CE1_N = 17,
                CE0_N = 18,
                MISO = 19,
                MOSI = 20,
                SCLK = 21,
            };
            constexpr static gpio::F func = gpio::F::F4;

            enum class Chip : uint32_t
            {
                CS0 = 0,
                CS1 = 1,
                CS2 = 2,
                //Reserved
            };
        };

        static bool init()
        {
#ifndef PI_BARE
            int memfd;
            if ((memfd = open("/dev/mem", O_RDWR | O_SYNC) ) < 0) 
            {
                //fprintf(stderr, "bcm2835_init: Unable to open /dev/mem: %s\n",
                //    strerror(errno)) ;
                return false;
            }
            auto closeMemFd = tools::ScopeExit([memfd]{::close(memfd);});
            io_base_addr = (uint8_t*)mmap(NULL, bare_io_size, (PROT_READ|PROT_WRITE), MAP_SHARED, memfd, bare_io_base_addr);
            if (io_base_addr == MAP_FAILED) 
            {
                io_base_addr = nullptr;
                return false;
            }
#endif
            return true;
        }

        static void close()
        {
#ifndef PI_BARE
            if (io_base_addr != nullptr)
            {
                munmap(io_base_addr, bare_io_base_addr);
                io_base_addr = nullptr;
            }
#endif
        }

        struct Init
        {
            Init()
            {
                init();
            }
            ~Init()
            {
                close();
            }
        };
    };

    namespace gpio
    {
        template<auto pin, class RPi>
        struct Pin;

        template<auto pin>
        using PinBplus = Pin<pin, rpi::RPiBplus>;
    }
}

#endif
