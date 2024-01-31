#ifndef RPI_SPI_BARE_H_
#define RPI_SPI_BARE_H_

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
        template<uint32_t i2c_base>
        struct i2c_func
        {
            template<class RPi> static inline BARECONSTEXPR volatile uint32_t* c_addr()    { return (volatile uint32_t*)(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_c / 4); }
            template<class RPi> static inline BARECONSTEXPR volatile uint32_t* s_addr()    { return (volatile uint32_t*)(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_s / 4); }
            template<class RPi> static inline BARECONSTEXPR volatile uint32_t* dlen_addr() { return (volatile uint32_t*)(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_dlen / 4); }
            template<class RPi> static inline BARECONSTEXPR volatile uint32_t* a_addr()    { return (volatile uint32_t*)(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_a / 4); }
            template<class RPi> static inline BARECONSTEXPR volatile uint32_t* fifo_addr() { return (volatile uint32_t*)(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_fifo / 4); }
            template<class RPi> static inline BARECONSTEXPR volatile uint32_t* div_addr()  { return (volatile uint32_t*)(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_div / 4); }
            template<class RPi> static inline BARECONSTEXPR volatile uint32_t* del_addr()  { return (volatile uint32_t*)(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_del / 4); }
            template<class RPi> static inline BARECONSTEXPR volatile uint32_t* clkt_addr() { return (volatile uint32_t*)(i2c_base_addr<RPi, i2c_base>() + RPi::off_i2c_clkt / 4); }
        };


        template<class RPi, class pins = typename RPi::I2C1_Pins>
        struct Config
        {
            template<auto p>
            using PinT = rpi::gpio::Pin<p, RPi>;

            static void init()
            {
#if defined(PI_BARE_FAKE)
                if constexpr (std::is_same_v<pins, typename RPi::I2C1_Pins>)
                {
                    dbg_i2c0_c = i2c_func<pins::off>::c_addr<RPi>();
                    dbg_i2c0_s = i2c_func<pins::off>::s_addr<RPi>();
                    dbg_i2c0_dlen = i2c_func<pins::off>::dlen_addr<RPi>();
                    dbg_i2c0_a = i2c_func<pins::off>::a_addr<RPi>();
                    dbg_i2c0_fifo = i2c_func<pins::off>::fifo_addr<RPi>();
                    dbg_i2c0_div = i2c_func<pins::off>::div_addr<RPi>();
                    dbg_i2c0_del = i2c_func<pins::off>::del_addr<RPi>();
                    dbg_i2c0_clkt = i2c_func<pins::off>::clkt_addr<RPi>();
                }else
                {
                    dbg_i2c1_c = i2c_func<pins::off>::c_addr<RPi>();
                    dbg_i2c1_s = i2c_func<pins::off>::s_addr<RPi>();
                    dbg_i2c1_dlen = i2c_func<pins::off>::dlen_addr<RPi>();
                    dbg_i2c1_a = i2c_func<pins::off>::a_addr<RPi>();
                    dbg_i2c1_fifo = i2c_func<pins::off>::fifo_addr<RPi>();
                    dbg_i2c1_div = i2c_func<pins::off>::div_addr<RPi>();
                    dbg_i2c1_del = i2c_func<pins::off>::del_addr<RPi>();
                    dbg_i2c1_clkt = i2c_func<pins::off>::clkt_addr<RPi>();
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
        };

        template<class RPi, typename pins=RPi::I2C1_Pins>
        struct Control
        {
            enum class Bits : uint32_t
            {
            };

            template<class E>
            friend uint32_t operator<<(E e, Bits b) { return uint32_t(e) << uint32_t(b); }

            static void configure_all()
            {
                //implement
            }

        };

        template<class RPi, class pins=typename RPi::SPI1_Pins>
        struct Transfer
        {
            using ctrl = Control<RPi, typename RPi::SPI1_Pins>;

            static uint8_t transfer_byte(uint8_t b)
            {
                //implement
            }
            
            static void write(const uint8_t *pSend, uint32_t len)
            {
                //implement
            }

            static void read(uint8_t *pRecv, uint32_t len)
            {
                //implement
            }
        };

        template<class RPi, class I2C>
        struct I2CInit
        {
            I2CInit()
            {
                rpi::i2c::Config<RPi, I2C>::init();
                rpi::i2c::Control<RPi, I2C>::configure_all();
            }
            ~I2CInit()
            {
                rpi::i2c::Config<RPi, I2C>::end();
            }
        };
    }
}

#endif
