#pragma bank 5
#include <gb/gb.h>
#include "missions.h"
#include "game_state.h"
#include "constants.h"

// ============================================================
// TEXTES FR
// Encodage : ~ = é/è/ê,  | = û/ù
// ============================================================
char *mission_strings_fr[] = {
    // MTXT_M1_1
    "Presidente, notre ancienne\nile a d| prendre sa retraite\nsuite a un l~ger d~saccord\ng~ologique.\nMais r~jouissez-vous ! Le\ng~n~reux Ronald Dump nous a\noffert ce lopin de terre.\nIl dit que la personne ~\nlaquelle elle appartenait\nn'en a plus besoin.\nVos objectifs :\n- 3 fermes\n- 6000 pesos\n- 25 habitants",
    // MTXT_M1_2
    "Magnifique, Presidente !\nVous avez prouv~ que la\ncivilisation est possible...\nm~me ici.\nViva el Presidente !",
    // MGOAL_M1_1
    "3 fermes + 6000$ + 25 hab",
    // MGOAL_M1_2
    "Mission 1 termin~e !",

    // MTXT_M2_1
    "Presidente ! Votre ~le\nmange, vos caisses sonnent.\nMais le peuple s'ennuie.\nUn peuple qui s'ennuie\npense. Et un peuple qui\npense, c'est dangereux.\nLa plantation produit de\nla culture. L'~glise\ndonne le bonheur.\nVos objectifs :\n- 1 plantation\n- 1 ~glise\n- 100 culture\n- Bonheur 60\n(Un bar aide pour le bonheur !)",
    // MTXT_M2_2
    "Presidente ! Votre peuple\nchante, prie, et boit.\nQue demander de plus ?\nViva el Presidente !",
    // MGOAL_M2_1
    "Culture + bonheur + b~timents",
    // MGOAL_M2_2
    "Mission 2 termin~e !",

    // MTXT_M3_1
    "Presidente ! Votre peuple\nest heureux, mais peu\n~duqu~. Et peu surveill~.\nUn peuple ~duqu~ ob~it\nplus intelligemment.\nUn peuple surveill~ ob~it\ntout court.\nUn h~pital r~duit aussi\nles d~c~s inutiles.\nVos objectifs :\n- 1 ~cole\n- 1 h~pital\n- 1 commissariat\n- 50 habitants",
    // MTXT_M3_2
    "Magnifique Presidente !\n~ducation, sant~, ordre...\nvotre utopie prend forme.\nViva el Presidente !",
    // MGOAL_M3_1
    "B~timents + 50 habitants",
    // MGOAL_M3_2
    "Mission 3 termin~e !",

    // MTXT_M4_1
    "Presidente ! Votre ~le\nprosp~re, mais elle manque\nd'~nergie. Sans ~lectricit~,\nla mine ne peut pas\nfonctionner. M~me la\ndiscoth~que reste silencieuse\n— et un peuple sans musique\nr~fl~chit trop.\nVos objectifs :\n- 1 centrale ~lectrique\n- 1 mine (sur un gisement !)\n- 200 minerai",
    // MTXT_M4_2
    "Presidente ! Votre ~le\nextrait, produit, ~claire !\nLe minerai se vend, les\ncaisses grossissent.\nViva el Presidente !",
    // MGOAL_M4_1
    "Centrale + mine + 200 minerai",
    // MGOAL_M4_2
    "Mission 4 termin~e !",
};

