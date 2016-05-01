#pragma once

// 0 in the first bit indicates the start of a section, the remaining
// 7 bits store the length of the section from 1 to 127 (0 would be a null byte
// meaning end of level)
#define MASK_SECTION  0b01111111
// 1 in the first bit indicates this represents a height at while the
// level switches from being solid and air
#define FLAG_BLOCK    0b10000000
// 1 in the second bit isn't currently used
#define FLAG_UNUSUED    0b01000000
// The final 6 bits store the height from 1
#define MASK_HEIGHT   0b00111111

typedef uint8_t level_t;
typedef uint8_t coord_t;
