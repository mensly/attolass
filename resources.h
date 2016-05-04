#pragma once
#ifdef MOCK_SDL
#define PROGMEM
#else
#include <avr/pgmspace.h>
#endif
#include "attolass.h"

extern const level_t res_level_concept[];

#define SPRITE(x) res_sprite_ ## x ## _data, \
  res_sprite_ ## x ## _width, res_sprite_ ## x ## _height

#define res_sprite_attolass_stand_width 8
#define res_sprite_attolass_stand_height 8
extern const uint8_t res_sprite_attolass_stand_data[];

#define res_sprite_attolass_jump_width 8
#define res_sprite_attolass_jump_height 8
extern const uint8_t res_sprite_attolass_jump_data[];