// ============================================================
// TEXTES EN
// ============================================================
char *mission_strings_en[] = {
    // MTXT_M1_1
    "Presidente, our old island\nhas retired due to a minor\ngeological disagreement.\nBut rejoice! The generous\nRonald Dump has gifted us\nthis patch of dirt.\nHe says the previous owner\nno longer needs it.\nYour objectives:\n- 3 farms\n- 6000 pesos\n- 25 inhabitants",
    // MTXT_M1_2
    "Magnificent, Presidente!\nYou have proven that\ncivilization is possible...\neven here.\nViva el Presidente!",
    // MGOAL_M1_1
    "3 farms + 6000$ + 25 inhab",
    // MGOAL_M1_2
    "Mission 1 complete!",

    // MTXT_M2_1
    "Presidente! Your island\neats, your coffers ring.\nBut the people are bored.\nA bored people thinks.\nAnd a thinking people\nis dangerous.\nThe plantation makes culture.\nThe church brings happiness.\nYour objectives:\n- 1 plantation\n- 1 church\n- 100 culture\n- Happiness 60\n(A bar helps with happiness!)",
    // MTXT_M2_2
    "Presidente! Your people\nsing, pray, and drink.\nWhat more could one ask?\nViva el Presidente!",
    // MGOAL_M2_1
    "Culture + happiness + buildings",
    // MGOAL_M2_2
    "Mission 2 complete!",

    // MTXT_M3_1
    "Presidente! Your people\nare happy but poorly\neducated. And unsupervised.\nAn educated people obeys\nmore intelligently.\nA watched people obeys\nentirely.\nA hospital also reduces\nunnecessary deaths.\nYour objectives:\n- 1 school\n- 1 hospital\n- 1 police station\n- 50 inhabitants",
    // MTXT_M3_2
    "Magnificent Presidente!\nEducation, health, order...\nyour utopia takes shape.\nViva el Presidente!",
    // MGOAL_M3_1
    "Buildings + 50 inhabitants",
    // MGOAL_M3_2
    "Mission 3 complete!",

    // MTXT_M4_1
    "Presidente! Your island\nprospers, but lacks energy.\nWithout electricity, the\nmine cannot function.\nEven the disco stays\nsilent — and a people\nwithout music thinks too much.\nYour objectives:\n- 1 power plant\n- 1 mine (on a deposit!)\n- 200 ore",
    // MTXT_M4_2
    "Presidente! Your island\nextracts, produces, shines!\nThe ore sells, the coffers\ngrow.\nViva el Presidente!",
    // MGOAL_M4_1
    "Power plant + mine + 200 ore",
    // MGOAL_M4_2
    "Mission 4 complete!",
};

// ============================================================
// TABLEAUX DE MISSIONS
// Champs : dialogue_idx, portrait_id, goal_idx,
//          target_money, target_pop, target_food_stock,
//          target_food_prod, target_happiness, target_ore,
//          target_culture, target_type, target_count,
//          target_type2, target_count2
// ============================================================

MissionStep mission1[] = {
    {MTXT_M1_1, PORTRAIT_PENULTIMO, MGOAL_M1_1, 6000, 25, 0, 0, 0, 0, 0, TILE_FARM_NW, 3, 0, 0},
    {MTXT_M1_2, PORTRAIT_PENULTIMO, MGOAL_M1_2,    0,  0, 0, 0, 0, 0, 0, 0,            0, 0, 0},
};

MissionStep mission2[] = {
    {MTXT_M2_1, PORTRAIT_PENULTIMO, MGOAL_M2_1, 0, 0, 0, 0, 60, 0, 100, TILE_PLANTATION_NW, 1, TILE_CHURCH_NW, 1},
    {MTXT_M2_2, PORTRAIT_PENULTIMO, MGOAL_M2_2, 0, 0, 0, 0,  0, 0,   0, 0,                  0, 0,              0},
};

MissionStep mission3[] = {
    {MTXT_M3_1, PORTRAIT_PENULTIMO, MGOAL_M3_1, 0, 50, 0, 0, 0, 0, 0, TILE_SCHOOL_NW, 1, TILE_HOSPITAL_NW, 1},
    {MTXT_M3_2, PORTRAIT_PENULTIMO, MGOAL_M3_2, 0,  0, 0, 0, 0, 0, 0, 0,              0, 0,                0},
};

MissionStep mission4[] = {
    {MTXT_M4_1, PORTRAIT_PENULTIMO, MGOAL_M4_1, 0, 0, 0, 0, 0, 200, 0, TILE_POWER_NW, 1, TILE_MINE_NW, 1},
    {MTXT_M4_2, PORTRAIT_PENULTIMO, MGOAL_M4_2, 0, 0, 0, 0, 0,   0, 0, 0,             0, 0,            0},
};

// ============================================================
// DISPATCH : copie l'étape step_idx de la mission courante
// dans *out. Appelée depuis nb_get_mission_step_b4 (ROM0).
// ============================================================
void copy_mission_step(uint8_t step_idx, MissionStep *out) {
    MissionStep *steps;
    if      (game.mission_id == 1) steps = mission2;
    else if (game.mission_id == 2) steps = mission3;
    else if (game.mission_id == 3) steps = mission4;
    else                           steps = mission1;
    *out = steps[step_idx];
}
