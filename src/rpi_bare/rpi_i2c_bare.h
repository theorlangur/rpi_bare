#ifndef RPI_I2C_BARE_H_
#define RPI_I2C_BARE_H_

#include <algorithm>
#include <variant>

#include "rpi_bare.h"
#include "rpi_gpio_bare.h"

namespace rpi
{
    namespace i2c
    {
#if defined(PI_BARE_FAKE)
        inline uint32_t i2c0_buf[32*1024];
        inline uint32_t i2c1_buf[32*1024];

        inline volatile uint32_t *dbg_i2c0_c = nullptr;
        inline volatile uint32_t *dbg_i2c0_s = nullptr;
        inline volatile uint32_t *dbg_i2c0_dlen = nullptr;
        inline volatile uint32_t *dbg_i2c0_a = nullptr;
        inline volatile uint32_t *dbg_i2c0_fifo = nullptr;
        inline volatile uint32_t *dbg_i2c0_div = nullptr;
        inline volatile uint32_t *dbg_i2c0_del = nullptr;
        inline volatile uint32_t *dbg_i2c0_clkt = nullptr;

        inline volatile uint32_t *dbg_i2c1_c = nullptr;
        inline volatile uint32_t *dbg_i2c1_s = nullptr;
        inline volatile uint32_t *dbg_i2c1_dlen = nullptr;
        inline volatile uint32_t *dbg_i2c1_a = nullptr;
        inline volatile uint32_t *dbg_i2c1_fifo = nullptr;
        inline volatile uint32_t *dbg_i2c1_div = nullptr;
        inline volatile uint32_t *dbg_i2c1_del = nullptr;
        inline volatile uint32_t *dbg_i2c1_clkt = nullptr;

        template<class RPi, uint32_t i2c_base> 
        inline BARECONSTEXPR volatile uint32_t* i2c_base_addr() 
        { 
            if constexpr (i2c_base == RPi::off_i2c0)
                return (volatile uint32_t*)i2c0_buf; 
            else
                return (volatile uint32_t*)i2c1_buf; 
        }
#else
        template<class RPi, uint32_t i2c_base> inline BARECONSTEXPR volatile uint32_t* i2c_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + i2c_base); }
