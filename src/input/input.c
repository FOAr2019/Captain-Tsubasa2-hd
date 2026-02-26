#include "input.h"
#include <libpad.h>
#include <string.h>

InputState g_input;

static char s_pad_buf[2][256] __attribute__((aligned(64)));
static struct padButtonStatus s_pad_status;
static u32 s_prev_buttons = 0xFFFF;

void input_init(void) {
    memset(&g_input, 0, sizeof(InputState));
    padInit(0);
    padPortOpen(0, 0, s_pad_buf[0]);
    padPortOpen(1, 0, s_pad_buf[1]);

    // ລໍຖ້າ pad ready
    int state;
    int timeout = 0;
    do {
        state = padGetState(0, 0);
        timeout++;
    } while((state != PAD_STATE_STABLE) &&
            (state != PAD_STATE_FINDCTP1) &&
            timeout < 1000);
}

void input_update(void) {
    u32 current_buttons = 0xFFFF;

    if(padGetState(0, 0) == PAD_STATE_STABLE) {
        padRead(0, 0, &s_pad_status);
        current_buttons = s_pad_status.btns;

        g_input.lx = s_pad_status.ljoy_h - 128;
        g_input.ly = s_pad_status.ljoy_v - 128;
        g_input.rx = s_pad_status.rjoy_h - 128;
        g_input.ry = s_pad_status.rjoy_v - 128;
    }

    // PS2 buttons are active LOW (0 = pressed)
    // Invert so 1 = pressed
    u32 cur_inv  = ~current_buttons & 0xFFFF;
    u32 prev_inv = ~s_prev_buttons  & 0xFFFF;

    g_input.held     = cur_inv;
    g_input.pressed  = cur_inv & ~prev_inv;  // newly pressed
    g_input.released = prev_inv & ~cur_inv;  // newly released

    s_prev_buttons = current_buttons;
}

int input_held(u32 btn) {
    return (g_input.held & btn) != 0;
}

int input_pressed(u32 btn) {
    return (g_input.pressed & btn) != 0;
}

int input_released(u32 btn) {
    return (g_input.released & btn) != 0;
}
