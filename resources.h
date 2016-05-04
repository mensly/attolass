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
extern const level_t res_level_concept[];
#pragma endregion

#pragma region res_sprite
extern const uint8_t res_sprite_attolass_jump_right_data[];
#define res_sprite_attolass_jump_right_width 8
#define res_sprite_attolass_jump_right_height 8

extern const uint8_t res_sprite_attolass_jump_left_data[];
#define res_sprite_attolass_jump_left_width 8
#define res_sprite_attolass_jump_left_height 8

extern const uint8_t res_sprite_attolass_stand_right_data[];
#define res_sprite_attolass_stand_right_width 8
#define res_sprite_attolass_stand_right_height 8

extern const uint8_t res_sprite_attolass_stand_left_data[];
#define res_sprite_attolass_stand_left_width 8
#define res_sprite_attolass_stand_left_height 8

extern const uint8_t res_sprite_attolass_walk_1_right_data[];
#define res_sprite_attolass_walk_1_right_width 8
#define res_sprite_attolass_walk_1_right_height 8

extern const uint8_t res_sprite_attolass_walk_1_left_data[];
#define res_sprite_attolass_walk_1_left_width 8
#define res_sprite_attolass_walk_1_left_height 8

extern const uint8_t res_sprite_attolass_walk_2_right_data[];
#define res_sprite_attolass_walk_2_right_width 8
#define res_sprite_attolass_walk_2_right_height 8

extern const uint8_t res_sprite_attolass_walk_2_left_data[];
#define res_sprite_attolass_walk_2_left_width 8
#define res_sprite_attolass_walk_2_left_height 8

#pragma endregion
