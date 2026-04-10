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
    "Presidente, notre ancienne\nile a d| prendre sa retraite\nsuite a un l~ger d~saccord\ng~ologique.\nMais r~jouissez-vous ! Le\ng~n~reux Ronald Dump nous a\noffert ce lopin de terre pour\nrebatir notre glorieuse\nutopie.\nIl dit que l'ile est\nmagnifique et que la personne\na laquelle elle appartient\nn'en a plus besoin.\nConstruisez d'abord une ferme,\nPresidente — votre peuple ne\nsait pas encore qu'il a faim,\nmais il apprendra vite !",
    // MTXT_M1_2
    "Presidente ! La ferme\nproduit ! Certes, personne\nne mange encore, mais les\nchiffres dans votre compte\nsont... positifs !\nLes fermes connect~es par\nune route vendent leur\nproduction.\nRemplissez les caisses :\nobjectif 5000 pesos.",
    // MTXT_M1_3
    "Bravo Presidente ! Les\ncaisses d~bordent !\nMais un d~fi plus grand vous\nattend : les gens. Ces\ncr~atures inexplicables qui\nr~clament des toits, de la\nnourriture, et pire encore...\ndu bonheur.\nConstruisez des logements\npour accueillir 30 habitants.",
    // MTXT_M1_4
    "Magnifique, Presidente !\nVous avez prouv~ que la\ncivilisation est possible...\nm~me ici.\nViva el Presidente !",
    // MGOAL_M1_1
    "Construire 1 ferme",
    // MGOAL_M1_2
    "Atteindre 5000$",
    // MGOAL_M1_3
    "Atteindre 30 habitants",
    // MGOAL_M1_4
    "Mission 1 termin~e !",

    // MTXT_M2_1
    "Presidente ! Votre ~le\nmange, vos caisses sonnent.\nMais le peuple... il s'ennuie.\nUn peuple qui s'ennuie pense.\nEt un peuple qui pense,\nc'est dangereux.\nLa plantation produit de la\nculture — du tabac, du caf~,\nde la vanille.\nConsid~rez ~a comme de l'art.\nConstruisez une plantation,\nPresidente !",
    // MTXT_M2_2
    "La plantation produit !\nLe peuple a maintenant acc~s\n~ la culture. Il n'en\ncomprend rien, mais il est\nfier.\nMais la culture seule ne\nsuffit pas — il faut aussi\nde la foi. Pas n~cessairement\nen vous, Presidente...\nmais ~a aide.\nL'~glise am~liore le bonheur\ng~n~ral. Construisez-en une\net accumulez 100 unit~s de\nculture.",
    // MTXT_M2_3
    "Excellent ! Votre peuple\nprie et produit — la\ncombinaison id~ale.\nMais Presidente, apr~s la\nmesse, il faut bien f~ter.\nLe bar est le temple du\npeuple libre. Quelques verres\net tout le monde oublie ses\nprobl~mes... et parfois m~me\nle loyer.\nConstruisez un bar et\natteignez un bonheur de 60.",
    // MTXT_M2_4
    "Presidente ! Votre peuple\nchante, prie, et boit.\nQue demander de plus ?\nViva el Presidente !",
    // MGOAL_M2_1
    "Construire 1 plantation",
    // MGOAL_M2_2
    "1 ~glise + 100 culture",
    // MGOAL_M2_3
    "1 bar + bonheur 60",
    // MGOAL_M2_4
    "Mission 2 termin~e !",

    // MTXT_M3_1
    "Presidente ! Votre peuple\nest heureux, mais il est...\ncomment dire... peu ~duqu~.\nUn peuple ~duqu~ travaille\nmieux, ob~it plus\nintelligemment, et surtout\nil comprend vos discours —\nce qui lui permet de les\napplaudir au bon moment.\nL'~cole am~liore l'~ducation\ndes habitants proches.\nConstruisez-en une !",
    // MTXT_M3_2
    "Les enfants apprennent !\nCertains apprennent m~me\ndes choses utiles.\nMais Presidente, les gens\ntombent malades. C'est\ninefficace.\nL'h~pital am~liore la sant~\ndes habitants proches et\nr~duit les d~c~s inutiles —\nles d~c~s utiles, eux,\nrestent n~gociables.\nConstruisez un h~pital !",
    // MTXT_M3_3
    "Votre peuple est ~duqu~\net en bonne sant~. Il\ncommence ~ avoir des id~es.\nNous ne pouvons pas laisser\n~a sans surveillance,\nPresidente.\nLa police r~duit la\ncriminalit~ dans son\np~rim~tre. Placez-la bien\net attirez 50 habitants.",
    // MTXT_M3_4
    "Magnifique Presidente !\n~ducation, sant~, ordre...\nvotre utopie prend forme.\nLe peuple est surveill~\npour son bien, soign~ pour\nsa productivit~, et ~duqu~\npour mieux vous ob~ir.\nViva el Presidente !",
    // MGOAL_M3_1
    "Construire 1 ~cole",
    // MGOAL_M3_2
    "Construire 1 h~pital",
    // MGOAL_M3_3
    "1 police + 50 habitants",
    // MGOAL_M3_4
    "Mission 3 termin~e !",

    // MTXT_M4_1
    "Presidente ! Votre ~le\nprosp~re, mais elle manque\nd'~nergie.\nSans ~lectricit~, la mine\nne peut pas fonctionner.\nLa scierie non plus. M~me\nla discoth~que reste\nsilencieuse — et un peuple\nsans musique est un peuple\nqui r~fl~chit trop.\nConstruisez une centrale\n~lectrique !",
    // MTXT_M4_2
    "L'~lectricit~ coule ! Les\nampoules s'allument et le\npeuple croit ~ un miracle\ndivin.\nMaintenant, les mines !\nCes vaillants trous dans\nle sol o| vos citoyens\ndescendent chaque matin\npour en ressortir moins\ngrands mais plus patriotes.\nAttention : la mine doit\n~tre construite sur un\ngisement de minerai.\nConstruisez 1 mine et\naccumulez 200 unit~s de\nminerai !",
    // MTXT_M4_3
    "Presidente ! Votre ~le\nextrait, produit, ~claire !\nLe minerai se vend, les\ncaisses grossissent, et le\npeuple descend fid~lement\ndans les profondeurs.\nViva el Presidente !",
    // MGOAL_M4_1
    "Construire 1 centrale",
    // MGOAL_M4_2
    "1 mine + 200 minerai",
    // MGOAL_M4_3
    "Mission 4 termin~e !",
};

