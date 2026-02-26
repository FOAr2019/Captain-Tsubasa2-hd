#include "battle.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================
// INTERNAL HELPERS
// ============================================

// ຄໍານວນ dice roll (1-20 ຄື D&D style)
static int roll_dice(int stat) {
    // stat range 0-99, roll adds randomness
    int base = stat / 5;           // 0-19
    int rnd  = rand() % 20;       // 0-19
    return base + rnd;
}

// ຂຽນ message
static void set_msg(BattleState* bs, const char* msg) {
    strncpy(bs->message, msg, 63);
    bs->message[63] = '\0';
}

// ============================================
// INIT
// ============================================
void battle_init(BattleState* bs, Team* home, Team* away) {
    memset(bs, 0, sizeof(BattleState));
    bs->team_home       = home;
    bs->team_away       = away;
    bs->attacker_team   = 0; // home starts
    bs->ball_holder     = &home->players[1]; // Tsubasa ເລີ່ມ
    bs->ball_holder->has_ball = 1;
    bs->phase           = BPHASE_SELECT_ACTION;
    bs->menu_selection  = 0;
    set_msg(bs, "Select Action");
}

// ============================================
// INPUT HANDLER
// ============================================
void battle_input(BattleState* bs, int btn) {
    if(bs->phase == BPHASE_SELECT_ACTION) {
        // Menu: 0=Shoot 1=Pass 2=Dribble 3=Special
        if(btn & BTN_UP)   bs->menu_selection--;
        if(btn & BTN_DOWN) bs->menu_selection++;
        if(bs->menu_selection < 0) bs->menu_selection = 3;
        if(bs->menu_selection > 3) bs->menu_selection = 0;

        if(btn & BTN_CROSS) { // X = Confirm
            BattleEvent* ev = &bs->current_event;
            memset(ev, 0, sizeof(BattleEvent));
            ev->attacker = bs->ball_holder;

            switch(bs->menu_selection) {
                case 0: // Shoot
                    ev->action = ACTION_SHOOT;
                    ev->shot_type = SHOT_NORMAL;
                    // ຜູ້ຮັກສາ = GK ຂອງທີມຕ່ໍ
                    ev->defender = (bs->attacker_team == 0)
                        ? &bs->team_away->players[0]
                        : &bs->team_home->players[0];
                    bs->phase = BPHASE_ANIMATE_ACTION;
                    set_msg(bs, "Shooting!");
                    break;

                case 1: // Pass
                    ev->action = ACTION_PASS;
                    bs->phase  = BPHASE_SELECT_TARGET;
                    set_msg(bs, "Select target player");
                    break;

                case 2: // Dribble
                    ev->action = ACTION_DRIBBLE;
                    // ສຸ່ມ DF ຂອງທີມຕ່ໍ
                    {
                        Team* opp = (bs->attacker_team == 0)
                            ? bs->team_away : bs->team_home;
                        int di = 1 + (rand() % 4);
                        ev->defender = &opp->players[di];
                    }
                    bs->phase = BPHASE_ANIMATE_ACTION;
                    set_msg(bs, "Dribbling!");
                    break;

                case 3: // Special Shot
                    if(ev->attacker->special_count > 0) {
                        bs->phase = BPHASE_SELECT_SPECIAL;
                        set_msg(bs, "Select Special Shot");
                    }
                    break;
            }
        }
    }
    else if(bs->phase == BPHASE_SELECT_SPECIAL) {
        Player* atk = bs->ball_holder;
        if(btn & BTN_UP)   bs->menu_selection--;
        if(btn & BTN_DOWN) bs->menu_selection++;
        if(bs->menu_selection < 0) bs->menu_selection = 0;
        if(bs->menu_selection >= atk->special_count)
            bs->menu_selection = atk->special_count - 1;

        if(btn & BTN_CROSS) {
            int si = bs->menu_selection;
            if(player_can_special(atk, si)) {
                BattleEvent* ev = &bs->current_event;
                ev->action    = ACTION_SPECIAL_SHOT;
                ev->shot_type = atk->special[si].type;
                ev->attacker  = atk;
                ev->defender  = (bs->attacker_team == 0)
                    ? &bs->team_away->players[0]
                    : &bs->team_home->players[0];
                bs->phase = BPHASE_ANIMATE_ACTION;
                set_msg(bs, atk->special[si].name);
            } else {
                set_msg(bs, "Not enough stamina!");
            }
        }
        if(btn & BTN_CIRCLE) { // O = Back
            bs->phase = BPHASE_SELECT_ACTION;
            set_msg(bs, "Select Action");
        }
    }
    else if(bs->phase == BPHASE_SHOW_RESULT) {
        if(btn & BTN_CROSS || btn & BTN_CIRCLE) {
            bs->phase = BPHASE_DONE;
        }
    }
}

