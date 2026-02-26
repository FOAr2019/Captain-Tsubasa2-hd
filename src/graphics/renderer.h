#ifndef RENDERER_H
#define RENDERER_H

#include "../types.h"

// ============================================
// SCREEN CONSTANTS
// ============================================
#define SCREEN_W    640
#define SCREEN_H    480
#define SCREEN_BPP  32

// ============================================
// COLORS (RGBA)
// ============================================
#define COL_BLACK    0x00000080
#define COL_WHITE    0xFFFFFF80
#define COL_RED      0xFF000080
#define COL_GREEN    0x00FF0080
#define COL_BLUE     0x0000FF80
#define COL_YELLOW   0xFFFF0080
#define COL_CYAN     0x00FFFF80
#define COL_GRAY     0x80808080
#define COL_DARKGRAY 0x40404080

// Field colors
#define COL_GRASS    0x226B2280
#define COL_GRASS2   0x1A581A80
#define COL_LINEWHT  0xFFFFFF80
#define COL_GOAL     0xCCCCCC80

// UI colors
#define COL_UI_BG    0x00000060
#define COL_UI_SEL   0xFFAA0080
#define COL_UI_TXT   0xFFFFFF80
#define COL_HP_GREEN 0x00DD0080
#define COL_HP_RED   0xDD000080

// ============================================
// SPRITE FRAME (for animation)
// ============================================
typedef struct {
    s16 u, v;   // texture coords
    s16 w, h;   // width/height
} SpriteFrame;

// ============================================
// TEXTURE
// ============================================
typedef struct {
    u32  vram_addr;
    u16  width;
    u16  height;
    u8   psm;
    u8   loaded;
} Texture;

// ============================================
// SPRITE
// ============================================
typedef struct {
    Texture*     tex;
    SpriteFrame* frames;
    int          frame_count;
    int          cur_frame;
    int          frame_timer;
    int          frame_delay; // ticks per frame
    int          x, y;
    int          flip_h;
    Color        tint;
} Sprite;

// ============================================
// FONT (simple bitmap font)
// ============================================
typedef struct {
    Texture* tex;
    int char_w;
    int char_h;
} BitmapFont;

// ============================================
// RENDERER FUNCTIONS
// ============================================
void renderer_init(void);
void renderer_begin_frame(void);
void renderer_end_frame(void);
void renderer_vsync(void);

// Primitives
void draw_rect_filled(int x, int y, int w, int h, u32 color);
void draw_rect_outline(int x, int y, int w, int h, u32 color);
void draw_line(int x1, int y1, int x2, int y2, u32 color);

// Text (built-in fallback font)
void draw_text(int x, int y, const char* str, u32 color);
void draw_text_center(int y, const char* str, u32 color);
void draw_number(int x, int y, int num, u32 color);

// Sprite
void draw_sprite_frame(Sprite* spr);
void sprite_update(Sprite* spr);

// UI panels
void draw_panel(int x, int y, int w, int h, const char* title);
void draw_healthbar(int x, int y, int w, int cur, int max, u32 color);

// Field
void draw_field(void);
void draw_field_zone_indicator(int zone);

// Score HUD
void draw_hud(const char* home, const char* away, int hs, int as, int minute);

// Battle UI
void draw_battle_menu(int selection, int can_special);
void draw_special_menu(void* player, int selection);
void draw_battle_result(const char* msg, int result);

// Fullscreen screens
void draw_title_screen(int blink);
void draw_team_select(void* teams, int team_count, int selection);
void draw_halftime_screen(const char* h, const char* a, int hs, int as);
void draw_fulltime_screen(const char* h, const char* a, int hs, int as);
void draw_goal_screen(const char* scorer);

#endif // RENDERER_H
