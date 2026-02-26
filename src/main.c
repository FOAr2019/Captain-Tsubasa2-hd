// ============================================
// CAPTAIN TSUBASA II - SUPER STRIKE
// HD Remake for PS2
// Fan-made project - Inspired by Konami NES
// ============================================

#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include <time.h>

#include "game/engine.h"
#include "graphics/renderer.h"
#include "input/input.h"

// ============================================
// MAIN ENTRY
// ============================================
int main(void) {
    // ---- Initialize PS2 kernel/SIF ----
    SifInitRpc(0);

    // ---- Load modules ----
    SifLoadStartModule("rom0:SIO2MAN", 0, NULL, NULL);
    SifLoadStartModule("rom0:PADMAN",  0, NULL, NULL);

    // ---- Seed random ----
    srand(GetSystemTime());

    // ---- Init subsystems ----
    renderer_init();
    input_init();
    engine_init(&g_engine);

    printf("[CaptainTsubasa] Boot OK\n");

    // ---- Main game loop ----
    while(1) {
        // 1. Read input
        input_update();

        // 2. Handle input
        engine_handle_input(&g_engine);

        // 3. Update game state
        engine_update(&g_engine);

        // 4. Render
        engine_render(&g_engine);

        // 5. Sync to VSync (60 fps)
        renderer_vsync();
    }

    // Never reached
    return 0;
}
