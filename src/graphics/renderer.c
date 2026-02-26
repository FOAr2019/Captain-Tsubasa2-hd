#include "renderer.h"
#include "../game/player.h"
#include <graph.h>
#include <draw.h>
#include <draw2d.h>
#include <dma.h>
#include <gs_psm.h>
#include <string.h>
#include <stdio.h>

// ============================================
// INTERNAL STATE
// ============================================
static framebuffer_t  s_fb[2];
static zbuffer_t      s_zb;
static int            s_cur_fb = 0;
static packet_t*      s_packet;
static qword_t*       s_q;

// Simple 8x8 ASCII font data (basic chars 32-126)
// Each char = 8 bytes (8 rows of 8 bits)
static const u8 FONT8x8[95][8] = {
    // Space (32)
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // ! (33)
    {0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00},
    // " (34)
    {0x66,0x66,0x66,0x00,0x00,0x00,0x00,0x00},
    // # (35)
    {0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00},
    // $ (36)
    {0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00},
    // % (37)
    {0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00},
    // & (38)
    {0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0x00},
    // ' (39)
    {0x06,0x06,0x03,0x00,0x00,0x00,0x00,0x00},
    // ( (40)
    {0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0x00},
    // ) (41)
    {0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0x00},
    // * (42)
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00},
    // + (43)
    {0x00,0x0C,0x0C,0x3F,0x0C,0x0C,0x00,0x00},
    // , (44)
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x06},
    // - (45)
    {0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00},
    // . (46)
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00},
    // / (47)
    {0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00},
    // 0 (48)
    {0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00},
    // 1 (49)
    {0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0x00},
    // 2 (50)
    {0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0x00},
    // 3 (51)
    {0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0x00},
    // 4 (52)
    {0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0x00},
    // 5 (53)
    {0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0x00},
    // 6 (54)
    {0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0x00},
    // 7 (55)
    {0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0x00},
    // 8 (56)
    {0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0x00},
    // 9 (57)
    {0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0x00},
    // : (58)
    {0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x00},
    // ; (59)
    {0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x06},
    // < (60)
    {0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0x00},
    // = (61)
    {0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00},
    // > (62)
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00},
    // ? (63)
    {0x1E,0x33,0x30,0x18,0x0C,0x00,0x0C,0x00},
    // @ (64)
    {0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0x00},
    // A (65)
    {0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00},
    // B (66)
    {0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00},
    // C (67)
    {0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0x00},
    // D (68)
    {0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00},
    // E (69)
    {0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0x00},
    // F (70)
    {0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0x00},
    // G (71)
    {0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0x00},
    // H (72)
    {0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00},
    // I (73)
    {0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00},
    // J (74)
    {0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00},
    // K (75)
    {0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0x00},
    // L (76)
    {0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0x00},
    // M (77)
    {0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00},
    // N (78)
    {0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00},
    // O (79)
    {0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0x00},
    // P (80)
    {0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00},
    // Q (81)
    {0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0x00},
    // R (82)
    {0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00},
    // S (83)
    {0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0x00},
    // T (84)
    {0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0x00},
    // U (85)
    {0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00},
    // V (86)
    {0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00},
    // W (87)
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00},
    // X (88)
    {0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00},
    // Y (89)
    {0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0x00},
    // Z (90)
    {0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00},
    // [ (91)
    {0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00},
    // \ (92)
    {0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00},
    // ] (93)
    {0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00},
    // ^ (94)
    {0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00},
    // _ (95)
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF},
    // ` (96)
    {0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,0x00},
    // a (97)
    {0x00,0x00,0x1E,0x30,0x3E,0x33,0x6E,0x00},
    // b (98)
    {0x07,0x06,0x06,0x3E,0x66,0x66,0x3B,0x00},
    // c (99)
    {0x00,0x00,0x1E,0x33,0x03,0x33,0x1E,0x00},
    // d (100)
    {0x38,0x30,0x30,0x3e,0x33,0x33,0x6E,0x00},
    // e (101)
    {0x00,0x00,0x1E,0x33,0x3f,0x03,0x1E,0x00},
    // f (102)
    {0x1C,0x36,0x06,0x0f,0x06,0x06,0x0F,0x00},
    // g (103)
    {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x1F},
    // h (104)
    {0x07,0x06,0x36,0x6E,0x66,0x66,0x67,0x00},
    // i (105)
    {0x0C,0x00,0x0E,0x0C,0x0C,0x0C,0x1E,0x00},
    // j (106)
    {0x30,0x00,0x30,0x30,0x30,0x33,0x33,0x1E},
    // k (107)
    {0x07,0x06,0x66,0x36,0x1E,0x36,0x67,0x00},
    // l (108)
    {0x0E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00},
    // m (109)
    {0x00,0x00,0x33,0x7F,0x7F,0x6B,0x63,0x00},
    // n (110)
    {0x00,0x00,0x1F,0x33,0x33,0x33,0x33,0x00},
    // o (111)
    {0x00,0x00,0x1E,0x33,0x33,0x33,0x1E,0x00},
    // p (112)
    {0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F},
    // q (113)
    {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78},
    // r (114)
    {0x00,0x00,0x3B,0x6E,0x66,0x06,0x0F,0x00},
    // s (115)
    {0x00,0x00,0x3E,0x03,0x1E,0x30,0x1F,0x00},
    // t (116)
    {0x08,0x0C,0x3E,0x0C,0x0C,0x2C,0x18,0x00},
    // u (117)
    {0x00,0x00,0x33,0x33,0x33,0x33,0x6E,0x00},
    // v (118)
    {0x00,0x00,0x33,0x33,0x33,0x1E,0x0C,0x00},
    // w (119)
    {0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00},
    // x (120)
    {0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00},
    // y (121)
    {0x00,0x00,0x33,0x33,0x33,0x3E,0x30,0x1F},
    // z (122)
    {0x00,0x00,0x3F,0x19,0x0C,0x26,0x3F,0x00},
    // { (123)
    {0x38,0x0C,0x0C,0x07,0x0C,0x0C,0x38,0x00},
    // | (124)
    {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00},
    // } (125)
    {0x07,0x0C,0x0C,0x38,0x0C,0x0C,0x07,0x00},
    // ~ (126)
    {0x6E,0x3B,0x00,0x00,0x00,0x00,0x00,0x00},
};

