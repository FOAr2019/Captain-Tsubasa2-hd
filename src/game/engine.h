#ifndef ENGINE_H
#define ENGINE_H

#include "../types.h"
#include "../game/player.h"
#include "../game/match.h"

typedef struct {
    GameState    state;
    GameState    prev_state;
    MatchState   match;
    int          selected_home;
    int          selected_away;
    int          blink_timer;
    int          team_cursor;
    int          frame_count;
} Engine;

void engine_init(Engine* eng);
void engine_update(Engine* eng);
void engine_render(Engine* eng);
void engine_handle_input(Engine* eng);
void engine_change_state(Engine* eng, GameState new_state);

extern Engine g_engine;

#endif // ENGINE_H
