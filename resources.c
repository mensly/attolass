#include "resources.h"

#define FILLED_SECTION  MASK_SECTION
#define EMPTY_SECTION   MASK_SECTION, FLAG_BLOCK

#pragma region res_level
PROGMEM const level_t res_level_concept[] = {
    16  & MASK_SECTION,
    8   | FLAG_BLOCK,
    30  & MASK_SECTION,
    8   | FLAG_BLOCK,
    32   | FLAG_BLOCK,
    35   | FLAG_BLOCK,
    16  & MASK_SECTION,
    8   | FLAG_BLOCK,

    40  & MASK_SECTION,
    24  | FLAG_BLOCK,
    
    40  & MASK_SECTION,
    24  | FLAG_BLOCK,
    40  | FLAG_BLOCK,
    
    30 & MASK_SECTION,
    2   | FLAG_BLOCK,
    40  | FLAG_BLOCK,
    
    10 & MASK_SECTION,
    0 | FLAG_BLOCK,
    40 | FLAG_BLOCK,
    
    4  & MASK_SECTION,
    2   | FLAG_BLOCK,
    40  | FLAG_BLOCK,
    
    10  & MASK_SECTION,
    2  | FLAG_BLOCK,
    18 | FLAG_BLOCK,
    20  | FLAG_BLOCK,
    54  | FLAG_BLOCK,
    
    10  & MASK_SECTION,
    2  | FLAG_BLOCK,
    26 | FLAG_BLOCK,
    28  | FLAG_BLOCK,
    54  | FLAG_BLOCK,
    
    127 & MASK_SECTION,
    2   | FLAG_BLOCK,
    26  | FLAG_BLOCK,
    42  | FLAG_BLOCK,
    54  | FLAG_BLOCK,
    
    2   & FILLED_SECTION,
    
    30  & EMPTY_SECTION,

    0
};
#pragma endregion

#pragma region res_sprite
PROGMEM const uint8_t res_sprite_attolass_jump_right_data[] = {
    0b00100000, 
    0b00011100, 
    0b01001110, 
    0b11111001, 
    0b01100011, 
    0b11101001, 
    0b00010011, 
    0b00001110, 
};

PROGMEM const uint8_t res_sprite_attolass_jump_left_data[] = {
    0b00001110, 
    0b00010011, 
    0b11101001, 
    0b01100011, 
    0b11111001, 
    0b01001110, 
    0b00011100, 
    0b00100000, 
};

PROGMEM const uint8_t res_sprite_attolass_stand_right_data[] = {
    0b00011000, 
    0b00001110, 
    0b10101111, 
    0b11110011, 
    0b01100101, 
    0b11110001, 
    0b10010101, 
    0b00001110, 
};

PROGMEM const uint8_t res_sprite_attolass_stand_left_data[] = {
    0b00001110, 
    0b10010101, 
    0b11110001, 
    0b01100101, 
    0b11110011, 
    0b10101111, 
    0b00001110, 
    0b00011000, 
};

PROGMEM const uint8_t res_sprite_attolass_walk_1_right_data[] = {
    0b00001110, 
    0b10000110, 
    0b11001111, 
    0b01110011, 
    0b11100101, 
    0b11110001, 
    0b01010101, 
    0b00001110, 
};

PROGMEM const uint8_t res_sprite_attolass_walk_1_left_data[] = {
    0b00001110, 
    0b01010101, 
    0b11110001, 
    0b11100101, 
    0b01110011, 
    0b11001111, 
    0b10000110, 
    0b00001110, 
};

PROGMEM const uint8_t res_sprite_attolass_walk_2_right_data[] = {
    0b00000111, 
    0b11000110, 
    0b01001111, 
    0b01110011, 
    0b01100101, 
    0b11110001, 
    0b10110101, 
    0b00001110, 
};

PROGMEM const uint8_t res_sprite_attolass_walk_2_left_data[] = {
    0b00001110, 
    0b10110101, 
    0b11110001, 
    0b01100101, 
    0b01110011, 
    0b01001111, 
    0b11000110, 
    0b00000111, 
};

#pragma endregion
