#ifndef MISSIONS_H
#define MISSIONS_H

#include <stdint.h>
#include "lang.h"

// -------------------------------------------------------
// Textes propres aux missions (dialogues + résumés d'objectifs).
// mission_strings_fr[] et mission_strings_en[] sont dans missions.c (bank 4).
//
// Pour ajouter une mission :
//   1. Ajouter les entrées dans cet enum (en respectant l'ordre)
//   2. Ajouter les chaînes dans mission_strings_fr[] et mission_strings_en[]
//      dans missions.c (même ordre)
// -------------------------------------------------------
typedef enum {
    // --- Mission 1 ---
    MTXT_M1_1 = 0,
    MTXT_M1_2,
    MGOAL_M1_1,
    MGOAL_M1_2,
    // --- Mission 2 ---
    MTXT_M2_1,
    MTXT_M2_2,
    MGOAL_M2_1,
    MGOAL_M2_2,
    // --- Mission 3 ---
    MTXT_M3_1,
    MTXT_M3_2,
    MGOAL_M3_1,
    MGOAL_M3_2,
    // --- Mission 4 ---
    MTXT_M4_1,
    MTXT_M4_2,
    MGOAL_M4_1,
    MGOAL_M4_2,
} MissionTextIdx;

// Tableaux dans missions.c (bank 4)
extern char *mission_strings_fr[];
extern char *mission_strings_en[];

// Wrapper ROM0 : copie le texte idx depuis bank 4 dans un buffer WRAM, restaure la bank courante.
// Safe depuis n'importe quelle bank.
char* nb_get_mission_text(uint8_t idx);
#define GET_MISSION_TEXT(idx) nb_get_mission_text(idx)

// --- IDs de portrait ---
#define PORTRAIT_PENULTIMO  0u

// --- Structure d'une sous-mission ---
// Tous les champs "target_*" à 0 signifient "ignoré".
typedef struct {
    uint16_t dialogue_idx;
    uint8_t  portrait_id;
    uint16_t goal_idx;

    int32_t  target_money;
    uint16_t target_pop;
    uint16_t target_food_stock;
    uint16_t target_food_prod;
    uint8_t  target_happiness;
    uint16_t target_ore;
    uint16_t target_culture;
    uint8_t  target_type;
    uint8_t  target_count;
    uint8_t  target_type2;
    uint8_t  target_count2;
} MissionStep;

// Tableaux de missions dans missions.c (bank 5)
extern MissionStep mission1[];
extern MissionStep mission2[];
extern MissionStep mission3[];
extern MissionStep mission4[];

// Copie l'étape step_idx de la mission courante (game.mission_id) dans *out.
// Définie dans missions.c (bank 5).
void copy_mission_step(uint8_t step_idx, MissionStep *out);

// Wrapper ROM0 : lit l'étape en bank 5, restaure bank 2.
// Appelée depuis story_mode.c (bank 2) uniquement.
void nb_get_mission_step_b4(uint8_t step_idx, MissionStep *out);

#endif
