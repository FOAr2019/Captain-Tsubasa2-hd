#ifndef INPUT_H
#define INPUT_H

#include "../types.h"

// PS2 Pad button masks
#define PAD_SELECT    0x0001
#define PAD_L3        0x0002
#define PAD_R3        0x0004
#define PAD_START     0x0008
#define PAD_UP        0x0010
#define PAD_RIGHT     0x0020
#define PAD_DOWN      0x0040
#define PAD_LEFT      0x0080
#define PAD_L2        0x0100
#define PAD_R2        0x0200
#define PAD_L1        0x0400
#define PAD_R1        0x0800
#define PAD_TRIANGLE  0x1000
#define PAD_CIRCLE    0x2000
#define PAD_CROSS     0x4000
#define PAD_SQUARE    0x8000

typedef struct {
    u32 held;       // ກໍາລັງກົດ
    u32 pressed;    // ກົດໃໝ່ (edge detect)
    u32 released;   // ປ່ອຍ (edge detect)
    s8  lx, ly;     // Left analog
    s8  rx, ry;     // Right analog
} InputState;

void   input_init(void);
void   input_update(void);
int    input_held(u32 btn);
int    input_pressed(u32 btn);
int    input_released(u32 btn);

extern InputState g_input;

#endif // INPUT_H