// ============================================
// RESOLVE BATTLE
// ============================================
void battle_resolve(BattleEvent* ev) {
    int atk_stat = 0, def_stat = 0;
    int base_rate = 70;

    switch(ev->action) {
        case ACTION_SHOOT:
            atk_stat  = ev->attacker->shoot;
            def_stat  = ev->defender->gk_catch;
            base_rate = 65;
            break;

        case ACTION_SPECIAL_SHOT:
            // Special shot ໃຊ້ special stats
            {
                int si = 0;
                for(int i = 0; i < ev->attacker->special_count; i++) {
                    if(ev->attacker->special[i].type == ev->shot_type) {
                        si = i; break;
                    }
                }
                atk_stat  = ev->attacker->special[si].power;
                def_stat  = ev->defender->gk_catch;
                base_rate = ev->attacker->special[si].success_rate;
                // ໃຊ້ stamina
                player_use_stamina(ev->attacker,
                    ev->attacker->special[si].stamina_cost);
                ev->stamina_used = ev->attacker->special[si].stamina_cost;
            }
            break;

        case ACTION_DRIBBLE:
            atk_stat  = ev->attacker->dribble;
            def_stat  = ev->defender->tackle;
            base_rate = 70;
            break;

        case ACTION_TACKLE:
            atk_stat  = ev->attacker->tackle;
            def_stat  = ev->defender->dribble;
            base_rate = 65;
            break;

        case ACTION_PASS:
            atk_stat  = ev->attacker->pass;
            def_stat  = 30; // base intercept chance
            base_rate = 80;
            break;

        default:
            ev->result = RESULT_NONE;
            return;
    }

    ev->atk_roll  = roll_dice(atk_stat);
    ev->def_roll  = roll_dice(def_stat);
    ev->atk_total = atk_stat + ev->atk_roll;
    ev->def_total = def_stat + ev->def_roll;

    // Stamina penalty
    int stam_pct = (ev->attacker->stamina * 100) / ev->attacker->max_stamina;
    if(stam_pct < 30) ev->atk_total -= 10;
    else if(stam_pct < 60) ev->atk_total -= 5;

    // ຕັດສິນ
    if(ev->action == ACTION_SHOOT || ev->action == ACTION_SPECIAL_SHOT) {
        if(ev->atk_total > ev->def_total) {
            ev->result = RESULT_GOAL;
        } else if(ev->atk_total == ev->def_total) {
            ev->result = RESULT_DEFLECTED;
        } else {
            ev->result = RESULT_SAVED;
        }
    } else {
        ev->result = (ev->atk_total >= ev->def_total)
            ? RESULT_SUCCESS : RESULT_FAIL;
    }
}

// ============================================
// UPDATE
// ============================================
void battle_update(BattleState* bs) {
    if(bs->phase == BPHASE_ANIMATE_ACTION) {
        bs->anim_timer++;
        if(bs->anim_timer >= 60) { // 1 second @ 60fps
            bs->anim_timer = 0;
            battle_resolve(&bs->current_event);

            // ຕັ້ງ message ຕາມຜົນ
            BattleEvent* ev = &bs->current_event;
            switch(ev->result) {
                case RESULT_GOAL:
                    set_msg(bs, "GOAL!!!");
                    // ເພີ່ມຄະແນນ
                    if(bs->attacker_team == 0)
                        bs->team_home->score++;
                    else
                        bs->team_away->score++;
                    break;
                case RESULT_SAVED:
                    set_msg(bs, "Saved by the goalkeeper!");
                    break;
                case RESULT_DEFLECTED:
                    set_msg(bs, "Deflected! Corner kick!");
                    break;
                case RESULT_SUCCESS:
                    if(ev->action == ACTION_DRIBBLE)
                        set_msg(bs, "Dribble successful!");
                    else if(ev->action == ACTION_PASS)
                        set_msg(bs, "Pass connected!");
                    else
                        set_msg(bs, "Success!");
                    break;
                case RESULT_FAIL:
                    set_msg(bs, "Failed! Ball lost!");
                    // ສ່ຽງ possession
                    bs->attacker_team = 1 - bs->attacker_team;
                    break;
                default:
                    break;
            }

            bs->phase = BPHASE_SHOW_RESULT;
            bs->result_timer = 0;
        }
    }
    else if(bs->phase == BPHASE_SHOW_RESULT) {
        bs->result_timer++;
        // Auto-advance ຫຼັງ 3 seconds
        if(bs->result_timer >= 180) {
            bs->phase = BPHASE_DONE;
        }
    }
}

int battle_is_done(BattleState* bs) {
    return (bs->phase == BPHASE_DONE);
}
