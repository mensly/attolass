#include "resources.h"

#pragma region res_level
PROGMEM const level_t res_level_concept[] = {
  48  & MASK_SECTION,
  8   | FLAG_BLOCK,

  40  & MASK_SECTION,
  24  | FLAG_BLOCK,

  40  & MASK_SECTION,
  24  | FLAG_BLOCK,
  50  | FLAG_BLOCK,

  0
};
#pragma endregion

#pragma region res_sprite
PROGMEM const uint8_t res_sprite_attolass_jump_data[] = {
    0b00100000, 
    0b00011100, 
    0b01001110, 
    0b11111001, 
    0b01100011, 
    0b11101001, 
    0b00010011, 
    0b00001110, 
};

PROGMEM const uint8_t res_sprite_attolass_stand_data[] = {
    0b00011000, 
    0b00001110, 
    0b10101111, 
    0b11110011, 
    0b01100101, 
    0b11110001, 
    0b10010101, 
    0b00001110, 
};

PROGMEM const uint8_t res_sprite_attolass_walk_1_data[] = {
    0b00001110, 
    0b10000110, 
    0b11001111, 
    0b01110011, 
    0b11100101, 
    0b11110001, 
    0b01010101, 
    0b00001110, 
};

PROGMEM const uint8_t res_sprite_attolass_walk_2_data[] = {
    0b00000111, 
    0b11000110, 
    0b01001111, 
    0b01110011, 
    0b01100101, 
    0b11110001, 
    0b10110101, 
    0b00001110, 
};

#pragma endregion
