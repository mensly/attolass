#include <Arduboy.h>
#include <avr/pgmspace.h>

#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x) Serial.println(x)
#else
 #define DEBUG_PRINT(x)
#endif

#define PGM_INCREMENT(pointer, value) pointer++; value = pgm_read_byte_near(pointer);

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

const coord_t SCREEN_WIDTH = 128;
const coord_t SCREEN_HEIGHT = 64;

Arduboy arduboy;
const level_t* level;

void drawLevel() {
  // TODO: Allow a pixel offset
  arduboy.clear();
  const level_t* pointer = level;
  level_t current = pgm_read_byte_near(pointer);
  coord_t x,y,width,height;
  x = 0;
  do {
    // Read in width of this section
    width = current & MASK_SECTION;
    y = SCREEN_HEIGHT;
    height = 0;
    PGM_INCREMENT(pointer, current);
    bool drawSpace = false;
    while (current & FLAG_BLOCK) {
      // Previous y coordinate is height from top of this block's bottom
      height = y;
      // Read height of this block
      y = SCREEN_HEIGHT - (current & MASK_HEIGHT);
      height = height - y;
      if (height > 0 && drawSpace) {
        arduboy.fillRect(x, y, width, height, WHITE);
      }
      // Blocks in a section alternate between solid and clear
      drawSpace = !drawSpace;
      PGM_INCREMENT(pointer, current);
    }
    if (drawSpace) {
      // Draw remainder of screen
      arduboy.fillRect(x, 0, width, y, WHITE);
    }
    x += width;
  } while (current != 0 && x < SCREEN_WIDTH);
  arduboy.display();
}

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  arduboy.begin();
  arduboy.setFrameRate(30);
  
  level = res_level_concept;
}

void loop() {
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;
  // Draw platforms of level
  drawLevel();
  // TODO: Allow scrolling level offset and redrawing
  // TODO: Add character
}

