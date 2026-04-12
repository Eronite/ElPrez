#ifndef ECONOMY_H
#define ECONOMY_H
#include <stdint.h>

// --- Rayons d'action (AoE) ---
#define POLICE_RADIUS    10
#define SCHOOL_RADIUS    12
#define HOSPITAL_RADIUS  12
#define CHURCH_RADIUS    8
#define BAR_RADIUS       10
#define POLLUTION_RADIUS 7
#define WORK_RADIUS      15


// Calcul mensuel de l'économie (ressources, emplois, bonheur, criminalité…)
void update_economy(void);

// Calcul étalé sur plusieurs frames
void update_economy_start(void);
uint8_t update_economy_tick(void);

// Wrappers nonbanked (bank 0) pour appels depuis economy.c (bank 3) vers bank 1
void call_update_economy(void);
void call_update_economy_start(void);
void call_update_economy_tick(void);
void nb_play_sound_build(void);
void nb_play_sound_error(void);
void nb_update_hud_b3(void);
void show_building_context_menu(uint8_t b_idx);

// Recherche le bâtiment sous le curseur — renvoie l'index dans building_registry,
// ou 0xFF si aucun bâtiment n'est trouvé.
uint8_t find_building_at_cursor(void);

// Affiche le menu d'amélioration pour le bâtiment à l'index donné.
void show_upgrade_menu(uint8_t bldg_idx);

#endif
