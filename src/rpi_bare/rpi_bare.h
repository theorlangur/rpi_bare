#ifndef RPI_BARE_H_
#define RPI_BARE_H_

#ifdef PI_BARE
#define BARECONSTEXPR constexpr
#else
#define BARECONSTEXPR 
#endif

#include <stdint.h>

namespace rpi
{
    namespace tools
    {
        template<class Lambda>
        struct ExitScopeObj
        {
            ExitScopeObj(Lambda &&l):_l(static_cast<Lambda&&>(l)){}
            ~ExitScopeObj() { _l(); }
            Lambda _l;
        };
        template<class L>
        auto ScopeExit(L l){return ExitScopeObj<L>(static_cast<L&&>(l));}

        template<uint8_t N>
        constexpr uint32_t clear_bits(uint32_t from, uint32_t v)
        {
            const uint32_t kClearMask = ((uint32_t(1) << N) - 1) << uint32_t(from);
            return (v & ~kClearMask);
        }

        template<uint8_t N, class E>
        constexpr uint32_t set_bits(uint32_t from, uint32_t v, E bits)
        {
            return clear_bits<N>(from, v) | (uint32_t(bits) << uint32_t(from));
        }

        template<uint8_t N>
        constexpr uint32_t get_bits(uint32_t from, uint32_t v)
        {
            return (v >> uint32_t(from)) & ((uint32_t(1) << N) - 1);
        }

        template<auto from, uint8_t N>
        constexpr uint32_t clear_bits(uint32_t v)
        {
            constexpr uint32_t kClearMask = ((uint32_t(1) << N) - 1) << uint32_t(from);
            return (v & ~kClearMask);
        }

        template<auto from, uint8_t N, class E>
        constexpr uint32_t set_bits(uint32_t v, E bits)
        {
            return clear_bits<from, N>(v) | (uint32_t(bits) << uint32_t(from));
        }

        template<auto from, uint8_t N>
        constexpr uint32_t get_bits(uint32_t v)
        {
            return (v >> uint32_t(from)) & ((uint32_t(1) << N) - 1);
        }

        inline void delay_at_least_cycles(int cycles)
        {
            auto before = __builtin_readcyclecounter();
            while(__builtin_readcyclecounter() < (before + cycles));
        }

        struct mem_barrier
        {
            mem_barrier() { __sync_synchronize(); }
            ~mem_barrier() { __sync_synchronize(); }
        };
    }
}

#endif