// ============================================
// INIT
// ============================================
void renderer_init(void) {
    dma_channel_initialize(DMA_CHANNEL_GIF, NULL, 0);
    dma_channel_fast_waits(DMA_CHANNEL_GIF);

    // Setup framebuffers (double buffer)
    s_fb[0].width   = SCREEN_W;
    s_fb[0].height  = SCREEN_H;
    s_fb[0].mask    = 0;
    s_fb[0].psm     = GS_PSM_32;
    s_fb[0].address = graph_vram_allocate(
        SCREEN_W, SCREEN_H, GS_PSM_32, GRAPH_ALIGN_PAGE);

    s_fb[1].width   = SCREEN_W;
    s_fb[1].height  = SCREEN_H;
    s_fb[1].mask    = 0;
    s_fb[1].psm     = GS_PSM_32;
    s_fb[1].address = graph_vram_allocate(
        SCREEN_W, SCREEN_H, GS_PSM_32, GRAPH_ALIGN_PAGE);

    // Z-buffer
    s_zb.enable  = DRAW_DISABLE;
    s_zb.mask    = 0;
    s_zb.method  = ZTEST_METHOD_GREATER_EQUAL;
    s_zb.zsm     = GS_ZBUF_32;
    s_zb.address = graph_vram_allocate(
        SCREEN_W, SCREEN_H, GS_ZBUF_32, GRAPH_ALIGN_PAGE);

    graph_set_mode(GRAPH_MODE_PROGRESSIVE,
                   GRAPH_MODE_HDTV_480P,
                   GRAPH_MODE_FRAME,
                   GRAPH_ENABLE);
    graph_set_screen(0, 0, SCREEN_W, SCREEN_H);
    graph_set_bgcolor(0, 0, 0);
    graph_set_framebuffer_filtered(s_fb[0].address, s_fb[0].width,
                                   s_fb[0].psm, 0, 0);
    graph_enable_output();

    s_packet = packet_init(50, PACKET_NORMAL);
    s_cur_fb = 0;
}

