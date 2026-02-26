#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

#define MAX_PLAYERS     16
#define MAX_TEAM_SIZE   11
#define MAX_TEAMS       32
#define NAME_LEN        24

// ============================================
// SPECIAL SHOT DATA
// ============================================
typedef struct {
    char      name[NAME_LEN];
    ShotType  type;
    u8        power;       // ພະລັງຍິງ
    u8        stamina_cost;// ໃຊ້ stamina ເທົ່າໃດ
    u8        success_rate;// % ສໍາເລັດ base
} SpecialShot;

// ============================================
// PLAYER STATS
// ============================================
typedef struct {
    // Basic Info
    char      name[NAME_LEN];
    char      name_jp[NAME_LEN];
    Position  pos;
    u8        number;      // ເສື້ອເລກ
    u8        country;     // 0=Japan, 1=Germany, etc.

    // Stats (0-99)
    u8        speed;
    u8        stamina;
    u8        max_stamina;
    u8        shoot;
    u8        pass;
    u8        dribble;
    u8        tackle;
    u8        header;
    u8        gk_catch;    // ສໍາລັບ GK
    u8        gk_punch;    // ສໍາລັບ GK

    // Special Shots
    SpecialShot special[3];
    u8          special_count;

    // Match state
    u8        on_field;    // ຢູ່ໃນສະໜາມບໍ?
    u8        has_ball;    // ມີບານບໍ?
    Vec2      field_pos;   // ຕໍາແໜ່ງໃນສະໜາມ

} Player;

// ============================================
// TEAM DATA
// ============================================
typedef struct {
    char    name[NAME_LEN];
    u8      color_primary[3];   // RGB
    u8      color_secondary[3];
    Player  players[MAX_PLAYERS];
    u8      player_count;
    u8      formation[11];      // index ຂອງ player ໃນສະໜາມ

    // Match stats
    u8      score;
    u8      fouls;
} Team;

// ============================================
// FUNCTION DECLARATIONS
// ============================================
void player_init_all(void);
Player* player_get(int team_idx, int player_idx);
Team*   team_get(int idx);
int     team_count(void);
void    player_use_stamina(Player* p, u8 amount);
int     player_can_special(Player* p, int shot_idx);

#endif // PLAYER_H
