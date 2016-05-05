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
#define PGM_DECREMENT(pointer, value) pointer--; value = pgm_read_byte_near(pointer);

const uint8_t CHARACTER_SIZE = 8;
#define MAX_Y_VELOCITY 6
#define APPLY_Y_VELOCITY (-MAX_Y_VELOCITY)
const coord_t PLAYER_POSITION_CENTER = (SCREEN_WIDTH - CHARACTER_SIZE) / 2;

const level_t* levelStart;      // Pointer to the start of the current level's data
const level_t* level;           // Pointer to the first section to display in the current level
position_t levelPosition;       // Overall position of scroll through the current level
uint8_t sectionOffset;          // Offset between the levelPosition and start of the position of the first section
const level_t* lastScreenSection;      // Pointer to the first section to display on the current level's last screen
uint8_t lastScreenOffset;       // Offset to draw lastSCreenSection when at last screen
position_pair_t player = { PLAYER_POSITION_CENTER, SCREEN_HEIGHT / 4 };
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
        case DirectionUp: {
            return player.y > 0 && !isAnyPixel(player.x - levelPosition, player.y - 1, CHARACTER_SIZE, 1, BLACK);
        }
        case DirectionDown: {
            position_t pixelY = player.y + CHARACTER_SIZE;
            return pixelY >= SCREEN_HEIGHT ||
                !isAnyPixel(player.x - levelPosition, pixelY,
                            CHARACTER_SIZE, 1, BLACK);
        }
        case DirectionRight: {
            position_t pixelX = player.x - levelPosition + CHARACTER_SIZE;
            coord_t height = SCREEN_HEIGHT - player.y;
            if (height > CHARACTER_SIZE) {
                height = CHARACTER_SIZE;
            }
            return pixelX < SCREEN_WIDTH &&
                !isAnyPixel(player.x - levelPosition + CHARACTER_SIZE,
                            player.y, 1, height, BLACK);
        }
        case DirectionLeft: {
            position_t pixelX = player.x - levelPosition;
            coord_t height = SCREEN_HEIGHT - player.y;
            if (height > CHARACTER_SIZE) {
                height = CHARACTER_SIZE;
            }
            return pixelX > 0 && !isAnyPixel(pixelX - 1, player.y, 1, height, BLACK);
        }
        default:
            return false;
    }
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
                player.y--;
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
            player.y++;
            falling = true;
        }
    }
}

void updatePlayer() {
    if (player.y >= SCREEN_HEIGHT) {
        // Death by pit
        restartLevel();
        return;
    }
    prevJumping = jumping;
    if (arduboy.pressed(LEFT_BUTTON) && canMove(DirectionLeft)) {
        player.x--;
        if (player.x <= PLAYER_POSITION_CENTER + levelPosition && levelPosition > 0) {
            levelPosition--;
            if (sectionOffset == 0) {
                // Move to previous section
                level--; // This can never underflow due to the levelPosition variable check
                while (pgm_read_byte_near(level) & FLAG_BLOCK) {
                    level--;
                }
                // Read in length of new section to find new offset
                sectionOffset = (pgm_read_byte_near(level) & MASK_SECTION) - 1;
            }
            else {
                sectionOffset--;
            }
        }
        moving = true;
        flipSprite = true;
    }
    else if (arduboy.pressed(RIGHT_BUTTON) && canMove(DirectionRight)) {
        player.x++;
        if (player.x >= PLAYER_POSITION_CENTER &&
                (level < lastScreenSection || sectionOffset < lastScreenOffset)) {
            levelPosition++;
            sectionOffset++;
            // Check if at the end of this section
            if (sectionOffset == (pgm_read_byte_near(level) & MASK_SECTION)) {
                sectionOffset = 0;
                // Move to next section
                level++;
                while (pgm_read_byte_near(level) & FLAG_BLOCK) {
                    level++;
                }
            }
        }
        moving = true;
        flipSprite = false;
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
    const level_t* pointer = level;
    level_t current = pgm_read_byte_near(pointer);
    int16_t x = -sectionOffset;
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
    position_t drawPlayerX = player.x - levelPosition;
    if (falling) {
        if (flipSprite) {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_jump_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_jump_right), BLACK);
        }
    }
    else if (!moving) {
        if (flipSprite) {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_stand_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_stand_right), BLACK);
        }
    }
    else if ((frame % 6) < 3) {
        if (flipSprite) {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_walk_1_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_walk_1_right), BLACK);
        }
    }
    else {
        if (flipSprite) {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_walk_2_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_walk_2_right), BLACK);
        }
    }
}

void setLevel(const level_t* chosenLevel) {
    // Restart positions and player state for level
    levelPosition = 0;
    sectionOffset = 0;
    player.x = PLAYER_POSITION_CENTER;
    player.y = SCREEN_HEIGHT / 4;
    jumping = false;
    prevJumping = false;
    falling = false;
    moving = false;
    flipSprite = false;
    
    // Store level as the start and current position
    level = chosenLevel;
    levelStart = chosenLevel;
    
    // Calculate location of final screen
    // Start by finding end level
    lastScreenSection = level;
    uint8_t current = pgm_read_byte_near(lastScreenSection);
    do {
        PGM_INCREMENT(lastScreenSection, current);
    } while (current);
    // Now move back to allocate enough width for a full screen
    lastScreenOffset = 0; // Edge case of level small enough for levelStart == lastScreenSection
    position_t remaining = SCREEN_WIDTH;
    do {
        PGM_DECREMENT(lastScreenSection, current);
        if ((current & FLAG_BLOCK) == 0) { // Each that is not a block
            uint8_t sectionLength = (current & MASK_SECTION);
            if (remaining > sectionLength) {
                remaining -= sectionLength;
            }
            else {
                lastScreenOffset = sectionLength - remaining;
                remaining = 0;
            }
        }
    } while (remaining > 0 && lastScreenSection > levelStart);
    
}

void restartLevel() {
    // TODO: Decrement lives
    setLevel(levelStart);
}

void setup() {
#ifdef START_SERIAL
    Serial.begin(9600);
#endif
    arduboy.begin();
    arduboy.setFrameRate(FPS);

    setLevel(res_level_concept);
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
    updatePlayer();
    draw();
}

