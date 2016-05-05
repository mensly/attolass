#pragma once
#ifdef MOCK_SDL
#define PROGMEM
#else
#include <avr/pgmspace.h>
#endif
#include "attolass.h"

#define SPRITE(x) res_sprite_ ## x ## _data, \
  res_sprite_ ## x ## _width, res_sprite_ ## x ## _height

#pragma region res_level
#pragma endregion

#pragma region res_sprite
#pragma endregion
