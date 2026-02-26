#ifndef MATCH_H
#define MATCH_H

#include "../types.h"
#include "player.h"
#include "battle.h"

#define MATCH_HALF_TIME  45   // 45 "turns" per half
#define FIELD_W          320  // field width in pixels
#define FIELD_H          240  // field height in pixels

// Field zones
typedef enum {
    ZONE_OWN_GOAL = 0,
    ZONE_OWN_DEFENSE,
    ZONE_MIDFIELD,
    ZONE_OPP_DEFENSE,
    ZONE_OPP_GOAL
} FieldZone;

// Match phase
typedef enum {
    MATCH_KICKOFF = 0,
    MATCH_PLAY,          // ເລືອກ action ໃນ field
    MATCH_BATTLE,        // turn-based battle
    MATCH_HALFTIME,
    MATCH_FULLTIME
} MatchPhase;

// Match state
typedef struct {
    Team*       team_home;
    Team*       team_away;
    MatchPhase  phase;
    BattleState battle;
    int         turn;          // turns elapsed
    int         half;          // 1 or 2
    FieldZone   ball_zone;     // ບານຢູ່ໃສ
    int         home_score;
    int         away_score;
    int         timer_display; // seconds display
    int         cursor;        // player selection cursor
    int         anim_timer;
    char        event_msg[64];
} MatchState;

void match_init(MatchState* ms, Team* home, Team* away);
void match_update(MatchState* ms);
void match_input(MatchState* ms, int btn);
int  match_is_over(MatchState* ms);

#endif // MATCH_H
