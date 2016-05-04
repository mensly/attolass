#include "attolass.h"
#include "resources.h"

#ifdef MOCK_SDL
    #ifdef DEBUG
        #include <stdio.h>
        #define DEBUG_PRINT(x) printf(x)
    #else
        #define DEBUG_PRINT(x)
    #endif
    #include "Mockboy.h"
    Mockboy arduboy;
#else
    #ifdef DEBUG
        #define DEBUG_PRINT(x) Serial.println(x)
        #define START_SERIAL
    #else
        #define DEBUG_PRINT(x)
    #endif
    #include <Arduboy.h>
    Arduboy arduboy;
#endif

#define PGM_INCREMENT(pointer, value) pointer++; value = pgm_read_byte_near(pointer);

const coord_t PLAYER_POSITION_CENTER = (SCREEN_WIDTH - res_sprite_attolass_stand_width) / 2;

const level_t* levelStart;      // Pointer to the start of the current level's data
const level_t* level;           // Pointer to the first section to display in the current level
position_t levelPosition;       // Overall position of scroll through the current level
uint8_t sectionOffset;          // Offset between the levelPosition and start of the position of the first section
position_t playerX = PLAYER_POSITION_CENTER;
position_t playerY = SCREEN_HEIGHT / 2;
bool jumping = false;           // Currently in a jump
bool moving = false;            // Currently moving left/right
const uint8_t FPS = 30;
uint8_t frame = 0;              // Frame number that loops back to 0 when it reaches FPS

void updatePlayer() {
    if (arduboy.pressed(LEFT_BUTTON)) {
        if (levelPosition > 0) {
            levelPosition--;
        }
        if (playerX > 0) {
            playerX--;
        }
        moving = true;
        // TODO: Update level pointer and sectionOffset
    }
    else if (arduboy.pressed(RIGHT_BUTTON)) {
        playerX++;
        if (playerX >= PLAYER_POSITION_CENTER) {
            levelPosition++;
        }
        moving = true;
        // TODO: Update level pointer and sectionOffset
    }
    else {
        moving = false;
    }
    jumping = arduboy.pressed(A_BUTTON);
}

void drawLevel() {
    // TODO: Allow drawing using sectionOffset
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
                arduboy.fillRect(x - levelPosition, y, width, height, WHITE);
            }
            // Blocks in a section alternate between solid and clear
            drawSpace = !drawSpace;
            PGM_INCREMENT(pointer, current);
        }
        if (drawSpace) {
            // Draw remainder of screen
            arduboy.fillRect(x - levelPosition, 0, width, y, WHITE);
        }
        x += width;
    } while (current != 0 && x < SCREEN_WIDTH);
}

void drawPlayer() {
    position_t drawPlayerX = playerX - levelPosition;
    if (jumping) {
        arduboy.drawBitmap(drawPlayerX, playerY - 10, SPRITE(attolass_jump), BLACK);
    }
    else if (!moving) {
        arduboy.drawBitmap(drawPlayerX, playerY, SPRITE(attolass_stand), BLACK);
    }
    else if ((frame % 6) < 3) {
        arduboy.drawBitmap(drawPlayerX, playerY, SPRITE(attolass_walk_1), BLACK);
    }
    else {
        arduboy.drawBitmap(drawPlayerX, playerY, SPRITE(attolass_walk_2), BLACK);
    }
}

void setup() {
#ifdef START_SERIAL
    Serial.begin(9600);
#endif
    arduboy.begin();
    arduboy.setFrameRate(FPS);

    level = levelStart = res_level_concept;
}

void draw() {
    arduboy.clear();
    // Draw platforms of level
    drawLevel();
    // Character sprite
    drawPlayer();
    arduboy.display();
}

void loop() {
    // pause render until it's time for the next frame
    if (!(arduboy.nextFrame()))
        return;
    frame = (frame + 1) % FPS;
    // TODO: Allow scrolling level offset and redrawing
    // TODO: Allow character to move around and animate
    updatePlayer();
    draw();
}

