#ifndef RPI_BARE_TOOLS_H_
#define RPI_BARE_TOOLS_H_

#ifdef PI_BARE
#define BARECONSTEXPR constexpr
#else
#define BARECONSTEXPR 
#endif

#include <stdint.h>
#include <type_traits>
#include <concepts>
#include <expected>

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


        inline void memcpy(uint8_t *pDest, const uint8_t *pSrc, std::size_t n) { while(n--) *pDest++ = *pSrc++; }
        inline void memcpy(uint8_t *pDest, const volatile uint8_t *pSrc, std::size_t n) { while(n--) *pDest++ = *pSrc++; }
        inline void memcpy(volatile uint8_t *pDest, const uint8_t *pSrc, std::size_t n) { while(n--) *pDest++ = *pSrc++; }
        inline void memcpy(volatile uint8_t *pDest, const volatile uint8_t *pSrc, std::size_t n) { while(n--) *pDest++ = *pSrc++; }

        template<class Eto, class V, class Efrom, typename F>
        std::expected<V,Eto> convert_expected(std::expected<V,Efrom> &&e, F &&eCnv)
        {
            if (e)
            {
                if constexpr (std::is_same_v<V,void>)
                    return {};
                else
                    return std::move(*e);
            }

            return std::unexpected(eCnv(std::move(e.error())));
        }
    }
}

#endif
