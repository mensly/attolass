#include "resources.h"

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

PROGMEM const uint8_t res_sprite_attolass_stand_data[] = {
  0b00011000,
  0b00001110,
  0b10101111,
  0b11110011,
  0b01100101,
  0b11110001,
  0b10010101,
  0b00001110
};

PROGMEM const uint8_t res_sprite_attolass_jump_data[] = {
  0b00100000,
  0b00011100,
  0b01001110,
  0b11110001,
  0b01100011,
  0b11101001,
  0b00010011,
  0b00001110
};