#endif
        enum class c_addr_type: uint32_t{};
        enum class s_addr_type: uint32_t{};
        enum class dlen_addr_type: uint32_t{};
        enum class a_addr_type: uint32_t{};
        enum class fifo_addr_type: uint32_t{};
        enum class div_addr_type: uint32_t{};
        enum class del_addr_type: uint32_t{};
        enum class clkt_addr_type: uint32_t{};

        template<class RPi, uint32_t i2c_base>
        struct i2c_func
        {
            static inline BARECONSTEXPR auto c_addr()    { return (volatile c_addr_type*    )(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_c / 4); }
            static inline BARECONSTEXPR auto s_addr()    { return (volatile s_addr_type*    )(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_s / 4); }
            static inline BARECONSTEXPR auto dlen_addr() { return (volatile dlen_addr_type* )(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_dlen / 4); }
            static inline BARECONSTEXPR auto a_addr()    { return (volatile a_addr_type*    )(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_a / 4); }
            static inline BARECONSTEXPR auto fifo_addr() { return (volatile fifo_addr_type* )(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_fifo / 4); }
            static inline BARECONSTEXPR auto div_addr()  { return (volatile div_addr_type*  )(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_div / 4); }
            static inline BARECONSTEXPR auto del_addr()  { return (volatile del_addr_type*  )(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_del / 4); }
            static inline BARECONSTEXPR auto clkt_addr() { return (volatile clkt_addr_type* )(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_clkt / 4); }
        };

        static constexpr uint32_t max_fifo_size = 16;

        template<class RegAddrType>
        struct BaseReg
        {
            BaseReg() = default;
            BaseReg(volatile const RegAddrType *addr)
            {
                *(RegAddrType*)this = *addr;
            }

            void operator=(volatile const RegAddrType *addr)
            {
                *(RegAddrType*)this = *addr;
            }

            void write_to(volatile RegAddrType *addr) const
            {
                *addr = *(const RegAddrType*)this;
            }

            void read_from(const volatile RegAddrType *addr)
            {
                *(RegAddrType*)this = *addr;
            }
        };

        struct ControlReg: BaseReg<c_addr_type>
        {
            enum class Bits
            {
                read = 0,
                clear = 4,//2 bits
                start_transfer = 7,
                int_done = 8,
                int_tx = 9,
                int_rx = 10,
                i2c_enabled = 15,
            };

            using BaseReg::BaseReg;

            uint32_t read           :1 = 0;
            uint32_t reserved       :3 = 0;
            uint32_t clear          :2 = 0;
            uint32_t reserved2      :1 = 0;
            uint32_t start_transfer :1 = 0;
            uint32_t int_done       :1 = 0;
            uint32_t int_tx         :1 = 0;
            uint32_t int_rx         :1 = 0;
            uint32_t reserved3      :4 = 0;
            uint32_t i2c_enabled    :1 = 0;
            uint32_t reserved4      :16= 0;
        };

        struct StatusReg: BaseReg<s_addr_type>
        {
            using BaseReg::BaseReg;
            enum class Bits
            {
                transfer_active = 0,
                transfer_done = 1,
                tx_need_write = 2,
                tx_need_read = 3,
                tx_can_accept_data = 4,
                rx_has_data = 5,
                tx_fifo_empty = 6,
                rx_fifo_full = 7,
                err_ack = 8,
                clkt_stretch_timeout = 9,
            };

            uint32_t transfer_active      :1 = 0;
            uint32_t transfer_done        :1 = 0;
            uint32_t tx_need_write        :1 = 0;
            uint32_t rx_need_read         :1 = 0;
            uint32_t tx_can_accept_data   :1 = 0;
            uint32_t rx_has_data          :1 = 0;
            uint32_t tx_fifo_empty        :1 = 0;
            uint32_t rx_fifo_full         :1 = 0;
            uint32_t err_ack              :1 = 0;
            uint32_t clkt_stretch_timeout :1 = 0;
            uint32_t reserved             :22= 0;
        };

        struct DlenReg: BaseReg<s_addr_type>
        {
            using BaseReg::BaseReg;
            uint32_t dlen: 16;
            uint32_t reserved: 16;
        };

        struct SlaveAddrReg: BaseReg<a_addr_type>
        {
            using BaseReg::BaseReg;
            uint32_t addr: 7;
            uint32_t reserved: 25;
        };

        struct FifoReg: BaseReg<fifo_addr_type>
        {
            using BaseReg::BaseReg;
            uint32_t data: 8;
            uint32_t reserved: 24;
        };

        struct DivReg: BaseReg<div_addr_type>
        {
            using BaseReg::BaseReg;
            uint32_t div: 16;
            uint32_t reserved: 16;
        };

        struct DelayReg: BaseReg<del_addr_type>
        {
            using BaseReg::BaseReg;
            uint32_t rising_edge_delay: 16;
            uint32_t falling_edge_delay: 16;
        };

        struct ClockStretchTimeoutReg: BaseReg<clkt_addr_type>
        {
            using BaseReg::BaseReg;
            uint32_t tout: 16 = 0x40;
            uint32_t reserved: 16;
        };

        template<class RPi, class pins = typename RPi::I2C1_Pins>
        struct I2C
        {
            template<auto p>
            using PinT = rpi::gpio::Pin<p, RPi>;
            using funcs = i2c_func<RPi, pins::off>;

            enum Error: uint8_t
            {
                Err = 1,
                Timeout = 2
            };
            using TransferResult = std::variant<uint32_t, Error>;

            struct Init
            {
                Init() { init(); configure_all(); }
                ~Init() { end(); }
            };

            static void init()
            {
#if defined(PI_BARE_FAKE)
                if constexpr (!std::is_same_v<pins, typename RPi::I2C1_Pins>)
                {
                    dbg_i2c0_c    = (volatile uint32_t*)funcs::c_addr();
                    dbg_i2c0_s    = (volatile uint32_t*)funcs::s_addr();
                    dbg_i2c0_dlen = (volatile uint32_t*)funcs::dlen_addr();
                    dbg_i2c0_a    = (volatile uint32_t*)funcs::a_addr();
                    dbg_i2c0_fifo = (volatile uint32_t*)funcs::fifo_addr();
                    dbg_i2c0_div  = (volatile uint32_t*)funcs::div_addr();
                    dbg_i2c0_del  = (volatile uint32_t*)funcs::del_addr();
                    dbg_i2c0_clkt = (volatile uint32_t*)funcs::clkt_addr();
                }else
                {
                    dbg_i2c1_c    = (volatile uint32_t*)funcs::c_addr();
                    dbg_i2c1_s    = (volatile uint32_t*)funcs::s_addr();
                    dbg_i2c1_dlen = (volatile uint32_t*)funcs::dlen_addr();
                    dbg_i2c1_a    = (volatile uint32_t*)funcs::a_addr();
                    dbg_i2c1_fifo = (volatile uint32_t*)funcs::fifo_addr();
                    dbg_i2c1_div  = (volatile uint32_t*)funcs::div_addr();
                    dbg_i2c1_del  = (volatile uint32_t*)funcs::del_addr();
                    dbg_i2c1_clkt = (volatile uint32_t*)funcs::clkt_addr();
                }
#endif
                __sync_synchronize();
                PinT<pins::Data>::select(pins::func);
                PinT<pins::Clock>::select(pins::func);
                __sync_synchronize();
            }

            static void end()
            {
                __sync_synchronize();
                PinT<pins::Data>::select(rpi::gpio::F::In);
                PinT<pins::Clock>::select(rpi::gpio::F::In);
                __sync_synchronize();
            }

            static void configure_all()
            {
                clear_fifo();
            }

            static void set_slave_addr(uint8_t addr)
            {
                auto a_reg = funcs::a_addr();
                rpi::tools::set_bits<0, 7>((volatile uint32_t*)a_reg, addr);
            }

            static void clear_fifo()
            {
                auto c_reg = funcs::c_addr();
                rpi::tools::set_bits<ControlReg::Bits::clear, 2>((volatile uint32_t*)c_reg, 0b011);
            }

            static StatusReg status()
            {
                return funcs::s_addr();
            }

            static TransferResult write(const uint8_t *pSend, uint32_t len)
            {
                clear_fifo();
                clear_status();
                auto dlen_reg = funcs::dlen_addr();
                auto preload_len = std::min(len, max_fifo_size);
                rpi::tools::set_bits<0, 16>((volatile uint32_t*)dlen_reg, preload_len);
                uint32_t _len = len;
                len -= preload_len;
                while(preload_len--)
                    write_fifo(*pSend++);

                ControlReg cr;
                cr.read = 0;
                cr.i2c_enabled = 1;
                cr.start_transfer = 1;
                auto c_reg = funcs::c_addr();
                cr.write_to(c_reg);

                StatusReg sr = status();
                while(len)
                {
                    while(!sr.tx_can_accept_data)
                        sr = status();

                    if (sr.err_ack)
                        return Error::Err;
                    if (sr.clkt_stretch_timeout)
                        return Error::Timeout;

                    write_fifo(*pSend++);
                    --len;
                }

                while(!sr.transfer_done && !sr.err_ack && !sr.clkt_stretch_timeout)
                    sr = status();

                if (sr.err_ack)
                    return Error::Err;
                if (sr.clkt_stretch_timeout)
                    return Error::Timeout;

                clear_status();
                return _len - len;
            }

            static TransferResult read(uint8_t *pRecv, uint32_t len)
            {
                clear_fifo();
                clear_status();
                auto dlen_reg = funcs::dlen_addr();
                auto _len = len;

                ControlReg cr;
                cr.read = 1;
                cr.i2c_enabled = 1;
                cr.start_transfer = 1;
                auto c_reg = funcs::c_addr();
                cr.write_to(c_reg);
                StatusReg sr = status();
                while(!sr.transfer_done)
                {
                    while(len && sr.rx_has_data)
                    {
                        *pRecv++ = read_fifo();
                        --len;
                        sr = status();
                    }
                }

                while(len && sr.rx_has_data)
                {
                    *pRecv++ = read_fifo();
                    --len;
                    sr = status();
                }

                if (sr.err_ack)
                    return Error::Err;
                if (sr.clkt_stretch_timeout)
                    return Error::Timeout;

                clear_status();
                return _len - len;
            }

        private:
            static void clear_status()
            {
                auto s_reg = funcs::s_addr();
                StatusReg sr;
                sr.transfer_done = 1;
                sr.err_ack = 1;
                sr.clkt_stretch_timeout = 1;
                sr.write_to(s_reg);
            }

            static bool fifo_empty()
            {
                auto s_reg = funcs::s_addr();
                return ((StatusReg*)s_reg)->tx_fifo_empty;
            }

            static void write_fifo(uint8_t b)
            {
                auto fifo_reg = funcs::fifo_addr();
                rpi::tools::set_bits<0, 8>((volatile uint32_t*)fifo_reg, b);
            }

            static uint8_t read_fifo()
            {
                auto fifo_reg = funcs::fifo_addr();
                return rpi::tools::get_bits<0, 8>((uint32_t)*fifo_reg) & 0x0ff;
            }
        };
    }
}

#endif