void renderer_begin_frame(void) {
    s_q = packet_data(s_packet);
    // ລ້າງໜ້າຈໍ
    s_q = draw_setup_environment(s_q, 0, &s_fb[s_cur_fb], &s_zb);
    s_q = draw_primitive_xyoffset(s_q, 0,
        (2048.0f - SCREEN_W/2.0f),
        (2048.0f - SCREEN_H/2.0f));
    s_q = draw_clear(s_q, 0,
        2048.0f - SCREEN_W/2.0f,
        2048.0f - SCREEN_H/2.0f,
        SCREEN_W, SCREEN_H,
        0, 0, 0);
}

void renderer_end_frame(void) {
    s_q = draw_finish(s_q);
    dma_wait_fast();
    dma_channel_send_normal(DMA_CHANNEL_GIF,
        s_packet->data,
        s_q - s_packet->data, 0, 0);
    draw_wait_finish();
    graph_set_framebuffer_filtered(s_fb[s_cur_fb].address,
        s_fb[s_cur_fb].width, s_fb[s_cur_fb].psm, 0, 0);
    s_cur_fb ^= 1;
}

void renderer_vsync(void) {
    graph_wait_vsync();
}

// ============================================
// DRAW RECTANGLE (filled)
// ============================================
void draw_rect_filled(int x, int y, int w, int h, u32 color) {
    prim_t prim;
    prim.type    = PRIM_SPRITE;
    prim.shading = PRIM_SHADE_FLAT;
    prim.mapping = DRAW_DISABLE;
    prim.fogging = DRAW_DISABLE;
    prim.blending= DRAW_ENABLE;
    prim.antialiasing = DRAW_DISABLE;
    prim.mapping_type = PRIM_MAP_ST;
    prim.colorfix= PRIM_UNFIXED;

    color_t col;
    col.r = (color >> 24) & 0xFF;
    col.g = (color >> 16) & 0xFF;
    col.b = (color >>  8) & 0xFF;
    col.a = (color)       & 0xFF;
    col.q = 1.0f;

    vertex_t v0, v1;
    v0.x = (int)((2048 - SCREEN_W/2 + x) * 16.0f);
    v0.y = (int)((2048 - SCREEN_H/2 + y) * 16.0f);
    v0.z = 0xFF;
    v1.x = (int)((2048 - SCREEN_W/2 + x + w) * 16.0f);
    v1.y = (int)((2048 - SCREEN_H/2 + y + h) * 16.0f);
    v1.z = 0xFF;

    s_q = draw_sprite(s_q, 0, &prim, &col, &v0, &v1);
}

// ============================================
// DRAW TEXT (using 8x8 bitmap font)
// Each pixel drawn as a 2x2 block for HD look
// ============================================
void draw_text(int x, int y, const char* str, u32 color) {
    int cx = x;
    int scale = 2; // 2x scale for HD look

    while(*str) {
        char c = *str++;
        if(c < 32 || c > 126) { cx += 8 * scale; continue; }
        const u8* glyph = FONT8x8[c - 32];
        for(int row = 0; row < 8; row++) {
            for(int col = 0; col < 8; col++) {
                if(glyph[row] & (1 << (7 - col))) {
                    draw_rect_filled(
                        cx + col*scale,
                        y  + row*scale,
                        scale, scale, color);
                }
            }
        }
        cx += 8 * scale;
    }
}

void draw_text_center(int y, const char* str, u32 color) {
    int len = 0;
    const char* s = str;
    while(*s++) len++;
    int x = (SCREEN_W - len * 16) / 2;
    draw_text(x, y, str, color);
}

