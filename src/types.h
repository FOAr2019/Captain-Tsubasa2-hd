#ifndef TYPES_H
#define TYPES_H

#include <tamtypes.h>

// ============================================
// BASIC TYPES
// ============================================
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed char    s8;
typedef signed short   s16;
typedef signed int     s32;

// ============================================
// GAME STATES
// ============================================
typedef enum {
    STATE_BOOT = 0,
    STATE_TITLE,
    STATE_MAIN_MENU,
    STATE_TEAM_SELECT,
    STATE_FORMATION,
    STATE_MATCH,
    STATE_BATTLE,
    STATE_SPECIAL_SHOT,
    STATE_GOAL,
    STATE_HALFTIME,
    STATE_GAMEOVER,
    STATE_VICTORY
} GameState;

// ============================================
// SPECIAL SHOT TYPES
// ============================================
typedef enum {
    SHOT_NORMAL = 0,
    SHOT_DRIVE,         // Tsubasa
    SHOT_TIGER,         // Hyuga
    SHOT_FIRE,          // Schneider
    SHOT_EAGLE,         // Misaki
    SHOT_OVERHEAD,      // Diaz
    SHOT_FALCON,        // Souda
    SHOT_ATOMIC,        // Hyuga (power up)
    SHOT_TWIN_SHOT,     // Tsubasa + Misaki
    SHOT_SKY,           // Tsubasa (super)
    SHOT_MAX
} ShotType;

// ============================================
// ACTION TYPES (Turn-based battle)
// ============================================
typedef enum {
    ACTION_NONE = 0,
    ACTION_SHOOT,
    ACTION_PASS,
    ACTION_DRIBBLE,
    ACTION_TACKLE,
    ACTION_INTERCEPT,
    ACTION_HEADER,
    ACTION_SPECIAL_SHOT,
    ACTION_GK_CATCH,
    ACTION_GK_PUNCH
} ActionType;

// ============================================
// PLAYER POSITION
// ============================================
typedef enum {
    POS_GK = 0,
    POS_DF,
    POS_MF,
    POS_FW
} Position;

// ============================================
// VECTOR 2D
// ============================================
typedef struct {
    s32 x;
    s32 y;
} Vec2;

// ============================================
// RECTANGLE
// ============================================
typedef struct {
    s32 x, y, w, h;
} Rect;

// ============================================
// COLOR
// ============================================
typedef struct {
    u8 r, g, b, a;
} Color;

#endif // TYPES_H
