#pragma once
#include <inttypes.h>

#define BLACK 0 //< unlit pixel
#define WHITE 1 //< lit pixel

#define _BV(x) (1<<x)
#define LEFT_BUTTON _BV(5)
#define RIGHT_BUTTON _BV(6)
#define UP_BUTTON _BV(7)
#define DOWN_BUTTON _BV(4)
#define A_BUTTON _BV(3)
#define B_BUTTON _BV(2)

#define pgm_read_byte_near(x) *(x)

class Mockboy {
    
public:
    Mockboy();

    /// Returns true if the button mask passed in is pressed.
    bool pressed(uint8_t buttons);

    /// Initialize hardware, boot logo, boot utilities, etc.
    void begin();

    /// Clears display.
    void clear();

    /// Draws a filled-in rectangle.
    void fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);

    /// Draws a bitmap from program memory to a specific X/Y
    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);

    void setFrameRate(uint8_t rate);
    bool nextFrame();

    /// Copies the contents of the screen buffer to the screen.
    void display();
};