#ifndef STORY_MODE_H
#define STORY_MODE_H

#include <stdint.h>
#include "missions.h"

// Fonctions bank 4 — appelées via wrappers ROM0.
void story_mode_logic(void);
// Copie mission1[game.current_step] dans *out. Appelée via nb_get_story_step().
void story_get_current_step(MissionStep *out);

// Wrapper ROM0 (défini dans main.c) : SWITCH_ROM(2) → story_mode_logic() → SWITCH_ROM(1).
// Déclaré ici pour être visible depuis logic.c (bank 1).
void call_story_mode_logic(void);

// mission2[] est dans missions.c (bank 4) — déclaré dans missions.h

#endif
