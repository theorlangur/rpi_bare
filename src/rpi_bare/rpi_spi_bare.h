#ifndef RPI_SPI_BARE_H_
#define RPI_SPI_BARE_H_

#include "rpi_bare.h"
#include "rpi_gpio_bare.h"

namespace rpi
{
    namespace spi
    {
#if defined(PI_BARE_FAKE)
        inline uint32_t spi0_base[32*1024];
        inline uint32_t aux_base[32*1024];

        inline volatile uint32_t *dbg_spi0_cs = nullptr;
        inline volatile uint32_t *dbg_spi0_fifo = nullptr;
        inline volatile uint32_t *dbg_spi0_clk = nullptr;
        inline volatile uint32_t *dbg_spi0_dlen = nullptr;

        inline volatile uint32_t *dbg_aux_enabled = nullptr;
        inline volatile uint32_t *dbg_aux_spi1_cntl = nullptr;
        inline volatile uint32_t *dbg_aux_spi1_cntl1 = nullptr;
        inline volatile uint32_t *dbg_aux_spi1_stat = nullptr;
        inline volatile uint32_t *dbg_aux_spi1_peek = nullptr;
        inline volatile uint32_t *dbg_aux_spi1_io = nullptr;
        inline volatile uint32_t *dbg_aux_spi1_txhold = nullptr;

