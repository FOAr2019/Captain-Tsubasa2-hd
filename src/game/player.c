#include "player.h"
#include <string.h>
#include <stdlib.h>

// ============================================
// ALL TEAMS DATA
// ============================================
static Team all_teams[MAX_TEAMS];
static int  total_teams = 0;

// ============================================
// HELPER: ສ້າງ Special Shot
// ============================================
static SpecialShot make_shot(const char* name, ShotType type,
                              u8 power, u8 cost, u8 rate) {
    SpecialShot s;
    strncpy(s.name, name, NAME_LEN);
    s.type         = type;
    s.power        = power;
    s.stamina_cost = cost;
    s.success_rate = rate;
    return s;
}

// ============================================
// HELPER: ສ້າງ Player
// ============================================
static Player make_player(const char* name, Position pos,
    u8 num, u8 spd, u8 stm,
    u8 sht, u8 pas, u8 dri,
    u8 tck, u8 hdr) {
    Player p;
    memset(&p, 0, sizeof(Player));
    strncpy(p.name, name, NAME_LEN);
    p.pos           = pos;
    p.number        = num;
    p.speed         = spd;
    p.stamina       = stm;
    p.max_stamina   = stm;
    p.shoot         = sht;
    p.pass          = pas;
    p.dribble       = dri;
    p.tackle        = tck;
    p.header        = hdr;
    p.special_count = 0;
    p.on_field      = 0;
    p.has_ball      = 0;
    return p;
}

