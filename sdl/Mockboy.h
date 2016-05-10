#pragma once
#include <inttypes.h>
#include <chrono>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

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
    
    // Initialize SDL window
    bool initSDL();

    // Placeholder for Arbuboy's setup
    void begin();

    /// Clears display.
    void clear();

    /// Draws a filled-in rectangle.
    void fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color);

    /// Sets a single pixel on the screen buffer to white or black.
    void drawPixel(int x, int y, uint8_t color);

    uint8_t getPixel(uint8_t x, uint8_t y);

    /// Draws a bitmap from program memory to a specific X/Y
    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);

    void setFrameRate(uint8_t rate);
    bool nextFrame();

    /// Copies the contents of the screen buffer to the screen.
    void display();
    
    //Free resources and close SDL
    void close();
    
    void onKeyDown(SDL_Keycode sdlKeyCode);
    void onKeyUp(SDL_Keycode sdlKeyCode);
    
private:
    SDL_Window* window;
    SDL_Surface* surface;
    SDL_Renderer* renderer;
    uint8_t buttonsState;
    uint8_t getKeyFlag(SDL_Keycode sdlKeyCode);
    void setDrawColor(uint8_t color);
    
    time_t millis();
    
    uint8_t frameRate;
    uint8_t eachFrameMillis;
    time_t nextFrameStart;
    std::chrono::high_resolution_clock::time_point startTime;
    
    int32_t offsetX;
    int32_t offsetY;
    uint8_t scale;
    void toggleFullscreen();
};