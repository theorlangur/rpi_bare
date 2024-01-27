#ifndef RPI_TIMERS_BARE_H_
#define RPI_TIMERS_BARE_H_

#include "rpi_bare.h"

namespace rpi
{
    namespace timers
    {
#if defined(PI_BARE_FAKE)
        extern uint32_t sys_timer_base[1024];
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* sys_timer_base_addr() { return (volatile uint32_t*)sys_timer_base; }
#else
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* sys_timer_base_addr() { return (volatile uint32_t*)(RPi::io_base_addr + RPi::off_sys_timer); }
#endif

        template<class RPi> inline BARECONSTEXPR volatile uint32_t* cs_base_addr() { return (volatile uint32_t*)(sys_timer_base_addr<RPi>() + RPi::off_sys_timer_cs/4); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* lo_base_addr() { return (volatile uint32_t*)(sys_timer_base_addr<RPi>() + RPi::off_sys_timer_clo/4); }
        template<class RPi> inline BARECONSTEXPR volatile uint32_t* hi_base_addr() { return (volatile uint32_t*)(sys_timer_base_addr<RPi>() + RPi::off_sys_timer_chi/4); }

        template<class RPi>
        struct Sys
        {
            static uint64_t now()
            {
                uint32_t hi = *hi_base_addr<RPi>();
                uint32_t lo = *lo_base_addr<RPi>();
                if (hi != *hi_base_addr<RPi>())
                {
                    hi = *hi_base_addr<RPi>();
                    lo = *lo_base_addr<RPi>();
                }

                return uint64_t(hi) << 32 | lo;
            }

#if defined(PI_BARE_FAKE)
            static void delay_microseconds(uint64_t us)
            {
                //dummy
            }
#else
            static void delay_microseconds(uint64_t us)
            {
                uint64_t n = now();
                while((now() - n) < us);
            }
#endif

            static void delay_ms(uint64_t ms)
            {
                delay_microseconds(ms * 1000);
            }
        };
    }
}

#endif