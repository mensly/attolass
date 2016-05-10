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

#define JUMP_BUTTON B_BUTTON
#define SHOOT_BUTTON A_BUTTON

const uint8_t CHARACTER_SIZE = 8;
#define MAX_Y_VELOCITY 6
#define APPLY_Y_VELOCITY (-MAX_Y_VELOCITY)
const coord_t PLAYER_POSITION_CENTER = (SCREEN_WIDTH - CHARACTER_SIZE) / 2;
const shot_t POSITION_NULL = {false, 0, {0, 0}};
#define MAX_SHOTS 3
#define SHOT_VELOCITY 3

const level_t* levelStart;      // Pointer to the start of the current level's data
const level_t* level;           // Pointer to the first section to display in the current level
position_t levelPosition;       // Overall position of scroll through the current level
uint8_t sectionOffset;          // Offset between the levelPosition and start of the position of the first section
const level_t* lastScreenSection;// Pointer to the first section to display on the current level's last screen
uint8_t lastScreenOffset;       // Offset to draw lastSCreenSection when at last screen
position_pair_t player = {      // Position of the player in the level
    PLAYER_POSITION_CENTER,
    SCREEN_HEIGHT / 4 };
shot_t shots[MAX_SHOTS] = {     // Current positions in the level of shots
    POSITION_NULL, POSITION_NULL, POSITION_NULL
};
int8_t yVelocity = 0;           // Current movement up or down
bool prevJumping = false;       // Previous frame was a jump
bool jumping = false;           // Currently in a jump
bool prevShooting = false;      // Previous frame had the shoot button held
bool falling = false;           // Currently falling
bool moving = false;            // Currently moving left/right
const uint8_t FPS = 30;         // FPS to run game at
uint8_t frame = 0;              // Frame number that loops back to 0 when it reaches FPS
bool flipDirection = false;     // Track facing (false=left, true=right)
const uint8_t HOVER_COUNT = 5;  // Number of frames to stay in the air the top of a jump
uint8_t hover = 0;              // Remaining frames to say in the air at the top of a current jump

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
        flipDirection = true;
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
        flipDirection = false;
    }
    else {
        moving = false;
    }
    // Calculate y velocity based on jump button
    jumping = arduboy.pressed(JUMP_BUTTON);
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

void updateShooting() {
    // Update positions
    for (uint8_t i = 0; i < MAX_SHOTS; i++) {
        if (shots[i].active) {
            shots[i].position.x += shots[i].velocity;
            // TODO: Collision detection with entities
            // Check bounds
            if (shots[i].position.x < levelPosition ||
                shots[i].position.x > levelPosition + SCREEN_WIDTH) {
                shots[i].active = false;
            }
        }
    }
    bool shooting = arduboy.pressed(SHOOT_BUTTON);
    if (shooting && !prevShooting) {
        // Find an available shot slot
        for (uint8_t i = 0; i < MAX_SHOTS; i++) {
            if (!shots[i].active) {
                // Initialise at the same location as the player, with a default velocity
                shots[i].active = true;
                if (flipDirection) {
                    shots[i].velocity = -SHOT_VELOCITY;
                    shots[i].position.x = player.x;
                }
                else {
                    shots[i].velocity = SHOT_VELOCITY;
                    shots[i].position.x = player.x + CHARACTER_SIZE;
                }
                shots[i].position.y = player.y + CHARACTER_SIZE / 2;
                break;
            }
        }
    }
    prevShooting = shooting;
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
        if (flipDirection) {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_jump_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_jump_right), BLACK);
        }
    }
    else if (!moving) {
        if (flipDirection) {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_stand_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_stand_right), BLACK);
        }
    }
    else if ((frame % 6) < 3) {
        if (flipDirection) {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_walk_1_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_walk_1_right), BLACK);
        }
    }
    else {
        if (flipDirection) {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_walk_2_left), BLACK);
        }
        else {
            arduboy.drawBitmap(drawPlayerX, player.y, SPRITE(attolass_walk_2_right), BLACK);
        }
    }
}

void drawShots() {
    position_t drawX, drawY;
    for (uint8_t i = 0; i < MAX_SHOTS; i++) {
        if (shots[i].active) {
            drawX = shots[i].position.x - levelPosition;
            drawY = shots[i].position.y;
            arduboy.drawPixel(drawX, drawY - 1, BLACK);
            arduboy.drawPixel(drawX - 1, drawY, BLACK);
            arduboy.drawPixel(drawX, drawY, (frame % 2) ? WHITE : BLACK);
            arduboy.drawPixel(drawX + 1, drawY, BLACK);
            arduboy.drawPixel(drawX, drawY + 1, BLACK);
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
    prevShooting = false;
    falling = false;
    moving = false;
    flipDirection = false;
    
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

void loop() {
    // pause render until it's time for the next frame
    if (!(arduboy.nextFrame()))
        return;
    frame = (frame + 1) % FPS;
    arduboy.clear();
    // Draw platforms of level, needed for collision detection for player etc
    drawLevel();
    
    updatePlayer();
    updateShooting();
    
    // Character sprite and projectiles
    drawPlayer();
    drawShots();
    
    // Update screen
    arduboy.display();
}

