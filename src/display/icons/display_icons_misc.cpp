#include "../display_font.h"

namespace display::icons::misc
{

    constinit const auto _symSquare4x4 = SYMBOL(
        ".**.\n"
        ".**.\n"
        ".**.\n"
        ".**."
        );

    constinit const auto _symTrizub = SYMBOL(
        "........*........\n"
        ".*......*......*.\n"
        ".**.....*.....**.\n"
        ".*.*....*....*.*.\n"
        ".**.....*.....**.\n"
        ".*.*...***...*.*.\n"
        ".*..*.*.*.*.*..*.\n"
        ".*....*.*.*....*.\n"
        ".******.*.******.\n"
        ".......*.*.......\n"
        "........*........"
        );

    tools::SymbolHeader symSquare4x4;
    tools::SymbolHeader symTrizub;

    void init()
    {
        symSquare4x4 = _symSquare4x4;
        symTrizub = _symTrizub;
    }
}