void draw_number(int x, int y, int num, u32 color) {
    char buf[16];
    // Simple int to str
    if(num == 0) {
        draw_text(x, y, "0", color);
        return;
    }
    int i = 0;
    char tmp[16];
    if(num < 0) { draw_text(x, y, "-", color); x += 16; num = -num; }
    while(num > 0) { tmp[i++] = '0' + (num % 10); num /= 10; }
    // reverse
    int j = 0;
    for(int k = i-1; k >= 0; k--) buf[j++] = tmp[k];
    buf[j] = '\0';
    draw_text(x, y, buf, color);
}

// ============================================
// DRAW FIELD
// ============================================
void draw_field(void) {
    // Background grass (alternate stripes)
    for(int i = 0; i < 8; i++) {
        u32 col = (i % 2 == 0) ? COL_GRASS : COL_GRASS2;
        draw_rect_filled(0, i * 60, SCREEN_W, 60, col);
    }

    // Field lines (white)
    // Border
    draw_rect_outline(20, 20, SCREEN_W-40, SCREEN_H-80, COL_LINEWHT);
    // Center line
    draw_rect_filled(SCREEN_W/2 - 1, 20, 2, SCREEN_H-80, COL_LINEWHT);
    // Center circle (simplified as square)
    draw_rect_outline(SCREEN_W/2 - 50, SCREEN_H/2 - 40 - 40,
                      100, 100, COL_LINEWHT);
    // Penalty areas
    draw_rect_outline(20, 80, 100, 180, COL_LINEWHT);
    draw_rect_outline(SCREEN_W-120, 80, 100, 180, COL_LINEWHT);
    // Goals
    draw_rect_filled(0, 150, 20, 90, COL_GOAL);
    draw_rect_filled(SCREEN_W-20, 150, 20, 90, COL_GOAL);
}

void draw_rect_outline(int x, int y, int w, int h, u32 color) {
    draw_rect_filled(x, y, w, 2, color);         // top
    draw_rect_filled(x, y+h-2, w, 2, color);     // bottom
    draw_rect_filled(x, y, 2, h, color);         // left
    draw_rect_filled(x+w-2, y, 2, h, color);     // right
}

void draw_line(int x1, int y1, int x2, int y2, u32 color) {
    // Simplified: only horizontal or vertical
    if(y1 == y2)
        draw_rect_filled(x1, y1, x2-x1, 2, color);
    else if(x1 == x2)
        draw_rect_filled(x1, y1, 2, y2-y1, color);
}

// ============================================
// DRAW HUD
// ============================================
void draw_hud(const char* home, const char* away,
              int hs, int as, int minute) {
    // HUD background
    draw_rect_filled(0, SCREEN_H-70, SCREEN_W, 70, COL_UI_BG);
    draw_rect_filled(0, SCREEN_H-72, SCREEN_W, 2, COL_WHITE);

    // Score
    char score_buf[16];
    // home name
    draw_text(10, SCREEN_H-60, home, COL_WHITE);
    // score
    {
        char s[8];
        s[0] = '0' + hs; s[1] = '-'; s[2] = '0' + as; s[3] = '\0';
        draw_text_center(SCREEN_H-60, s, COL_YELLOW);
    }
    // away name
    {
        int len = 0; const char* tmp = away;
        while(*tmp++) len++;
        draw_text(SCREEN_W - 10 - len*16, SCREEN_H-60, away, COL_WHITE);
    }

    // Minute
    {
        char m[16];
        int mi = minute;
        m[0] = '0' + mi/10; m[1] = '0' + mi%10;
        m[2] = '\''; m[3] = '\0';
        draw_text_center(SCREEN_H-35, m, COL_CYAN);
    }
}

// ============================================
// BATTLE MENU
// ============================================
void draw_battle_menu(int selection, int can_special) {
    int px = 10, py = 20;
    int pw = 180, ph = 130;

    draw_rect_filled(px, py, pw, ph, COL_UI_BG);
    draw_rect_outline(px, py, pw, ph, COL_WHITE);

    const char* options[] = {
        "Shoot", "Pass", "Dribble", "Special Shot"
    };
    for(int i = 0; i < 4; i++) {
        u32 col = (i == selection) ? COL_UI_SEL : COL_UI_TXT;
        if(i == 3 && !can_special) col = COL_GRAY;
        if(i == selection)
            draw_rect_filled(px+4, py + 8 + i*28, pw-8, 24, 0x33330080);
        draw_text(px+12, py+12 + i*28, options[i], col);
    }
}

