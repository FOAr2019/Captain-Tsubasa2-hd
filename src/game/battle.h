#ifndef BATTLE_H
#define BATTLE_H

#include "../types.h"
#include "player.h"

// ============================================
// BATTLE PHASES
// ============================================
typedef enum {
    BPHASE_SELECT_ACTION = 0,   // ເລືອກ action
    BPHASE_SELECT_TARGET,       // ເລືອກ target (pass)
    BPHASE_SELECT_SPECIAL,      // ເລືອກ special shot
    BPHASE_ANIMATE_ACTION,      // ສະແດງ animation
    BPHASE_SHOW_RESULT,         // ສະແດງຜົນ
    BPHASE_DONE                 // ຈົບ
} BattlePhase;

// ============================================
// BATTLE RESULT
// ============================================
typedef enum {
    RESULT_NONE = 0,
    RESULT_SUCCESS,
    RESULT_FAIL,
    RESULT_GOAL,
    RESULT_SAVED,
    RESULT_DEFLECTED
} BattleResult;

// ============================================
// BATTLE EVENT
// ============================================
typedef struct {
    ActionType   action;
    Player*      attacker;
    Player*      defender;       // GK or DF
    ShotType     shot_type;      // ຖ້າ special shot
    int          atk_roll;       // dice roll ຂອງ attacker
    int          def_roll;       // dice roll ຂອງ defender
    int          atk_total;
    int          def_total;
    BattleResult result;
    int          stamina_used;
} BattleEvent;

// ============================================
// BATTLE STATE
// ============================================
typedef struct {
    BattlePhase  phase;
    BattleEvent  current_event;
    Team*        team_home;
    Team*        team_away;
    int          attacker_team;  // 0=home, 1=away
    Player*      ball_holder;
    int          menu_selection; // cursor ໃນ menu
    int          anim_timer;     // animation timer
    int          result_timer;   // ສະແດງຜົນ timer
    char         message[64];    // ຂໍ້ຄວາມສະແດງ
} BattleState;

// ============================================
// FUNCTIONS
// ============================================
void battle_init(BattleState* bs, Team* home, Team* away);
void battle_update(BattleState* bs);
void battle_input(BattleState* bs, int btn);
void battle_resolve(BattleEvent* ev);
int  battle_calc_success(int atk, int def, int base_rate);
int  battle_is_done(BattleState* bs);

// Button defines (PS2 controller)
#define BTN_UP      0x1000
#define BTN_DOWN    0x4000
#define BTN_LEFT    0x8000
#define BTN_RIGHT   0x2000
#define BTN_CROSS   0x4000  // X
#define BTN_CIRCLE  0x2000  // O
#define BTN_SQUARE  0x8000
#define BTN_TRIANGLE 0x1000
#define BTN_START   0x0800
#define BTN_SELECT  0x0100

#endif // BATTLE_H