        template<class RPi> inline BARECONSTEXPR volatile uint32_t* spi0_base_addr() { return (volatile uint32_t*)spi0_base; }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* aux_base_addr() { return (volatile uint32_t*)aux_base; }
#else
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* spi0_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_spi0); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* aux_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_aux); }
#endif
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* cs_base_addr() { return (volatile uint32_t*)(spi0_base_addr<RPi>() + RPi::off_spi0_cs / 4); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* fifo_base_addr() { return (volatile uint32_t*)(spi0_base_addr<RPi>() + RPi::off_spi0_fifo/4); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* clk_base_addr() { return (volatile uint32_t*)(spi0_base_addr<RPi>() + RPi::off_spi0_clk/4); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* dlen_base_addr() { return (volatile uint32_t*)(spi0_base_addr<RPi>() + RPi::off_spi0_dlen/4); }

        template<class RPi> inline BARECONSTEXPR volatile uint32_t* aux_enabled_addr() { return aux_base_addr<RPi>() + RPi::off_aux_enables / 4; }

        template<class RPi> inline BARECONSTEXPR volatile uint32_t* aux_spi1_cntl_addr()   { return (volatile uint32_t*)(aux_base_addr<RPi>() + RPi::off_aux_spi1_cntl0 / 4); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* aux_spi1_stat_addr()   { return (volatile uint32_t*)(aux_base_addr<RPi>() + RPi::off_aux_spi1_stat/4); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* aux_spi1_peek_addr()   { return (volatile uint32_t*)(aux_base_addr<RPi>() + RPi::off_aux_spi1_peek/4); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* aux_spi1_io_addr()     { return (volatile uint32_t*)(aux_base_addr<RPi>() + RPi::off_aux_spi1_io/4); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* aux_spi1_txhold_addr() { return (volatile uint32_t*)(aux_base_addr<RPi>() + RPi::off_aux_spi1_txhold/4); }

        template<class RPi> static constexpr uint32_t calc_spi_freq(uint16_t div) { return RPi::g_SysFreqHz / (2 * (div + 1));}
        template<class RPi> constexpr static uint32_t g_MaxFreq = calc_spi_freq<RPi>(0);
        template<class RPi> constexpr static uint32_t g_MinFreq = calc_spi_freq<RPi>(0b01111'1111'1111);
        template<class RPi> static constexpr uint16_t calc_spi_divider(uint32_t f) { return (RPi::g_SysFreqHz + f - 1) / f;}
        constexpr static uint16_t g_MaxSpeedDivider = 0xfff;

        template<class RPi> 
        static uint16_t get_speed_div_from_freq(uint32_t f)
        {
            if (f > g_MaxFreq<RPi>) f = g_MaxFreq<RPi>;
            else if (f < g_MinFreq<RPi>) f = g_MinFreq<RPi>;

            auto s = calc_spi_divider<RPi>(f * 2);
            if (s > g_MaxSpeedDivider) s = g_MaxSpeedDivider;
            return s;
        }

        template<class RPi, class pins = typename RPi::SPI0_Pins>
        struct Config
        {
            template<auto p>
            using PinT = rpi::gpio::Pin<p, RPi>;

            static void init()
            {
#if defined(PI_BARE_FAKE)
                if constexpr (std::is_same_v<pins, typename RPi::SPI0_Pins>)
                {
                    dbg_spi0_cs = cs_base_addr<RPi>();
                    dbg_spi0_fifo = fifo_base_addr<RPi>();
                    dbg_spi0_clk = clk_base_addr<RPi>();
                    dbg_spi0_dlen = dlen_base_addr<RPi>();
                }else
                {
                    dbg_aux_enabled = aux_enabled_addr<RPi>();
                    dbg_aux_spi1_cntl = aux_spi1_cntl_addr<RPi>();
                    dbg_aux_spi1_cntl1 = dbg_aux_spi1_cntl + 1;
                    dbg_aux_spi1_stat = aux_spi1_stat_addr<RPi>();
                    dbg_aux_spi1_peek = aux_spi1_peek_addr<RPi>();
                    dbg_aux_spi1_io = aux_spi1_io_addr<RPi>();
                    dbg_aux_spi1_txhold = aux_spi1_txhold_addr<RPi>();
                }
#endif
                __sync_synchronize();
                if constexpr (std::is_same_v<pins, typename RPi::SPI1_Pins>)
                    PinT<pins::CE2_N>::select(pins::func);
                PinT<pins::CE1_N>::select(pins::func);
                PinT<pins::CE0_N>::select(pins::func);
                PinT<pins::MISO>::select(pins::func);
                PinT<pins::MOSI>::select(pins::func);
                PinT<pins::SCLK>::select(pins::func);
                __sync_synchronize();
            }

            static void end()
            {
                __sync_synchronize();
                if constexpr (std::is_same_v<pins, typename RPi::SPI1_Pins>)
                    PinT<pins::CE2_N>::select(rpi::gpio::F::In);
                PinT<pins::CE1_N>::select(rpi::gpio::F::In);
                PinT<pins::CE0_N>::select(rpi::gpio::F::In);
                PinT<pins::MISO>::select(rpi::gpio::F::In);
                PinT<pins::MOSI>::select(rpi::gpio::F::In);
                PinT<pins::SCLK>::select(rpi::gpio::F::In);
                __sync_synchronize();
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

        enum class FIFO : uint32_t
        {
            None = 0b00,
            Tx   = 0b01,
            Rx   = 0b10,
            All  = Tx | Rx,
        };

        template<class RPi, class pins>
        struct SPI;

        template<class RPi>
        struct SPI<RPi, typename RPi::SPI0_Pins>
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

            static void configure_all(
                RPi::SPI0_Pins::Chip cs = RPi::SPI0_Pins::Chip::CS0, 
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

            static void set_clock_divider(uint32_t div)
            {
                tools::mem_barrier m;
                auto addr = clk_base_addr<RPi>();
                *addr = div;
            }

            static void set_speed_div_from_freq(uint32_t f)
            {
                set_clock_divider(get_speed_div_from_freq<RPi>(f));
            }

            static uint8_t transfer_byte(uint8_t b)
            {
                auto fifo = fifo_base_addr<RPi>();

                tools::mem_barrier m;
                clear_fifo(FIFO::All);
                set_ta(true);

                while(!can_write());

                *fifo = b;

                while(!done());

                uint8_t ret = *fifo;
                set_ta(false);
                return ret;
            }

            static void transfer(const uint8_t *pSend, uint8_t *pRecv, uint32_t len)
            {
                auto fifo = fifo_base_addr<RPi>();

                tools::mem_barrier m;
                clear_fifo(FIFO::All);
                set_ta(true);

                uint32_t sendLen = 0;
                uint32_t recvLen = 0;

                while((sendLen < len) || (recvLen < len))
                {
                    while(can_write() && (sendLen < len))
                    {
                        *fifo = pSend[sendLen];
                        ++sendLen;
                    }

                    while(can_read() && (recvLen < len))
                    {
                        pRecv[recvLen] = *fifo;
                        ++recvLen;
                    }
                }

                while(!done())
                    drain_read();
                set_ta(false);
            }

            static void send(const uint8_t *pSend, uint32_t len)
            {
                auto fifo = fifo_base_addr<RPi>();

                tools::mem_barrier m;
                clear_fifo(FIFO::All);
                set_ta(true);

                for(uint32_t sendLen = 0; sendLen < len; ++sendLen)
                {
                    while(!can_write());

                    *fifo = pSend[sendLen];
                    drain_read();
                }

                while(!done())
                    drain_read();

                set_ta(false);
            }
        private:
            template<class E>
            friend uint32_t operator<<(E e, Bits b) { return uint32_t(e) << uint32_t(b); }

            static void drain_read()
            {
                while(can_read())
                    (void) *fifo_base_addr<RPi>();
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
        };

        template<class RPi>
        struct SPI<RPi, typename RPi::SPI1_Pins>
        {
        private:
            enum class Bits : uint32_t
            {
                enalbeSPI1 = 1,

                speed = 20,
                speed_len = 12,

                cs     = 17,
                cs_len = 3,

                post_input = 16,
                var_cs = 15,
                var_width = 14,

                dout_hold = 12,
                dout_hold_len=2,

                enable = 11,
                in_pol = 10,
                clr_fifo = 9,
                out_pol = 8,
                clk_pol = 7,
                ms_first = 6,

                shift_len = 0,
                shift_len_len = 6,

                busy = 6,
                rx_empty = 7,
                rx_full = 8,
                tx_empty = 9,
                tx_full = 10,
                bits_left = 0,
                bits_left_len = 6,

                c1_keep_input = 0,
                c1_ms_in_first = 1,
            };

            union Control0
            {
                struct Bits
                {
                    uint32_t shift_len    : 6 = 0; //0-5
                    uint32_t out_ms_first : 1 = 1; //6
                    uint32_t inv_clk      : 1 = 0; //7
                    uint32_t out_rising   : 1 = 1; //8
                    uint32_t clear_fifo   : 1 = 0; //9
                    uint32_t in_rising    : 1 = 1; //10
                    uint32_t enable       : 1 = 1; //11
                    uint32_t dout_hold_t  : 2 = 0; //12-13
                    uint32_t var_width    : 1 = 0; //14
                    uint32_t var_cs       : 1 = 0; //15
                    uint32_t post_input   : 1 = 0; //16
                    uint32_t cs_0         : 1 = 1; //17
                    uint32_t cs_1         : 1 = 1; //18
                    uint32_t cs_2         : 1 = 1; //19
                    uint32_t speed        : 12 = 0; //20-31
                }m_bits{};
                uint32_t m_dw32;
            };

            union Control1
            {
                struct Bits
                {
                    uint32_t keep_input    : 1 = 0; //0
                    uint32_t in_ms_first   : 1 = 1; //1
                    uint32_t unused        : 4 = 0; //2-5
                    uint32_t done_irq      : 1 = 0; //6
                    uint32_t tx_empty      : 1 = 0; //7
                    uint32_t cs_high       : 3 = 0; //8-10
                }m_bits{};
                uint32_t m_dw32;
            };

            inline static Control0 g_Control0;
            inline static Control1 g_Control1;

            template<class E>
            friend uint32_t operator<<(E e, Bits b) { return uint32_t(e) << uint32_t(b); }
        public:

            static void set_speed_div_from_freq(uint32_t f)
            {
                set_clock_divider(get_speed_div_from_freq<RPi>(f));
            }

            static void set_clock_divider(uint32_t d)
            {
                g_Control0.m_bits.speed = d;
            }

            static void configure_all(
                RPi::SPI1_Pins::Chip cs = RPi::SPI1_Pins::Chip::CS0, 
                ClockPhase ph = ClockPhase::Middle, 
                Polarity clock = Polarity::Low, 
                Polarity chipSelectAll = Polarity::Low,
                Polarity chipSelect0 = Polarity::Low,
                Polarity chipSelect1 = Polarity::Low,
                Polarity chipSelect2 = Polarity::Low
            )
            {
                auto chipSelectInverse = (~(uint32_t(1) << (uint32_t)cs)) & 0x7/*we have 3 CE to select from, so 0b0111*/;
                g_Control0.m_dw32 = rpi::tools::set_bits<Bits::cs, Bits::cs_len>(g_Control0.m_dw32, chipSelectInverse);
                g_Control0.m_bits.inv_clk = (uint32_t)clock;

                rpi::tools::set_bits<Bits::enalbeSPI1, 1>(aux_enabled_addr<RPi>(), 1);
                end_transfer();
            }

            static uint8_t transfer_byte(uint8_t b)
            {
                auto io = aux_spi1_io_addr<RPi>();
                begin_transfer(8);
                rpi::tools::set_bits<24, 8>(io, b);
                while(!done());
                b = *io & 0xff;
                end_transfer();
                return b;
            }
            
            static void send(const uint8_t *pSend, uint32_t len)
            {
                auto io = aux_spi1_io_addr<RPi>();
                auto txhold = aux_spi1_txhold_addr<RPi>();
                begin_transfer(0);//variable width
                while(len)
                {
                    while(is_full());
                    uint32_t cnt = rpi::tools::min(len, uint32_t(3));
                    uint32_t w = rpi::tools::set_bits<24, 5>(uint32_t(0), cnt * 8);
                    for(int i = 0; i < cnt; ++i, ++pSend)
                        w |= (*pSend) << ((2 - i) * 8);
                    len -= cnt;
                    if (len)
                        *txhold = w;
                    else
                        *io = w;
                    while(!done());

                    (void)*io;
                }
                end_transfer();
            }
            
            static void transfer(const uint8_t *pSend, uint8_t *pRecv, uint32_t len)
            {
                auto io = aux_spi1_io_addr<RPi>();
                auto txhold = aux_spi1_txhold_addr<RPi>();
                uint32_t tx_len = len, rx_len = len;
                begin_transfer(0);//variable width
                while(tx_len || rx_len)
                {
                    while(can_write() && tx_len)
                    {
                        uint32_t cnt = rpi::tools::min(tx_len, uint32_t(3));
                        uint32_t w = rpi::tools::set_bits<24, 8>(uint32_t(0), cnt * 24);
                        for(int i = 0; i < cnt; ++i, ++pSend)
                            w |= (*pSend) << ((2 - i) * 8);
                        tx_len -= cnt;
                        if (len)
                            *txhold = w;
                        else
                            *io = w;
                    }

                    while((can_read() || !done()) && rx_len)
                    {
                        uint32_t cnt = rpi::tools::min(rx_len, uint32_t(3));
                        uint32_t w = *io;
                        if (pRecv)
                        {
                            for(int i = 0; i < cnt; ++i, ++pRecv)
                                *pRecv = (w >> ((2-i) * 8)) & 0xff;
                        }
                        rx_len -= cnt;
                    }
                }
                end_transfer();
            }
        private:
            static void begin_transfer(uint8_t bits_to_transfer)
            {
                auto cntl0 = aux_spi1_cntl_addr<RPi>();
                auto cntl1 = cntl0 + 1;
                g_Control0.m_bits.var_width = bits_to_transfer == 0;
                g_Control0.m_bits.shift_len = bits_to_transfer;
                g_Control1.m_bits.in_ms_first = true;

                *cntl0 = g_Control0.m_dw32;
                *cntl1 = g_Control1.m_dw32;
            }
            
            static void end_transfer()
            {
                auto cntl0 = aux_spi1_cntl_addr<RPi>();
                auto cntl1 = cntl0 + 1;

                *cntl1 = 0;
                *cntl0 = rpi::tools::set_bits<Bits::clr_fifo, 1>(uint32_t(0), 1);
            }

            static void clear_fifo(FIFO f)
            {
                rpi::tools::set_bits<Bits::clr_fifo, 1>(aux_spi1_cntl_addr<RPi>(), 1);
            }

            static bool done()
            {
                tools::mem_barrier m;
                return !rpi::tools::get_bits<Bits::busy, 1>(*aux_spi1_stat_addr<RPi>());
            }

            static bool can_write()
            {
                tools::mem_barrier m;
                return !rpi::tools::get_bits<Bits::tx_full, 1>(*aux_spi1_stat_addr<RPi>());
            }

            static bool can_read()
            {
                tools::mem_barrier m;
                return !rpi::tools::get_bits<Bits::rx_empty, 1>(*aux_spi1_stat_addr<RPi>());
            }

            static bool is_full()
            {
                tools::mem_barrier m;
                return rpi::tools::get_bits<Bits::tx_full, 1>(*aux_spi1_stat_addr<RPi>());
            }

            static bool needs_reading()
            {
                tools::mem_barrier m;
                return !rpi::tools::get_bits<Bits::rx_empty, 1>(*aux_spi1_stat_addr<RPi>());
            }
        };

        template<class RPi, class SPIPins>
        struct SPIInit
        {
            SPIInit(typename SPIPins::Chip cs, uint32_t freq=1000'000)
            {
                rpi::spi::Config<RPi, SPIPins>::init();
                //rpi::spi::Control<RPi, SPIPins>::set_clock_divider(128);
                rpi::spi::SPI<RPi, SPIPins>::set_speed_div_from_freq(freq);
                rpi::spi::SPI<RPi, SPIPins>::configure_all(cs);
            }
            ~SPIInit()
            {
                rpi::spi::Config<RPi, SPIPins>::end();
            }
        };
    }
}

#endif
