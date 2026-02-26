#include "engine.h"
#include "player.h"
#include "../graphics/renderer.h"
#include "../input/input.h"
#include <string.h>
#include <stdlib.h>

Engine g_engine;

// ============================================
// INIT
// ============================================
void engine_init(Engine* eng) {
    memset(eng, 0, sizeof(Engine));
    eng->state       = STATE_TITLE;
    eng->prev_state  = STATE_TITLE;
    eng->selected_home = 0;
    eng->selected_away = 1;
    eng->blink_timer   = 0;
    eng->team_cursor   = 0;
    eng->frame_count   = 0;

    player_init_all();
}

// ============================================
// CHANGE STATE
// ============================================
void engine_change_state(Engine* eng, GameState new_state) {
    eng->prev_state = eng->state;
    eng->state      = new_state;
    eng->blink_timer = 0;

    if(new_state == STATE_MATCH) {
        Team* home = team_get(eng->selected_home);
        Team* away = team_get(eng->selected_away);
        if(home && away) {
            match_init(&eng->match, home, away);
        }
    }
}

// ============================================
// INPUT HANDLER
// ============================================
void engine_handle_input(Engine* eng) {
    switch(eng->state) {

        case STATE_TITLE:
            if(input_pressed(PAD_START) || input_pressed(PAD_CROSS)) {
                engine_change_state(eng, STATE_TEAM_SELECT);
            }
            break;

        case STATE_TEAM_SELECT:
            if(input_pressed(PAD_UP)) {
                eng->team_cursor--;
                if(eng->team_cursor < 0)
                    eng->team_cursor = team_count() - 1;
            }
            if(input_pressed(PAD_DOWN)) {
                eng->team_cursor++;
                if(eng->team_cursor >= team_count())
                    eng->team_cursor = 0;
            }
            if(input_pressed(PAD_CROSS)) {
                eng->selected_home = eng->team_cursor;
                // CPU picks different team
                eng->selected_away = (eng->team_cursor + 1) % team_count();
                engine_change_state(eng, STATE_MATCH);
            }
            if(input_pressed(PAD_CIRCLE)) {
                engine_change_state(eng, STATE_TITLE);
            }
            break;

        case STATE_MATCH:
            {
                // Build button bitmask for match
                int btn = 0;
                if(input_pressed(PAD_CROSS))    btn |= BTN_CROSS;
                if(input_pressed(PAD_CIRCLE))   btn |= BTN_CIRCLE;
                if(input_pressed(PAD_UP))       btn |= BTN_UP;
                if(input_pressed(PAD_DOWN))     btn |= BTN_DOWN;
                if(input_pressed(PAD_LEFT))     btn |= BTN_LEFT;
                if(input_pressed(PAD_RIGHT))    btn |= BTN_RIGHT;
                if(input_pressed(PAD_TRIANGLE)) btn |= BTN_TRIANGLE;
                if(input_pressed(PAD_SQUARE))   btn |= BTN_SQUARE;

                match_input(&eng->match, btn);

                // Check match over
                if(match_is_over(&eng->match)) {
                    engine_change_state(eng, STATE_GAMEOVER);
                }
            }
            break;

        case STATE_GAMEOVER:
        case STATE_VICTORY:
            if(input_pressed(PAD_CROSS) || input_pressed(PAD_START)) {
                engine_change_state(eng, STATE_TITLE);
            }
            break;

        default:
            break;
    }
}

// ============================================
// UPDATE
// ============================================
void engine_update(Engine* eng) {
    eng->frame_count++;
    eng->blink_timer++;

    switch(eng->state) {
        case STATE_TITLE:
            // Nothing to update
            break;

        case STATE_MATCH:
            match_update(&eng->match);
            // Check for goal → show goal screen
            if(eng->match.phase == MATCH_FULLTIME) {
                engine_change_state(eng, STATE_GAMEOVER);
            }
            break;

        default:
            break;
    }
}

// ============================================
// RENDER
// ============================================
void engine_render(Engine* eng) {
    renderer_begin_frame();

    switch(eng->state) {

        case STATE_TITLE:
            draw_title_screen(eng->blink_timer);
            break;

        case STATE_TEAM_SELECT:
            draw_team_select(team_get(0), team_count(), eng->team_cursor);
            break;

        case STATE_MATCH:
        {
            MatchState* ms = &eng->match;
            Team* home = ms->team_home;
            Team* away = ms->team_away;

            if(ms->phase == MATCH_HALFTIME) {
                draw_halftime_screen(home->name, away->name,
                    ms->home_score, ms->away_score);
                break;
            }
            if(ms->phase == MATCH_FULLTIME) {
                draw_fulltime_screen(home->name, away->name,
                    ms->home_score, ms->away_score);
                break;
            }

            // Draw field
            draw_field();
            draw_field_zone_indicator(ms->ball_zone);

            // Draw HUD
            draw_hud(home->name, away->name,
                     ms->home_score, ms->away_score,
                     ms->timer_display);

            // Draw player stamina bars (home team key players)
            {
                int bx = 10, by = SCREEN_H - 135;
                draw_panel(bx, by, 200, 60, "Stamina");
                Player* tsubasa = &home->players[1];
                draw_text(bx+8, by+24, tsubasa->name, COL_WHITE);
                draw_healthbar(bx+8, by+42, 180,
                    tsubasa->stamina, tsubasa->max_stamina,
                    (tsubasa->stamina > 50) ? COL_HP_GREEN : COL_HP_RED);
            }

            // Draw event message
            if(ms->event_msg[0]) {
                int ex = SCREEN_W/2 - 100;
                draw_rect_filled(ex, 50, 200, 24, COL_UI_BG);
                draw_text_center(54, ms->event_msg, COL_YELLOW);
            }

            // Battle UI
            if(ms->phase == MATCH_BATTLE) {
                BattleState* bs = &ms->battle;

                if(bs->phase == BPHASE_SELECT_ACTION) {
                    Player* ball_holder = bs->ball_holder;
                    int can_sp = (ball_holder->special_count > 0);
                    draw_battle_menu(bs->menu_selection, can_sp);

                    // Stamina panel for ball holder
                    draw_panel(10, 160, 200, 50, ball_holder->name);
                    draw_healthbar(18, 192, 180,
                        ball_holder->stamina, ball_holder->max_stamina,
                        (ball_holder->stamina > 40)
                            ? COL_HP_GREEN : COL_HP_RED);
                }
                else if(bs->phase == BPHASE_SELECT_SPECIAL) {
                    draw_special_menu(bs->ball_holder, bs->menu_selection);
                }
                else if(bs->phase == BPHASE_ANIMATE_ACTION ||
                        bs->phase == BPHASE_SHOW_RESULT) {
                    draw_battle_result(bs->message, bs->current_event.result);
                }
            }
            else if(ms->phase == MATCH_PLAY) {
                // Show "Press X to play"
                if(eng->blink_timer / 30 % 2 == 0) {
                    draw_text_center(SCREEN_H/2 - 10,
                        "Press X to take action", COL_WHITE);
                }
            }
        }
        break;

        case STATE_GAMEOVER:
        {
            MatchState* ms = &eng->match;
            draw_fulltime_screen(
                ms->team_home->name, ms->team_away->name,
                ms->home_score, ms->away_score);
        }
        break;

        default:
            draw_rect_filled(0, 0, SCREEN_W, SCREEN_H, 0x00000080);
            break;
    }

    renderer_end_frame();
}
