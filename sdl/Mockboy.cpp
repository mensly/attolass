#ifdef MOCK_SDL
#include "Mockboy.h"

Mockboy::Mockboy() {
}

/// Returns true if the button mask passed in is pressed.
bool Mockboy::pressed(uint8_t buttons) {
    return false;
}

/// Initialize hardware, boot logo, boot utilities, etc.
void Mockboy::begin() {
}

/// Clears display.
void Mockboy::clear() {
}

/// Draws a filled-in rectangle.
void Mockboy::fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color) {
}

/// Draws a bitmap from program memory to a specific X/Y
void Mockboy::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color) {
}

void Mockboy::setFrameRate(uint8_t rate) {
}
bool Mockboy::nextFrame() {
    return true;
}

/// Copies the contents of the screen buffer to the screen.
void Mockboy::display() {
}
#endif