// ============================================
// INIT: JAPAN TEAM
// ============================================
static void init_team_japan(Team* t) {
    strncpy(t->name, "Japan", NAME_LEN);
    t->color_primary[0]   = 0;
    t->color_primary[1]   = 50;
    t->color_primary[2]   = 150;
    t->color_secondary[0] = 255;
    t->color_secondary[1] = 255;
    t->color_secondary[2] = 255;
    t->score  = 0;
    t->fouls  = 0;
    t->player_count = 0;

    // --- #1 Wakabayashi (GK) ---
    Player wakabayashi = make_player(
        "Wakabayashi", POS_GK, 1,
        70, 95, 60, 65, 65, 70, 70
    );
    wakabayashi.gk_catch = 95;
    wakabayashi.gk_punch = 80;
    wakabayashi.special[0] = make_shot("God Hand Catch", SHOT_NORMAL, 90, 15, 85);
    wakabayashi.special[1] = make_shot("Eagle Jump", SHOT_NORMAL, 75, 10, 90);
    wakabayashi.special_count = 2;
    t->players[t->player_count++] = wakabayashi;

    // --- #10 Tsubasa (FW) ---
    Player tsubasa = make_player(
        "Tsubasa", POS_FW, 10,
        88, 100, 92, 90, 92, 65, 80
    );
    tsubasa.special[0] = make_shot("Drive Shot", SHOT_DRIVE, 88, 20, 80);
    tsubasa.special[1] = make_shot("Sky Drive Shot", SHOT_SKY, 95, 35, 70);
    tsubasa.special[2] = make_shot("Twin Shot", SHOT_TWIN_SHOT, 92, 25, 75);
    tsubasa.special_count = 3;
    t->players[t->player_count++] = tsubasa;

    // --- #11 Hyuga (FW) ---
    Player hyuga = make_player(
        "Hyuga", POS_FW, 11,
        82, 90, 97, 70, 80, 70, 90
    );
    hyuga.special[0] = make_shot("Tiger Shot", SHOT_TIGER, 95, 25, 75);
    hyuga.special[1] = make_shot("Neo Tiger Shot", SHOT_TIGER, 98, 35, 65);
    hyuga.special[2] = make_shot("Atomic Shot", SHOT_ATOMIC, 99, 40, 60);
    hyuga.special_count = 3;
    t->players[t->player_count++] = hyuga;

    // --- #9 Misaki (MF) ---
    Player misaki = make_player(
        "Misaki", POS_MF, 9,
        90, 92, 87, 97, 90, 68, 82
    );
    misaki.special[0] = make_shot("Eagle Shot", SHOT_EAGLE, 90, 22, 78);
    misaki.special[1] = make_shot("Twin Shot", SHOT_TWIN_SHOT, 92, 25, 75);
    misaki.special_count = 2;
    t->players[t->player_count++] = misaki;

    // --- #8 Ishizaki (MF) ---
    Player ishizaki = make_player(
        "Ishizaki", POS_MF, 8,
        85, 88, 75, 80, 82, 75, 75
    );
    ishizaki.special[0] = make_shot("Overhead Kick", SHOT_OVERHEAD, 78, 18, 72);
    ishizaki.special_count = 1;
    t->players[t->player_count++] = ishizaki;

    // --- #5 Matsuyama (DF) ---
    Player matsuyama = make_player(
        "Matsuyama", POS_DF, 5,
        78, 85, 68, 75, 75, 85, 72
    );
    matsuyama.special[0] = make_shot("Sliding Tackle", SHOT_NORMAL, 72, 12, 80);
    matsuyama.special_count = 1;
    t->players[t->player_count++] = matsuyama;

    // --- #4 Morisaki (DF) ---
    Player morisaki = make_player(
        "Morisaki", POS_DF, 4,
        75, 82, 60, 70, 72, 82, 70
    );
    morisaki.special_count = 0;
    t->players[t->player_count++] = morisaki;

    // --- #6 Sano (DF) ---
    Player sano = make_player(
        "Sano", POS_DF, 6,
        76, 83, 62, 72, 74, 83, 71
    );
    sano.special_count = 0;
    t->players[t->player_count++] = sano;

    // --- #7 Urabe (MF) ---
    Player urabe = make_player(
        "Urabe", POS_MF, 7,
        80, 85, 72, 82, 80, 72, 74
    );
    urabe.special[0] = make_shot("Falcon Shot", SHOT_FALCON, 75, 15, 75);
    urabe.special_count = 1;
    t->players[t->player_count++] = urabe;

    // --- #3 Jito (DF) ---
    Player jito = make_player(
        "Jito", POS_DF, 3,
        72, 80, 58, 68, 70, 80, 68
    );
    jito.special_count = 0;
    t->players[t->player_count++] = jito;

    // --- #2 Wakashimazu (GK/Sub) ---
    Player wakashimazu = make_player(
        "Wakashimazu", POS_GK, 2,
        68, 85, 55, 60, 60, 65, 65
    );
    wakashimazu.gk_catch = 80;
    wakashimazu.gk_punch = 70;
    wakashimazu.special[0] = make_shot("Iron Wall Catch", SHOT_NORMAL, 80, 12, 82);
    wakashimazu.special_count = 1;
    t->players[t->player_count++] = wakashimazu;

    // Formation: 4-3-3
    // GK=0, DF=3,6,7,9, MF=4,8,10, FW=1,2,3
    int formation[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for(int i = 0; i < 11; i++) {
        t->formation[i] = formation[i];
        t->players[formation[i]].on_field = 1;
    }
}

// ============================================
// INIT: GERMANY TEAM
// ============================================
static void init_team_germany(Team* t) {
    strncpy(t->name, "Germany", NAME_LEN);
    t->color_primary[0]   = 220;
    t->color_primary[1]   = 220;
    t->color_primary[2]   = 220;
    t->color_secondary[0] = 0;
    t->color_secondary[1] = 0;
    t->color_secondary[2] = 0;
    t->score  = 0;
    t->fouls  = 0;
    t->player_count = 0;

    // --- Muller (GK) ---
    Player muller = make_player(
        "Muller", POS_GK, 1,
        72, 90, 58, 62, 62, 68, 68
    );
    muller.gk_catch = 88;
    muller.gk_punch = 75;
    muller.special[0] = make_shot("Strong Catch", SHOT_NORMAL, 85, 13, 83);
    muller.special_count = 1;
    t->players[t->player_count++] = muller;

    // --- Schneider (FW) ---
    Player schneider = make_player(
        "Schneider", POS_FW, 9,
        85, 92, 98, 75, 85, 72, 85
    );
    schneider.special[0] = make_shot("Fire Shot", SHOT_FIRE, 96, 28, 72);
    schneider.special[1] = make_shot("Neo Fire Shot", SHOT_FIRE, 99, 38, 62);
    schneider.special_count = 2;
    t->players[t->player_count++] = schneider;

    // --- Kaltz (DF) ---
    Player kaltz = make_player(
        "Kaltz", POS_DF, 2,
        80, 88, 70, 78, 78, 88, 75
    );
    kaltz.special[0] = make_shot("Sliding Tackle", SHOT_NORMAL, 75, 14, 82);
    kaltz.special_count = 1;
    t->players[t->player_count++] = kaltz;

    // --- Levin (MF) ---
    Player levin = make_player(
        "Levin", POS_MF, 7,
        83, 88, 82, 85, 84, 72, 78
    );
    levin.special[0] = make_shot("Overhead Kick", SHOT_OVERHEAD, 82, 18, 75);
    levin.special_count = 1;
    t->players[t->player_count++] = levin;

    // Add more DF/MF players
    Player p;
    const char* names[] = {"Kruger","Fischer","Becker","Wagner","Hoffmann","Klose","Rahn"};
    for(int i = 0; i < 7; i++) {
        p = make_player(names[i],
            (i < 3) ? POS_DF : POS_MF,
            i+3, 76+i, 82+i, 65+i, 70+i, 70+i, 78+i, 70+i
        );
        p.special_count = 0;
        t->players[t->player_count++] = p;
    }

    int formation[] = {0,1,2,3,4,5,6,7,8,9,10};
    for(int i = 0; i < 11; i++) {
        t->formation[i] = formation[i];
        t->players[formation[i]].on_field = 1;
    }
}

// ============================================
// INIT: ARGENTINA (DIAZ) TEAM
// ============================================
static void init_team_argentina(Team* t) {
    strncpy(t->name, "Argentina", NAME_LEN);
    t->color_primary[0]   = 115;
    t->color_primary[1]   = 188;
    t->color_primary[2]   = 220;
    t->color_secondary[0] = 255;
    t->color_secondary[1] = 255;
    t->color_secondary[2] = 255;
    t->score  = 0;
    t->fouls  = 0;
    t->player_count = 0;

    // --- Hernandez (GK) ---
    Player hernandez = make_player(
        "Hernandez", POS_GK, 1,
        70, 88, 56, 60, 60, 66, 66
    );
    hernandez.gk_catch = 85;
    hernandez.gk_punch = 72;
    hernandez.special_count = 0;
    t->players[t->player_count++] = hernandez;

    // --- Diaz (FW) ---
    Player diaz = make_player(
        "Diaz", POS_FW, 10,
        88, 93, 94, 82, 90, 68, 92
    );
    diaz.special[0] = make_shot("Overhead Kick", SHOT_OVERHEAD, 93, 24, 77);
    diaz.special[1] = make_shot("Super Overhead", SHOT_OVERHEAD, 97, 34, 67);
    diaz.special_count = 2;
    t->players[t->player_count++] = diaz;

    // Fill rest
    Player p;
    const char* names[] = {"Rodriguez","Gomez","Martinez","Lopez",
                           "Garcia","Perez","Sanchez","Torres","Ramirez"};
    for(int i = 0; i < 9; i++) {
        p = make_player(names[i],
            (i < 3) ? POS_DF : (i < 6) ? POS_MF : POS_FW,
            i+2, 75+i, 82+i, 68+i, 72+i, 72+i, 76+i, 71+i
        );
        p.special_count = 0;
        t->players[t->player_count++] = p;
    }

    int formation[] = {0,1,2,3,4,5,6,7,8,9,10};
    for(int i = 0; i < 11; i++) {
        t->formation[i] = formation[i];
        t->players[formation[i]].on_field = 1;
    }
}

// ============================================
// PUBLIC API
// ============================================
void player_init_all(void) {
    total_teams = 0;
    init_team_japan    (&all_teams[total_teams++]);
    init_team_germany  (&all_teams[total_teams++]);
    init_team_argentina(&all_teams[total_teams++]);
}

Player* player_get(int team_idx, int player_idx) {
    if(team_idx < 0 || team_idx >= total_teams) return NULL;
    if(player_idx < 0 || player_idx >= all_teams[team_idx].player_count) return NULL;
    return &all_teams[team_idx].players[player_idx];
}

Team* team_get(int idx) {
    if(idx < 0 || idx >= total_teams) return NULL;
    return &all_teams[idx];
}

int team_count(void) {
    return total_teams;
}

void player_use_stamina(Player* p, u8 amount) {
    if(p->stamina >= amount)
        p->stamina -= amount;
    else
        p->stamina = 0;
}

int player_can_special(Player* p, int shot_idx) {
    if(shot_idx < 0 || shot_idx >= p->special_count) return 0;
    return (p->stamina >= p->special[shot_idx].stamina_cost);
}