// ============================================
// SPECIAL SHOT MENU
// ============================================
void draw_special_menu(void* player_ptr, int selection) {
    Player* p = (Player*)player_ptr;
    int px = 10, py = 20;
    int pw = 220, ph = 30 + p->special_count * 32;

    draw_rect_filled(px, py, pw, ph, COL_UI_BG);
    draw_rect_outline(px, py, pw, ph, COL_WHITE);
    draw_text(px+8, py+6, "Special Shots:", COL_YELLOW);

    for(int i = 0; i < p->special_count; i++) {
        u32 col = (i == selection) ? COL_UI_SEL : COL_UI_TXT;
        // Check stamina
        int can = player_can_special(p, i);
        if(!can) col = COL_GRAY;
        if(i == selection)
            draw_rect_filled(px+4, py+28 + i*32, pw-8, 28, 0x33330080);
        draw_text(px+12, py+32 + i*32, p->special[i].name, col);
        // Stamina cost
        char cost[8];
        cost[0] = 'S'; cost[1] = 'P'; cost[2] = ':';
        int c = p->special[i].stamina_cost;
        cost[3] = '0' + c/10; cost[4] = '0' + c%10; cost[5] = '\0';
        draw_text(px+pw-70, py+32 + i*32, cost, COL_CYAN);
    }
}

// ============================================
// STAMINA BAR
// ============================================
void draw_healthbar(int x, int y, int w, int cur, int max, u32 color) {
    draw_rect_filled(x, y, w, 12, COL_DARKGRAY);
    int fill = (cur * w) / (max > 0 ? max : 1);
    if(fill > 0) draw_rect_filled(x, y, fill, 12, color);
    draw_rect_outline(x, y, w, 12, COL_WHITE);
}

// ============================================
// BATTLE RESULT
// ============================================
void draw_battle_result(const char* msg, int result) {
    u32 col;
    switch(result) {
        case 3: col = COL_YELLOW; break;  // GOAL
        case 4: col = COL_RED;    break;  // SAVED
        case 1: col = COL_GREEN;  break;  // SUCCESS
        default: col = COL_WHITE; break;
    }
    // Big banner
    draw_rect_filled(SCREEN_W/2 - 160, SCREEN_H/2 - 20,
                     320, 50, COL_UI_BG);
    draw_rect_outline(SCREEN_W/2 - 160, SCREEN_H/2 - 20,
                      320, 50, col);
    draw_text_center(SCREEN_H/2 - 8, msg, col);
}

// ============================================
// TITLE SCREEN
// ============================================
void draw_title_screen(int blink) {
    // Background
    draw_rect_filled(0, 0, SCREEN_W, SCREEN_H, 0x00224480);
    // Title
    draw_text_center(80,  "CAPTAIN TSUBASA II", COL_YELLOW);
    draw_text_center(110, "SUPER STRIKE", COL_WHITE);
    draw_text_center(140, "HD REMAKE", COL_CYAN);
    // Subtitle
    draw_text_center(200, "Fan-made Remake", COL_GRAY);
    // Blink prompt
    if(blink / 30 % 2 == 0)
        draw_text_center(300, "Press START", COL_WHITE);
    // Footer
    draw_text_center(420, "Inspired by Konami NES classic", COL_GRAY);
}

