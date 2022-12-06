#include "../display_font.h"

namespace display::icons::misc
{
    constinit const auto symSquare4x4 = display::tools::CreateSymbol(
        ".**.\n"
        ".**.\n"
        ".**.\n"
        ".**."
        );

    constinit const auto symTrizub = display::tools::CreateSymbol(
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
}