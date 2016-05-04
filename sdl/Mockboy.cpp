#ifdef MOCK_SDL
#include "../attolass.h"
#include "Mockboy.h"

Mockboy::Mockboy() {
    setFrameRate(60);
    buttonsState = 0;
    startTime = std::chrono::high_resolution_clock::now();
    nextFrameStart = 0;
}

/// Initialize hardware, boot logo, boot utilities, etc.
bool Mockboy::initSDL() {
    bool success = true;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else {
        // Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            printf( "Warning: Linear texture filtering not enabled!" );
        }
        
        //Create window
        window = SDL_CreateWindow("attolass", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   SCREEN_WIDTH * MOCK_SCALE, SCREEN_HEIGHT * MOCK_SCALE, SDL_WINDOW_SHOWN );
        if (window == NULL) {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else {
            //Create renderer for window
            surface = SDL_GetWindowSurface(window);
            renderer = SDL_CreateSoftwareRenderer(surface);
            if (renderer == NULL) {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else {
                //Initialize renderer color
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF );
            }
        }
    }
    return success;
}

void Mockboy::begin() { }

void Mockboy::close() {
    //Destroy window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    
    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

/// Clears display.
void Mockboy::clear() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
}

uint8_t Mockboy::getKeyFlag(SDL_Keycode sdlKeyCode) {
    switch (sdlKeyCode) {
        case SDLK_LEFT:
            return LEFT_BUTTON;
        case SDLK_RIGHT:
            return RIGHT_BUTTON;
        case SDLK_UP:
            return UP_BUTTON;
        case SDLK_DOWN:
            return DOWN_BUTTON;
        case SDLK_x:
            return A_BUTTON;
        case SDLK_z:
            return B_BUTTON;
        default:
            return 0;
    }
}

void Mockboy::onKeyDown(SDL_Keycode sdlKeyCode) {
    buttonsState |= getKeyFlag(sdlKeyCode);
}

void Mockboy::onKeyUp(SDL_Keycode sdlKeyCode) {
    buttonsState &= ~getKeyFlag(sdlKeyCode);
}

/// Returns true if the button mask passed in is pressed.
bool Mockboy::pressed(uint8_t buttons) {
    return (buttonsState & buttons) == buttons;
}

inline void Mockboy::setDrawColor(uint8_t color) {
    switch (color) {
        case WHITE:
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            break;
        default:
        case BLACK:
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            break;
    }
}

/// Draws a filled-in rectangle.
void Mockboy::fillRect(int16_t x, int16_t y, uint8_t w, uint8_t h, uint8_t color) {
    SDL_Rect fillRect = { x * MOCK_SCALE, y * MOCK_SCALE, w * MOCK_SCALE, h * MOCK_SCALE };
    setDrawColor(color);
    SDL_RenderFillRect(renderer, &fillRect);
}


/// Sets a single pixel on the screen buffer to white or black.
void Mockboy::drawPixel(int x, int y, uint8_t color) {
    fillRect(x, y, 1, 1, color);
}

uint8_t Mockboy::getPixel(uint8_t x, uint8_t y) {
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + (y * MOCK_SCALE) * surface->pitch + (x * MOCK_SCALE) * bpp;
    Uint32 pixel = 0;
    switch(bpp) {
        case 1:
            pixel = *p;
        case 2:
            pixel = *(Uint16 *)p;
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                pixel = p[0] << 16 | p[1] << 8 | p[2];
            else
                pixel = p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            pixel = *(Uint32 *)p;
    }
//    printf("bpp=%d\tpixel=%08x", bpp, pixel);
    return pixel == 0 ? BLACK : WHITE;
}

/// Draws a bitmap from program memory to a specific X/Y
void Mockboy::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color) {
    // TODO: More efficient implementation
    uint8_t heightBytes = h / 8 + (h % 8 > 0 ? 1 : 0);
    for (uint8_t i = 0; i < w; i++) {
        for (uint8_t j = 0; j < h; j++) {
            uint8_t yOffset = j / 8;
            uint8_t pixel = (*(bitmap + (i * heightBytes) + yOffset) >> (j - yOffset)) & 1;
            if (pixel) {
                drawPixel(x + i, y + j, color);
            }
        }
    }
}

void Mockboy::setFrameRate(uint8_t rate) {
    frameRate = rate;
    eachFrameMillis = 1000 / rate;
}

time_t Mockboy::millis() {
    auto time = std::chrono::high_resolution_clock::now() - startTime;
    return std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
}

bool Mockboy::nextFrame() {
    time_t now = millis();
    if (now < nextFrameStart) {
        return false;
    }
    nextFrameStart = now + eachFrameMillis;
    return true;
}

/// Copies the contents of the screen buffer to the screen.
void Mockboy::display() {
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);
}
#endif