// ============================================
// TEAM SELECT SCREEN
// ============================================
void draw_team_select(void* teams_ptr, int team_count, int selection) {
    Team* teams = (Team*)teams_ptr;
    draw_rect_filled(0, 0, SCREEN_W, SCREEN_H, 0x00002280);
    draw_text_center(20, "SELECT YOUR TEAM", COL_YELLOW);

    for(int i = 0; i < team_count; i++) {
        int py = 80 + i * 60;
        u32 bg = (i == selection) ? 0x0044AA80 : COL_UI_BG;
        draw_rect_filled(SCREEN_W/2 - 150, py, 300, 50, bg);
        draw_rect_outline(SCREEN_W/2 - 150, py, 300, 50,
            (i == selection) ? COL_YELLOW : COL_GRAY);
        draw_text_center(py + 16, teams[i].name, COL_WHITE);
    }

    draw_text_center(400, "X = Select", COL_GRAY);
}

// ============================================
// HALFTIME / FULLTIME SCREENS
// ============================================
void draw_halftime_screen(const char* h, const char* a, int hs, int as) {
    draw_rect_filled(0, 0, SCREEN_W, SCREEN_H, COL_UI_BG);
    draw_text_center(100, "HALF TIME", COL_YELLOW);
    draw_text_center(180, h, COL_WHITE);
    {
        char s[8];
        s[0] = '0' + hs; s[1] = '-'; s[2] = '0' + as; s[3] = '\0';
        draw_text_center(220, s, COL_CYAN);
    }
    draw_text_center(260, a, COL_WHITE);
    draw_text_center(350, "Press X to continue", COL_GRAY);
}

void draw_fulltime_screen(const char* h, const char* a, int hs, int as) {
    draw_rect_filled(0, 0, SCREEN_W, SCREEN_H, COL_UI_BG);
    draw_text_center(80, "FULL TIME", COL_YELLOW);

    draw_text_center(160, h, COL_WHITE);
    {
        char s[8];
        s[0] = '0' + hs; s[1] = '-'; s[2] = '0' + as; s[3] = '\0';
        draw_text_center(200, s, COL_CYAN);
    }
    draw_text_center(240, a, COL_WHITE);

    const char* result;
    if(hs > as)       result = "YOU WIN!!!";
    else if(as > hs)  result = "YOU LOSE...";
    else              result = "DRAW!";

    u32 rcol = (hs > as) ? COL_YELLOW : (as > hs) ? COL_RED : COL_CYAN;
    draw_text_center(320, result, rcol);
    draw_text_center(400, "Press X to return", COL_GRAY);
}

void draw_goal_screen(const char* scorer) {
    // Flash yellow
    draw_rect_filled(0, 0, SCREEN_W, SCREEN_H, 0xFFCC0040);
    draw_text_center(150, "G O A L !!!", COL_WHITE);
    draw_text_center(220, scorer, COL_YELLOW);
}

// Sprite update (animation)
void sprite_update(Sprite* spr) {
    if(!spr || !spr->frames) return;
    spr->frame_timer++;
    if(spr->frame_timer >= spr->frame_delay) {
        spr->frame_timer = 0;
        spr->cur_frame = (spr->cur_frame + 1) % spr->frame_count;
    }
}

void draw_sprite_frame(Sprite* spr) {
    if(!spr || !spr->tex) return;
    // Placeholder: draw colored box for sprite position
    draw_rect_filled(spr->x, spr->y,
        spr->frames[spr->cur_frame].w,
        spr->frames[spr->cur_frame].h,
        COL_WHITE);
}

void draw_panel(int x, int y, int w, int h, const char* title) {
    draw_rect_filled(x, y, w, h, COL_UI_BG);
    draw_rect_outline(x, y, w, h, COL_WHITE);
    if(title) draw_text(x + 8, y + 6, title, COL_YELLOW);
}

void draw_field_zone_indicator(int zone) {
    const char* zone_names[] = {
        "Own Goal", "Defense", "Midfield", "Attack", "Opp Goal"
    };
    if(zone < 0 || zone > 4) return;
    draw_text(SCREEN_W/2 - 60, 5, zone_names[zone], COL_CYAN);
    // Arrow indicator
    int arrow_x = 40 + zone * ((SCREEN_W - 80) / 4);
    draw_rect_filled(40, 12, SCREEN_W-80, 4, COL_GRAY);
    draw_rect_filled(arrow_x - 6, 8, 12, 12, COL_YELLOW);
}
