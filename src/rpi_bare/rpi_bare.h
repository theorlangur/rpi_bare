#ifndef RPI_BARE_H_
#define RPI_BARE_H_

#ifdef PI_BARE
#define BARECONSTEXPR constexpr
#else
#define BARECONSTEXPR 
#endif

#include <stdint.h>
#include <type_traits>
#include <concepts>

namespace rpi
{
    namespace tools
    {
        template<typename T>
        concept SourceIterator = requires(T a)
        {
            { a++ };
            { *a } -> std::convertible_to<uint8_t>;
        };

        template<typename T>
        concept DestinationIterator = requires(T a)
        {
            { a++ };
            { *a } -> std::same_as<uint8_t&>;
        };

        struct ReverseSource
        {
            ReverseSource(const uint8_t *pBuf, size_t len):pSrc(pBuf + len - 1){}
            template<size_t N>
            ReverseSource(const uint8_t (&buf)[N]):pSrc(buf + N - 1){}

            const uint8_t *operator++(int) { return pSrc--; }
            uint8_t operator*() const { return *pSrc; }

        private:
            const uint8_t *pSrc;
        };

        struct ReverseDestination
        {
            ReverseDestination(uint8_t *pBuf, size_t len):pDst(pBuf + len - 1){}
            template<size_t N>
            ReverseDestination(const uint8_t (&buf)[N]):pDst(buf + N - 1){}

            uint8_t *operator++(int) { return pDst--; }
            uint8_t& operator*() const { return *pDst; }

        private:
            uint8_t *pDst;
        };

        template<class T>
        struct ReverseSourceT: ReverseSource
        {
            ReverseSourceT(T const &d):
                ReverseSource(reinterpret_cast<const uint8_t*>(&d), sizeof(T))
            {}
        };

        template<class T>
        struct ReverseDestinationT: ReverseDestination
        {
            ReverseDestinationT(T &d):
                ReverseDestination(reinterpret_cast<uint8_t*>(&d), sizeof(T))
            {}
        };

        template<class Lambda>
        struct ExitScopeObj
        {
            ExitScopeObj(Lambda &&l):_l(static_cast<Lambda&&>(l)){}
            ~ExitScopeObj() { _l(); }
            Lambda _l;
        };
        template<class L>
        auto ScopeExit(L l){return ExitScopeObj<L>(static_cast<L&&>(l));}

        template<auto N>
        constexpr uint32_t clear_bits(uint32_t from, uint32_t v)
        {
            const uint32_t kClearMask = ((uint32_t(1) << uint8_t(N)) - 1) << uint32_t(from);
            return (v & ~kClearMask);
        }

        template<auto N, class E>
        constexpr uint32_t set_bits(uint32_t from, uint32_t v, E bits)
        {
            return clear_bits<N>(from, v) | (uint32_t(bits) << uint32_t(from));
        }

        template<auto N, class E>
        constexpr void set_bits(volatile uint32_t *pFrom, uint32_t v, E bits)
        {
            *pFrom = set_bits<N, E>(*pFrom, v, bits);
        }

        template<auto N>
        constexpr uint32_t get_bits(uint32_t from, uint32_t v)
        {
            return (v >> uint32_t(from)) & ((uint32_t(1) << uint8_t(N)) - 1);
        }

        template<auto from, auto N>
        constexpr uint32_t clear_bits(uint32_t v)
        {
            constexpr uint32_t kClearMask = ((uint32_t(1) << uint8_t(N)) - 1) << uint32_t(from);
            return (v & ~kClearMask);
        }

        template<auto from, auto N, class E>
        constexpr uint32_t set_bits(uint32_t v, E bits)
        {
            return clear_bits<from, N>(v) | (uint32_t(bits) << uint32_t(from));
        }

        template<auto from, uint8_t N, class E>
        constexpr void set_bits(volatile uint32_t *pFrom, E bits)
        {
            *pFrom = set_bits<from, N, E>(*pFrom, bits);
        }

        template<auto from, auto N>
        constexpr uint32_t get_bits(uint32_t v)
        {
            return (v >> uint32_t(from)) & ((uint32_t(1) << uint8_t(N)) - 1);
        }

        template<class T>
        const T& min(const T& t1, const T& t2) { return t1 < t2 ? t1 : t2;}
        template<class T>
        const T& max(const T& t1, const T& t2) { return t1 > t2 ? t1 : t2;}

        inline constexpr uint16_t swap_bytes(uint16_t v)
        {
            return ((v >> 8) & 0xff) | ((v << 8) & 0xff00);
        }

        inline constexpr uint32_t swap_bytes(uint32_t v)
        {
            //0x12345678
            //0x78563412
            return ((v >> 24) & 0xff) 
                | ((v >> 16) & 0xff00)
                | ((v << 8) & 0xff0000)
                | ((v << 24) & 0xff000000);
        }

        template<std::integral T>
        inline constexpr T swap_bytes(T v)
        {
            uint8_t *pBytes = reinterpret_cast<uint8_t*>(&v);
            for(size_t i = 0; i < sizeof(T); ++i)
                std::swap(pBytes[i], pBytes[sizeof(T) - i - 1]);
            return v;
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

        inline void memcpy(uint8_t *pDest, const uint8_t *pSrc, std::size_t n) { while(n--) *pDest++ = *pSrc++; }
        inline void memcpy(uint8_t *pDest, const volatile uint8_t *pSrc, std::size_t n) { while(n--) *pDest++ = *pSrc++; }
        inline void memcpy(volatile uint8_t *pDest, const uint8_t *pSrc, std::size_t n) { while(n--) *pDest++ = *pSrc++; }
        inline void memcpy(volatile uint8_t *pDest, const volatile uint8_t *pSrc, std::size_t n) { while(n--) *pDest++ = *pSrc++; }
    }

    namespace gpio
    {
        enum class F: uint32_t
        {
            In  = 0b000,
            Out = 0b001,
            F0  = 0b100,
            F1  = 0b101,
            F2  = 0b110,
            F3  = 0b111,
            F4  = 0b011,
            F5  = 0b010,
        };

        enum class PUD: uint32_t
        {
            Off      = 0b00,
            PullDown = 0b01, 
            PullUp   = 0b10, 
        };
    }
}

#endif
