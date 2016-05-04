#ifdef MOCK_SDL
#include "../attolass.h"
#include "Mockboy.h"
#include <iostream>

SDL_Window* gWindow = NULL;
//SDL_Renderer* gRenderer = NULL;

extern Mockboy arduboy;

int main(int argc, const char * argv[]) {
    //Start up SDL and create window
    if (!arduboy.initSDL()) {
        printf("Failed to initialize!\n");
    }
    else {
        bool quit = false;
        SDL_Event e;
        arduboy.clear();
        setup();
        do {
            while (SDL_PollEvent(&e) != 0)
            {
                switch (e.type) {
                    case SDL_QUIT:
                        quit = true;
                        break;
                    case SDL_KEYDOWN:
                        if (e.key.keysym.sym == SDLK_ESCAPE) {
                            quit = true;
                        }
                        else {
                            arduboy.onKeyDown(e.key.keysym.sym);
                        }
                        break;
                    case SDL_KEYUP:
                        arduboy.onKeyUp(e.key.keysym.sym);
                        break;
                }
            }
            loop();
        } while (!quit);
        
    }
    arduboy.close();
    return 0;
}
#endif