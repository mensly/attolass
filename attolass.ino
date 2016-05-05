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

const uint8_t CHARACTER_SIZE = 8;
#define MAX_Y_VELOCITY 6
#define APPLY_Y_VELOCITY (-MAX_Y_VELOCITY)
const coord_t PLAYER_POSITION_CENTER = (SCREEN_WIDTH - CHARACTER_SIZE) / 2;

const level_t* levelStart;      // Pointer to the start of the current level's data
const level_t* level;           // Pointer to the first section to display in the current level
position_t levelPosition;       // Overall position of scroll through the current level
uint8_t sectionOffset;          // Offset between the levelPosition and start of the position of the first section
position_t playerX = PLAYER_POSITION_CENTER;
position_t playerY = SCREEN_HEIGHT / 2;
int8_t yVelocity = 0;
bool prevJumping = false;           // Previous frame was a jump
bool jumping = false;           // Currently in a jump
bool falling = false;           // Currently falling
bool moving = false;            // Currently moving left/right
const uint8_t FPS = 30;         // FPS to run game at
uint8_t frame = 0;              // Frame number that loops back to 0 when it reaches FPS
bool flipSprite = false;        // Track facing (false=left, true=right)
const uint8_t HOVER_COUNT = 5;
uint8_t hover = 0;

typedef enum {
    DirectionUp, DirectionDown, DirectionLeft, DirectionRight
} direction_t;

bool isAnyPixel(coord_t x, coord_t y, coord_t width, coord_t height, uint8_t color) {
    coord_t maxX = x + width;
    coord_t maxY = y + height;
    for (int i = x; i < maxX; i++) {
        for (int j = y; j < maxY; j++) {
            if (arduboy.getPixel(i, j) == color) {
                return true;
            }
        }
    }
    return false;
}

bool canMove(direction_t direction) {
    switch (direction) {
        case DirectionUp:
            return !isAnyPixel(playerX - levelPosition, playerY - 1, CHARACTER_SIZE, 1, BLACK);
        case DirectionDown:
            return !isAnyPixel(playerX - levelPosition, playerY + CHARACTER_SIZE, CHARACTER_SIZE, 1, BLACK);
        case DirectionRight:
            return !isAnyPixel(playerX - levelPosition + CHARACTER_SIZE, playerY, 1, CHARACTER_SIZE, BLACK);
        case DirectionLeft:
            return !isAnyPixel(playerX - levelPosition - 1, playerY, 1, CHARACTER_SIZE, BLACK);
        default:
            return false;
    }
    // Can fall if there are no black pixels below the character
}

void applyVelocity() {
    if (yVelocity < 0) {
        if (jumping) {
            for (uint8_t i = -yVelocity; i > 0; i--) {
                if (!canMove(DirectionUp)) {
                    yVelocity = 0;
                    hover = 0;
                    break;
                }
                playerY--;
            }
        }
        else {
            yVelocity = 0;
        }
    }
    else {
        for (uint8_t i = 0; i < yVelocity; i++) {
            if (!canMove(DirectionDown)) {
                yVelocity = 0;
                hover = 0;
                break;
            }
            playerY++;
            falling = true;
        }
    }
}

void updatePlayer() {
    prevJumping = jumping;
    if (arduboy.pressed(LEFT_BUTTON) && canMove(DirectionLeft)) {
        if (levelPosition > 0) {
            levelPosition--;
        }
        if (playerX > 0) {
            playerX--;
        }
        moving = true;
        flipSprite = true;
        // TODO: Update level pointer and sectionOffset
    }
    else if (arduboy.pressed(RIGHT_BUTTON) && canMove(DirectionRight)) {
        playerX++;
        if (playerX >= PLAYER_POSITION_CENTER) {
            levelPosition++;
        }
        moving = true;
        flipSprite = false;
        // TODO: Update level pointer and sectionOffset
    }
    else {
        moving = false;
    }
    // Calculate y velocity based on jump button
    jumping = arduboy.pressed(A_BUTTON);
    if (canMove(DirectionDown)) {
        if (yVelocity == 0 && hover > 0 && jumping) {
            hover--;
        }
        else {
            yVelocity++;
            if (yVelocity > MAX_Y_VELOCITY) {
                yVelocity = MAX_Y_VELOCITY;
            }
        }
    }
    else {
        if (jumping && !prevJumping) {
            hover = HOVER_COUNT;
            yVelocity = APPLY_Y_VELOCITY;
        }
        else {
            yVelocity = 0;
            hover = 0;
        }

    }
    falling = false;
    // Apply y velocity based on nearby blocks
    applyVelocity();
    falling |= canMove(DirectionDown);
}

void drawLevel() {
    // TODO: Allow drawing using sectionOffset
    const level_t* pointer = level;
    level_t current = pgm_read_byte_near(pointer);
    int16_t x = -levelPosition;
    coord_t y,width,height;
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
    position_t drawPlayerX = playerX - levelPosition;
    if (jumping || falling) {
        if (flipSprite) {
            arduboy.drawBitmap(drawPlayerX, playerY, SPRITE(attolass_jump_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, playerY, SPRITE(attolass_jump_right), BLACK);
        }
    }
    else if (!moving) {
        if (flipSprite) {
            arduboy.drawBitmap(drawPlayerX, playerY, SPRITE(attolass_stand_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, playerY, SPRITE(attolass_stand_right), BLACK);
        }
    }
    else if ((frame % 6) < 3) {
        if (flipSprite) {
            arduboy.drawBitmap(drawPlayerX, playerY, SPRITE(attolass_walk_1_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, playerY, SPRITE(attolass_walk_1_right), BLACK);
        }
    }
    else {
        if (flipSprite) {
            arduboy.drawBitmap(drawPlayerX, playerY, SPRITE(attolass_walk_2_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, playerY, SPRITE(attolass_walk_2_right), BLACK);
        }
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