// ============================================================
// TEXTES EN
// ============================================================
char *mission_strings_en[] = {
    // MTXT_M1_1
    "Presidente, our old island\nhas retired due to a minor\ngeological disagreement.\nBut rejoice! The generous\nRonald Dump has graciously\ngifted us this patch of dirt\nto rebuild our glorious\nutopia.\nHe says the island is\nmagnificent and that the\nperson it belongs to has\nno further use for it.\nBuild a farm first,\nPresidente — your people\ndon't yet know they're\nhungry, but they'll learn\nquickly!",
    // MTXT_M1_2
    "Presidente! The farm\nproduces! True, nobody is\neating yet, but the numbers\nin your account are...\npositive!\nFarms connected by roads\nsell their produce.\nFill the coffers:\ngoal 5000 pesos.",
    // MTXT_M1_3
    "Bravo Presidente! The\ncoffers overflow!\nBut a greater challenge\nawaits: people. These\ninexplicable creatures\ndemanding roofs, food,\nand worse... happiness.\nBuild housing to welcome\n30 inhabitants.",
    // MTXT_M1_4
    "Magnificent, Presidente!\nYou have proven that\ncivilization is possible...\neven here.\nViva el Presidente!",
    // MGOAL_M1_1
    "Build 1 farm",
    // MGOAL_M1_2
    "Reach 5000$",
    // MGOAL_M1_3
    "Reach 30 inhabitants",
    // MGOAL_M1_4
    "Mission 1 complete!",

    // MTXT_M2_1
    "Presidente! Your island\neats, your coffers ring.\nBut the people... they\nare bored.\nA bored people thinks.\nAnd a thinking people\nis dangerous.\nThe plantation produces\nculture — tobacco, coffee,\nvanilla. Consider it art.\nBuild a plantation,\nPresidente!",
    // MTXT_M2_2
    "The plantation produces!\nThe people now have access\nto culture. They understand\nnone of it, but they\nare proud.\nBut culture alone is not\nenough — faith is needed\ntoo. Not necessarily in\nyou, Presidente... but\nit helps.\nThe church improves overall\nhappiness. Build one and\naccumulate 100 culture.",
    // MTXT_M2_3
    "Excellent! Your people\npray and produce — the\nideal combination.\nBut Presidente, after mass,\none must celebrate. The bar\nis the temple of the free\npeople. A few drinks and\neveryone forgets their\nproblems... and sometimes\neven the rent.\nBuild a bar and reach\na happiness of 60.",
    // MTXT_M2_4
    "Presidente! Your people\nsing, pray, and drink.\nWhat more could one ask?\nViva el Presidente!",
    // MGOAL_M2_1
    "Build 1 plantation",
    // MGOAL_M2_2
    "1 church + 100 culture",
    // MGOAL_M2_3
    "1 bar + happiness 60",
    // MGOAL_M2_4
    "Mission 2 complete!",

    // MTXT_M3_1
    "Presidente! Your people\nare happy, but they are...\nhow to say... poorly\neducated.\nAn educated people works\nbetter, obeys more\nintelligently, and above\nall understands your\nspeeches — which allows\nthem to applaud at the\nright moment.\nThe school improves\neducation for nearby\nresidents. Build one!",
    // MTXT_M3_2
    "Children are learning!\nSome are even learning\nuseful things.\nBut Presidente, people\nget sick. That is\ninefficient.\nThe hospital improves\nhealth for nearby residents\nand reduces unnecessary\ndeaths — useful deaths,\nhowever, remain negotiable.\nBuild a hospital!",
    // MTXT_M3_3
    "Your people are educated\nand healthy. They are\nstarting to have ideas.\nWe cannot leave that\nunsupervised, Presidente.\nThe police reduces crime\nin its perimeter.\nPlace it well and attract\n50 inhabitants.",
    // MTXT_M3_4
    "Magnificent Presidente!\nEducation, health, order...\nyour utopia takes shape.\nThe people are watched\nfor their own good, healed\nfor their productivity,\nand educated to better\nobey you.\nViva el Presidente!",
    // MGOAL_M3_1
    "Build 1 school",
    // MGOAL_M3_2
    "Build 1 hospital",
    // MGOAL_M3_3
    "1 police + 50 inhabitants",
    // MGOAL_M3_4
    "Mission 3 complete!",

    // MTXT_M4_1
    "Presidente! Your island\nprospers, but it lacks\nenergy.\nWithout electricity, the\nmine cannot function.\nNor can the sawmill. Even\nthe disco stays silent —\nand a people without music\nis a people that thinks\ntoo much.\nBuild a power plant!",
    // MTXT_M4_2
    "Electricity flows! Lights\nturn on and the people\nbelieve it's a divine\nmiracle.\nNow, the mines! Those\nvaliant holes in the ground\nwhere your citizens descend\neach morning and emerge\nshorter but more patriotic.\nNote: the mine must be\nbuilt on an ore deposit.\nBuild 1 mine and\naccumulate 200 ore!",
    // MTXT_M4_3
    "Presidente! Your island\nextracts, produces, shines!\nThe ore sells, the coffers\ngrow, and the people\nfaithfully descend into\nthe depths.\nViva el Presidente!",
    // MGOAL_M4_1
    "Build 1 power plant",
    // MGOAL_M4_2
    "1 mine + 200 ore",
    // MGOAL_M4_3
    "Mission 4 complete!",
};

