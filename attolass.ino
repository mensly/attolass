#include <Arduboy.h>
#include "attolass_types.h"
#include "resources.h"

#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x) Serial.println(x)
#else
 #define DEBUG_PRINT(x)
#endif

#define PGM_INCREMENT(pointer, value) pointer++; value = pgm_read_byte_near(pointer);

const coord_t SCREEN_WIDTH = 128;
const coord_t SCREEN_HEIGHT = 64;

Arduboy arduboy;
const level_t* level;
coord_t playerX = (SCREEN_WIDTH - res_sprite_attolass_stand_width) / 2;
coord_t playerY = SCREEN_HEIGHT / 2;

void drawLevel() {
  // TODO: Allow a pixel offset
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
}

void drawPlayer() {
  arduboy.drawBitmap(playerX, playerY, SPRITE(attolass_stand), BLACK);
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
  arduboy.clear();
  // Draw platforms of level
  drawLevel();
  // Character sprite
  drawPlayer();
  // TODO: Allow scrolling level offset and redrawing
  // TODO: Allow character to move around and animate
  arduboy.display();
}

