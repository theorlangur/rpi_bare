#include "../display_font.h"

namespace display::font::f5x7
{
    constinit const auto s0 = display::tools::CreateSymbol(
        ".***.\n"
        "*...*\n"
        "*..**\n"
        "*.*.*\n"
        "**..*\n"
        "*...*\n"
        ".***."
        );
    constinit const auto s1 = display::tools::CreateSymbol(
        "....*\n"
        "...**\n"
        "..*.*\n"
        ".*..*\n"
        "....*\n"
        "....*\n"
        "....*"
        );
    constinit const auto s2 = display::tools::CreateSymbol(
        ".***.\n"
        "*...*\n"
        "*...*\n"
        "...*.\n"
        "..*..\n"
        ".*...\n"
        "*****"
        );
    constinit const auto s3 = display::tools::CreateSymbol(
        ".***.\n"
        "*...*\n"
        "....*\n"
        "..**.\n"
        "....*\n"
        "*...*\n"
        ".***."
        );
    constinit const auto s4 = display::tools::CreateSymbol(
        ".*..*\n"
        ".*..*\n"
        "*...*\n"
        "*****\n"
        "....*\n"
        "....*\n"
        "....*"
        );
    constinit const auto s5 = display::tools::CreateSymbol(
        "*****\n"
        "*....\n"
        "*....\n"
        "****.\n"
        "....*\n"
        "*...*\n"
        ".***."
        );
    constinit const auto s6 = display::tools::CreateSymbol(
        ".***.\n"
        "*...*\n"
        "*....\n"
        "*.**.\n"
        "*...*\n"
        "*...*\n"
        ".***."
        );
    constinit const auto s7 = display::tools::CreateSymbol(
        "****.\n"
        "....*\n"
        "...*.\n"
        ".***.\n"
        ".*...\n"
        "*....\n"
        "*...."
        );
    constinit const auto s8 = display::tools::CreateSymbol(
        ".***.\n"
        "*...*\n"
        "*...*\n"
        ".***.\n"
        "*...*\n"
        "*...*\n"
        ".***."
        );
    constinit const auto s9 = display::tools::CreateSymbol(
        ".***.\n"
        "*...*\n"
        "*...*\n"
        ".****\n"
        "....*\n"
        "*...*\n"
        ".***."
        );

    constinit const auto sa = display::tools::CreateSymbol(
        ".....\n"
        ".....\n"
        ".....\n"
        ".**.*\n"
        "*..**\n"
        "*..**\n"
        ".**.*"
        );
    constinit const auto sb = display::tools::CreateSymbol(
        "*....\n"
        "*....\n"
        "*....\n"
        "*.**.\n"
        "*...*\n"
        "*...*\n"
        "*.**."
        );
    constinit const auto sc = display::tools::CreateSymbol(
        ".....\n"
        ".....\n"
        ".***.\n"
        "*...*\n"
        "*....\n"
        "*...*\n"
        ".***."
        );
    constinit const auto sd = display::tools::CreateSymbol(
        "....*\n"
        "....*\n"
        "....*\n"
        ".**.*\n"
        "*...*\n"
        "*...*\n"
        ".**.*"
        );
    constinit const auto se = display::tools::CreateSymbol(
        ".....\n"
        ".....\n"
        ".***.\n"
        "*...*\n"
        "*****\n"
        "*....\n"
        ".***."
        );
    constinit const auto sf = display::tools::CreateSymbol(
        "..**.\n"
        ".*..*\n"
        ".*...\n"
        "***..\n"
        ".*...\n"
        ".*...\n"
        ".*..."
        );
    constinit const auto sx = display::tools::CreateSymbol(
        ".....\n"
        ".....\n"
        "*...*\n"
        ".*.*.\n"
        "..*..\n"
        ".*.*.\n"
        "*...*"
        );
}

namespace display::font::f5x7
{
    void init(tools::SymbolHeader (&_Chars)[256])
    {
        _Chars['0'] = s0;
        _Chars['1'] = s1;
        _Chars['2'] = s2;
        _Chars['3'] = s3;
        _Chars['4'] = s4;
        _Chars['5'] = s5;
        _Chars['6'] = s6;
        _Chars['7'] = s7;
        _Chars['8'] = s8;
        _Chars['9'] = s9;

        _Chars['a'] = sa;
        _Chars['b'] = sb;
        _Chars['c'] = sc;
        _Chars['d'] = sd;
        _Chars['e'] = se;
        _Chars['f'] = sf;
        _Chars['x'] = sx;
    }
}
