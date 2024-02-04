#ifndef DISPLAY_TOOLS_H_
#define DISPLAY_TOOLS_H_

#include "display_driver.h"

#define SYMBOL(s) display::tools::CreateSymbolD<display::tools::analyze_literal(s)>(s)

namespace display
{
    namespace tools
    {
        template<class T> T diff_abs(T a, T b) { return a > b ? a - b : b - a; }

        struct Dims
        {
            uint8_t w;
            uint8_t h;
        };

        struct Point
        {
            uint8_t x;
            uint8_t y;
        };

        struct Rect
        {
            Point p;
            Dims d;
        };

        struct SymbolHeader
        {
            const uint8_t *pData;
            Dims size;
        };

        template<size_t N>
        constexpr Dims analyze_literal(const char (&d)[N])
        {
            Dims r{0, 0};
            for(size_t i = 0; i < N; ++i)
            {
                if (d[i] == '\n')
                {
                    if (r.h == 0) r.w = i;
                    ++r.h;
                }
            }
            ++r.h;
            return r;
        }

        template<Dims dm>
        struct Symbol: SymbolHeader
        {
            static constexpr size_t kTotalMemSize = dm.w * ((dm.h + 7)/8);
            uint8_t symData[kTotalMemSize];

            template<size_t N>
            constexpr Symbol(const char (&d)[N], char zero = '.', char one='*')
            {
                for(size_t i = 0; i < kTotalMemSize; ++i)
                    symData[i] = 0;

                size = analyze_literal(d);
                uint8_t x = 0, y = 0;
                size_t bit = 0, byte = 0;
                for(size_t i = 0; i < N; ++i, ++x)
                {
                    if (d[i] == one)
                    {
                        size_t byteOff = byte * size.w + x;
                        symData[byteOff] |= 1 << bit;
                    }
                    else if (d[i] == '\n')
                    {
                        ++y;
                        bit = y % 8;
                        byte = y / 8;
                        x = -1;
                    }
                }
                pData = symData;
            }
        };

        template<size_t N>
        constexpr auto CreateSymbol(const char (&d)[N], char zero = '.', char one='*')
        {
            return Symbol<Dims{N,8}>(d, zero, one);
        }

        template<Dims dm, size_t N>
        constexpr auto CreateSymbolD(const char (&d)[N], char zero = '.', char one='*')
        {
            return Symbol<dm>(d, zero, one);
        }
    }
}

#endif
