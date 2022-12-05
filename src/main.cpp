#include "rpi.h"
#include "display_driver.h"
#include "display_font.h"
#include "rpi_bare/rpi_bare.h"
#include "rpi_bare/rpi_cfg_bare.h"
#include "rpi_bare/rpi_timers_bare.h"

void draw_sq(uint8_t x, uint8_t y, bool v)
{
    display::set_point(x, y,    v);
    display::set_point(x+2, y,  v);
    display::set_point(x, y+2,  v);
    display::set_point(x+2, y+2,v);
}

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

struct SymbolHeader
{
    const uint8_t *pData;
    Dims size;
};

void render_symbol(Point p, const SymbolHeader &s)
{
    display::set_bitmap(p.x, p.y, s.pData, s.size.w, s.size.h);
}

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


template<size_t N>
struct Symbol: SymbolHeader
{
    static constexpr size_t kTotalMemSize = N;
    uint8_t symData[kTotalMemSize];

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
    return Symbol<N>(d, zero, one);
}

constinit const auto symSquare4x4 = CreateSymbol(
    ".**.\n"
    ".**.\n"
    ".**.\n"
    ".**."
    );

constinit const auto symTrizub = CreateSymbol(
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

extern "C" void kernel_main()
{
    using Timer = rpi::timers::Sys<rpi::RPiBplus>;
    rpi::Init initRpi;
    if (!initRpi)
        return;
    display::Init initDisplay;
    if (!initDisplay)
        return;

    display::clear();
    display::show();
    Timer::delay_ms(2000);
    for(uint8_t i = 0; i < 16; ++i)
    {
        hex::draw_num(i * (hex::kW + 1), 1, i);
    }
    display::show();
    Timer::delay_ms(5000);
    hex::draw_hex32(0, 1 + (hex::kH + 1) * 1, (uint32_t)&kernel_main);
    hex::draw_hex32(0, 1 + ((hex::kH + 1) * 2), (uint32_t)&symTrizub);
    hex::draw_hex32(0, 1 + ((hex::kH + 1) * 3), (uint32_t)display::DisplayMemory);
    display::show();
    Timer::delay_ms(10000);
    render_symbol({0,30}, symTrizub);
    display::show_part(0, 30, symTrizub.size.w, symTrizub.size.h);
    Timer::delay_ms(20000);

    display::clear_part(0, 0, 64, 8);
    Timer::delay_ms(10000);
    display::clear_part(0, 8, 64, 8);
    Timer::delay_ms(10000);
    for(int i = 0; i < 64; ++i)
    {
        display::DisplayMemory[i] = 0xff;
        display::DisplayMemory[i + 128] = 0x0f;
    }
    display::show_part(0, 0, 64, 12);
    Timer::delay_ms(10000);
    display::clear_part(0, 16, 64, 8);
    Timer::delay_ms(10000);
    //display::clear_part(0, 0, symTrizub.size.w, symTrizub.size.h);
    display::clear_part(0, 0, 18, 11);
    draw_sq(1, 1, true);
    display::show_part(0, 0, 8, 8);
    Timer::delay_ms(5000);
    render_symbol({0,0}, symTrizub);
    display::show_part(0, 0, symTrizub.size.w, symTrizub.size.h);
    Timer::delay_ms(10000);

    uint8_t x = 64, y = 32;

    display::clear();
    display::show();
    Timer::delay_ms(2000);

    bool keyUp = false, keyDown = false, keyRight = false, keyLeft = false;

    Point pos{2, 0}, prev;
    int8_t dx = 1, dy = 1;
    prev = pos;

    while(true)
    {
        Timer::delay_ms(5);
        /*if (display::is_pressed(display::Pins::KEY1))
        {
            break;
        }*/

        display::clear_part(prev.x, prev.y, symTrizub.size.w, symTrizub.size.h);
        render_symbol(pos, symTrizub);
        display::show_part(pos.x, pos.y, symTrizub.size.w, symTrizub.size.h);
        if ((dx > 0 && ((pos.x + dx) + symTrizub.size.w >= display::kDisplayWidth)) || ((dx < 0) && !pos.x))
            dx = -dx;

        if (((pos.y + dy) + symTrizub.size.h >= display::kDisplayHeight) || ((dy < 0) && !pos.y))
            dy = -dy;

        prev = pos;
        pos.x += dx;
        pos.y += dy;
    }
}

#ifndef PI_BARE
int main()
{
    kernel_main();
    return 0;
}
#endif