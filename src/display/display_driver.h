#ifndef DISPLAY_DRIVER_H_
#define DISPLAY_DRIVER_H_
#include <stddef.h>
#include <stdint.h>

#include "../rpi_cfg.h"

namespace display
{
    //GPIO pins
    enum class Pins: uint8_t
    {
        //OLED
        OLED_CS = OLED_DISPLAY_CS_PIN,
        OLED_RST = 25,
        OLED_DC = 24,
        //Keys
        KEY_UP = 6,
        KEY_DOWN = 19,
        KEY_LEFT = 5,
        KEY_RIGHT = 26,
        KEY_PRESS = 13,
        KEY1 = 21,
        KEY2 = 20,
        KEY3 = 16,
    };

    void operator <<(Pins p, bool v);
    //returns true if values was changed
    bool operator >>(Pins p, bool &v);

    constexpr uint8_t kDisplayMaxWidth = 132;
    constexpr uint8_t kDisplayMaxHeight = 64;
    constexpr uint8_t kWidthPadding = 2;
    constexpr uint8_t kDisplayWidth = kDisplayMaxWidth - kWidthPadding*2;
    constexpr uint8_t kDisplayHeight = kDisplayMaxHeight;
    constexpr uint16_t kDisplayMemorySize = kDisplayWidth * kDisplayHeight / 8;
    extern uint8_t DisplayMemory[kDisplayMemorySize];

    bool init();
    void init_gpio_pins();
    void reset();

    void finish();

    bool is_pressed(Pins pin);

    void switch_oled(bool on);
    void set_page(uint8_t page);
    void set_column(uint8_t column);
    void set_point(uint8_t x, uint8_t y, bool on);
    void set_bitmap(uint8_t x, uint8_t y, const uint8_t *pData, uint8_t w, uint8_t h);
    void set_bitmap_sub(uint8_t x, uint8_t y, const uint8_t *pData, uint8_t w, uint8_t h, uint8_t off_x, uint8_t off_y, uint8_t sub_w, uint8_t sub_h);

    void send_command(uint8_t v);
    void send_data(uint8_t v);
    void send_data(const uint8_t *pData, size_t s);

    void clear_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

    void clear_part(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
    void show_part(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
    void show();
    void clear();

    struct Init
    {
        Init();
        ~Init();

        operator bool() const { return valid; }
    private:
        bool valid;
    };
}

#endif
