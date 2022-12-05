#ifndef RPI_SPI_BARE_H_
#define RPI_SPI_BARE_H_

#include "rpi_gpio_bare.h"

namespace rpi
{
    namespace spi
    {
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* cs_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_spi0 + RPi::off_spi0_cs); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* fifo_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_spi0 + RPi::off_spi0_fifo); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* clk_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_spi0 + RPi::off_spi0_clk); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* dlen_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_spi0 + RPi::off_spi0_dlen); }

        template<class RPi>
        struct Config
        {
            template<auto p>
            using PinT = rpi::gpio::Pin<p, RPi>;
            using pins = typename RPi::SPI0_Pins;

            static void init()
            {
                __sync_synchronize();
                PinT<pins::CE1_N>::select(rpi::gpio::F::F0);
                PinT<pins::CE0_N>::select(rpi::gpio::F::F0);
                PinT<pins::MISO>::select(rpi::gpio::F::F0);
                PinT<pins::MOSI>::select(rpi::gpio::F::F0);
                __sync_synchronize();
                PinT<pins::SCLK>::select(rpi::gpio::F::F0);
            }

            static void end()
            {
                __sync_synchronize();
                PinT<pins::CE1_N>::select(rpi::gpio::F::In);
                PinT<pins::CE0_N>::select(rpi::gpio::F::In);
                PinT<pins::MISO>::select(rpi::gpio::F::In);
                PinT<pins::MOSI>::select(rpi::gpio::F::In);
                __sync_synchronize();
                PinT<pins::SCLK>::select(rpi::gpio::F::In);
            }
        };

        enum class Polarity : uint32_t
        {
            Low = 0,
            High = 1
        };

        enum class ClockPhase : uint32_t
        {
            Middle = 0,
            Begin = 1
        };

        enum class Chip : uint32_t
        {
            CS0 = 0,
            CS1 = 1,
            CS2 = 2,
            //Reserved
        };

        enum class FIFO : uint32_t
        {
            None = 0b00,
            Tx   = 0b01,
            Rx   = 0b10,
            All  = Tx | Rx,
        };

        template<class RPi>
        struct Control
        {
            enum class Bits : uint32_t
            {
                cs     = 0,
                cpha   = 2,   
                cpol   = 3,   
                clear  = 4,
                cspol  = 6,   
                ta     = 7,
                done   = 16,
                rxd    = 17,
                txd    = 18,
                rxr    = 19,
                rxf    = 20,
                cspol0 = 21,
                cspol1 = 22,
                cspol2 = 23,
            };

            template<class E>
            friend uint32_t operator<<(E e, Bits b) { return uint32_t(e) << uint32_t(b); }

            static void configure_all(
                Chip cs = Chip::CS0, 
                ClockPhase ph = ClockPhase::Middle, 
                Polarity clock = Polarity::Low, 
                Polarity chipSelectAll = Polarity::Low,
                Polarity chipSelect0 = Polarity::Low,
                Polarity chipSelect1 = Polarity::Low,
                Polarity chipSelect2 = Polarity::Low
            )
            {
                auto addr = cs_base_addr<RPi>();
                *addr =   cs << Bits::cs
                        | ph << Bits::cpha 
                        | clock << Bits::cpol 
                        | chipSelectAll << Bits::cspol
                        | chipSelect0 << Bits::cspol0
                        | chipSelect1 << Bits::cspol1
                        | chipSelect2 << Bits::cspol2;
            }

            static void clear_fifo(FIFO f)
            {
                auto addr = cs_base_addr<RPi>();
                *addr = rpi::tools::set_bits<Bits::clear, 2>(*addr, f);
            }

            static void set_ta(bool active)
            {
                auto addr = cs_base_addr<RPi>();
                *addr = rpi::tools::set_bits<Bits::ta, 1>(*addr, active);
            }

            static bool done()
            {
                tools::mem_barrier m;
                auto addr = cs_base_addr<RPi>();
                return rpi::tools::get_bits<Bits::done, 1>(*addr);
            }

            static bool can_write()
            {
                tools::mem_barrier m;
                auto addr = cs_base_addr<RPi>();
                return rpi::tools::get_bits<Bits::txd, 1>(*addr);
            }

            static bool can_read()
            {
                tools::mem_barrier m;
                auto addr = cs_base_addr<RPi>();
                return rpi::tools::get_bits<Bits::rxd, 1>(*addr);
            }

            static bool is_full()
            {
                tools::mem_barrier m;
                auto addr = cs_base_addr<RPi>();
                return rpi::tools::get_bits<Bits::rxf, 1>(*addr);
            }

            static bool needs_reading()
            {
                tools::mem_barrier m;
                auto addr = cs_base_addr<RPi>();
                return rpi::tools::get_bits<Bits::rxr, 1>(*addr);
            }

            static void set_clock_divider(uint32_t div)
            {
                tools::mem_barrier m;
                auto addr = clk_base_addr<RPi>();
                *addr = div;
            }
        };

        template<class RPi>
        struct Transfer
        {
            static uint8_t transfer_byte(uint8_t b)
            {
                using ctrl = Control<RPi>;
                auto fifo = fifo_base_addr<RPi>();

                tools::mem_barrier m;
                ctrl::clear_fifo(FIFO::All);
                ctrl::set_ta(true);

                while(!ctrl::can_write());

                *fifo = b;

                while(!ctrl::done());

                uint8_t ret = *fifo;
                ctrl::set_ta(false);
                return ret;
            }

            static void transfer(const uint8_t *pSend, uint8_t *pRecv, uint32_t len)
            {
                using ctrl = Control<RPi>;
                auto fifo = fifo_base_addr<RPi>();

                tools::mem_barrier m;
                ctrl::clear_fifo(FIFO::All);
                ctrl::set_ta(true);

                uint32_t sendLen = 0;
                uint32_t recvLen = 0;

                while((sendLen < len) || (recvLen < len))
                {
                    while(ctrl::can_write() && (sendLen < len))
                    {
                        *fifo = pSend[sendLen];
                        ++sendLen;
                    }

                    while(ctrl::can_read() && (recvLen < len))
                    {
                        pRecv[recvLen] = *fifo;
                        ++recvLen;
                    }
                }

                while(!ctrl::done());
                ctrl::set_ta(false);
            }

            static void drain_read()
            {
                using ctrl = Control<RPi>;
                while(ctrl::can_read())
                    (void) *fifo_base_addr<RPi>();
            }

            static void send(const uint8_t *pSend, uint32_t len)
            {
                using ctrl = Control<RPi>;
                auto fifo = fifo_base_addr<RPi>();

                tools::mem_barrier m;
                ctrl::clear_fifo(FIFO::All);
                ctrl::set_ta(true);

                for(uint32_t sendLen = 0; sendLen < len; ++sendLen)
                {
                    while(!ctrl::can_write());

                    *fifo = pSend[sendLen];
                    drain_read();
                }

                while(!ctrl::done());

                drain_read();
                ctrl::set_ta(false);
            }
        };
    }
}

#endif