// ============================================================
// TABLEAUX DE MISSIONS
// Champs : dialogue_idx, portrait_id, goal_idx,
//          target_money, target_pop, target_food_stock,
//          target_food_prod, target_happiness, target_ore,
//          target_culture, target_type, target_count
// ============================================================

MissionStep mission1[] = {
    {MTXT_M1_1, PORTRAIT_PENULTIMO, MGOAL_M1_1,    0,  0, 0, 0, 0,   0,   0, TILE_FARM_NW, 1},
    {MTXT_M1_2, PORTRAIT_PENULTIMO, MGOAL_M1_2, 5000,  0, 0, 0, 0,   0,   0, 0,            0},
    {MTXT_M1_3, PORTRAIT_PENULTIMO, MGOAL_M1_3,    0, 30, 0, 0, 0,   0,   0, 0,            0},
    {MTXT_M1_4, PORTRAIT_PENULTIMO, MGOAL_M1_4,    0,  0, 0, 0, 0,   0,   0, 0,            0},
};

MissionStep mission2[] = {
    {MTXT_M2_1, PORTRAIT_PENULTIMO, MGOAL_M2_1,    0,  0, 0, 0,  0,   0,   0, TILE_PLANTATION_NW, 1},
    {MTXT_M2_2, PORTRAIT_PENULTIMO, MGOAL_M2_2,    0,  0, 0, 0,  0,   0, 100, TILE_CHURCH_NW,     1},
    {MTXT_M2_3, PORTRAIT_PENULTIMO, MGOAL_M2_3,    0,  0, 0, 0, 60,   0,   0, TILE_BAR_NW,        1},
    {MTXT_M2_4, PORTRAIT_PENULTIMO, MGOAL_M2_4,    0,  0, 0, 0,  0,   0,   0, 0,                  0},
};

MissionStep mission3[] = {
    {MTXT_M3_1, PORTRAIT_PENULTIMO, MGOAL_M3_1,    0,  0, 0, 0,  0,   0,   0, TILE_SCHOOL_NW,   1},
    {MTXT_M3_2, PORTRAIT_PENULTIMO, MGOAL_M3_2,    0,  0, 0, 0,  0,   0,   0, TILE_HOSPITAL_NW, 1},
    {MTXT_M3_3, PORTRAIT_PENULTIMO, MGOAL_M3_3,    0, 50, 0, 0,  0,   0,   0, TILE_POLICE_NW,   1},
    {MTXT_M3_4, PORTRAIT_PENULTIMO, MGOAL_M3_4,    0,  0, 0, 0,  0,   0,   0, 0,                0},
};

MissionStep mission4[] = {
    {MTXT_M4_1, PORTRAIT_PENULTIMO, MGOAL_M4_1,    0,  0, 0, 0,  0,   0,   0, TILE_POWER_NW, 1},
    {MTXT_M4_2, PORTRAIT_PENULTIMO, MGOAL_M4_2,    0,  0, 0, 0,  0, 200,   0, TILE_MINE_NW,  1},
    {MTXT_M4_3, PORTRAIT_PENULTIMO, MGOAL_M4_3,    0,  0, 0, 0,  0,   0,   0, 0,             0},
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
