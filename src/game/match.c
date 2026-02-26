#include "match.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void set_event(MatchState* ms, const char* msg) {
    strncpy(ms->event_msg, msg, 63);
    ms->event_msg[63] = '\0';
}

void match_init(MatchState* ms, Team* home, Team* away) {
    memset(ms, 0, sizeof(MatchState));
    ms->team_home   = home;
    ms->team_away   = away;
    ms->phase       = MATCH_KICKOFF;
    ms->half        = 1;
    ms->turn        = 0;
    ms->ball_zone   = ZONE_MIDFIELD;
    ms->home_score  = 0;
    ms->away_score  = 0;
    set_event(ms, "Kick Off!");
}

// ============================================
// AI: ຕັດສິນໃຈ action ສໍາລັບ CPU
// ============================================
static void ai_decide(MatchState* ms) {
    // Simple AI: ຂຶ້ນກັບ zone
    BattleState* bs = &ms->battle;
    int action = 0;

    if(ms->ball_zone == ZONE_OPP_GOAL) {
        action = 0; // Shoot
    } else if(ms->ball_zone == ZONE_OPP_DEFENSE) {
        action = rand() % 2; // Shoot or Dribble
    } else {
        action = 1 + rand() % 2; // Pass or Dribble
    }

    // Simulate input
    bs->menu_selection = action;
    int fake_btn = BTN_CROSS;
    battle_input(bs, fake_btn);
}

void match_update(MatchState* ms) {
    switch(ms->phase) {
        case MATCH_KICKOFF:
            ms->anim_timer++;
            if(ms->anim_timer > 90) {
                ms->anim_timer = 0;
                // ເລີ່ມ battle kickoff
                battle_init(&ms->battle, ms->team_home, ms->team_away);
                ms->phase = MATCH_PLAY;
                set_event(ms, "Play!");
            }
            break;

        case MATCH_PLAY:
            // ລໍຖ້າ input ຈາກ player
            break;

        case MATCH_BATTLE:
            battle_update(&ms->battle);
            if(battle_is_done(&ms->battle)) {
                // ຈັດການຜົນ
                BattleEvent* ev = &ms->battle.current_event;

                if(ev->result == RESULT_GOAL) {
                    if(ms->battle.attacker_team == 0)
                        ms->home_score++;
                    else
                        ms->away_score++;

                    set_event(ms, "GOAL!!!");
                    ms->ball_zone = ZONE_MIDFIELD;
                }
                else if(ev->result == RESULT_FAIL) {
                    // possession change + advance zone
                    ms->battle.attacker_team = 1 - ms->battle.attacker_team;
                    if(ms->ball_zone > ZONE_OWN_GOAL)
                        ms->ball_zone--;
                }
                else if(ev->result == RESULT_SUCCESS) {
                    // advance zone
                    if(ms->ball_zone < ZONE_OPP_GOAL)
                        ms->ball_zone++;
                }

                // increment turn
                ms->turn++;
                ms->timer_display = (ms->turn * 90) / MATCH_HALF_TIME;

                // check halftime
                if(ms->turn >= MATCH_HALF_TIME && ms->half == 1) {
                    ms->phase = MATCH_HALFTIME;
                    set_event(ms, "Half Time!");
                    return;
                }
                // check fulltime
                if(ms->turn >= MATCH_HALF_TIME * 2) {
                    ms->phase = MATCH_FULLTIME;
                    set_event(ms, "Full Time!");
                    return;
                }

                // CPU turn?
                if(ms->battle.attacker_team == 1) {
                    battle_init(&ms->battle, ms->team_home, ms->team_away);
                    ms->battle.attacker_team = 1;
                    ai_decide(ms);
                    ms->phase = MATCH_BATTLE;
                } else {
                    // player turn
                    battle_init(&ms->battle, ms->team_home, ms->team_away);
                    ms->phase = MATCH_PLAY;
                    set_event(ms, "Your turn!");
                }
            }
            break;

        case MATCH_HALFTIME:
            ms->anim_timer++;
            if(ms->anim_timer > 180) {
                ms->anim_timer = 0;
                ms->half = 2;
                ms->turn = MATCH_HALF_TIME;
                // Restore some stamina
                for(int t = 0; t < 2; t++) {
                    Team* team = (t == 0) ? ms->team_home : ms->team_away;
                    for(int p = 0; p < team->player_count; p++) {
                        Player* pl = &team->players[p];
                        u8 restore = pl->max_stamina / 4;
                        pl->stamina += restore;
                        if(pl->stamina > pl->max_stamina)
                            pl->stamina = pl->max_stamina;
                    }
                }
                battle_init(&ms->battle, ms->team_home, ms->team_away);
                ms->phase = MATCH_PLAY;
                set_event(ms, "2nd Half! Play!");
            }
            break;

        case MATCH_FULLTIME:
            // ຈົບ match
            break;
    }
}

void match_input(MatchState* ms, int btn) {
    if(ms->phase == MATCH_PLAY) {
        // Player confirms to start battle
        if(btn & BTN_CROSS) {
            battle_init(&ms->battle, ms->team_home, ms->team_away);
            ms->battle.attacker_team = 0;
            ms->phase = MATCH_BATTLE;
        }
    }
    else if(ms->phase == MATCH_BATTLE) {
        battle_input(&ms->battle, btn);
    }
}

int match_is_over(MatchState* ms) {
    return (ms->phase == MATCH_FULLTIME);
}
