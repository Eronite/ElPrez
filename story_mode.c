#pragma bank 4
#include <gb/gb.h>
#include "story_mode.h"
#include "missions.h"
#include "game_state.h"
#include "constants.h"
#include "graphics.h"
#include "audio.h"

// Wrappers nonbanked définis dans main.c (ROM0) :
// nb_story_dialogue_b2    : charge le portrait, affiche le dialogue (bank 1), revient en bank 2
// nb_update_hud_b2        : appelle update_hud() en bank 1, revient en bank 2
// nb_play_mission_b2      : appelle play_sound_mission_success() en bank 1, revient en bank 2
// nb_get_mission_step_b4  : copie mission1[step] depuis bank 4, revient en bank 2
void nb_story_dialogue_b2(uint8_t portrait_id, uint16_t text_idx) __nonbanked;
void nb_update_hud_b2(void) __nonbanked;
void nb_play_mission_b2(void) __nonbanked;
void nb_get_mission_step_b4(uint8_t step_idx, MissionStep *out) __nonbanked;

// mission2[] déplacé dans missions.c (bank 4)

// ============================================================
// LECTURE DE L'ÉTAPE COURANTE (appelée via wrapper ROM0)
// Copie mission1[game.current_step] dans le buffer fourni par l'appelant.
// Appelée depuis nb_get_story_step() dans main.c pour que menus.c
// puisse lire les objectifs depuis la bank 1 sans accéder à la bank 2.
// ============================================================
void story_get_current_step(MissionStep *out) {
    nb_get_mission_step_b4(game.current_step, out);
}

// ============================================================
// VÉRIFICATION DES OBJECTIFS D'UNE ÉTAPE
// Retourne 1 si tous les objectifs non-nuls sont remplis
// et qu'il y a au moins un objectif défini.
// ============================================================
static uint8_t objectives_met(MissionStep *m) {
    uint8_t has_any = (m->target_money      > 0) |
                      (m->target_pop        > 0) |
                      (m->target_food_stock > 0) |
                      (m->target_food_prod  > 0) |
                      (m->target_happiness  > 0) |
                      (m->target_ore        > 0) |
                      (m->target_culture    > 0) |
                      (m->target_type      != 0) |
                      (m->target_type2     != 0);
    if (!has_any) return 0;

    if (m->target_money      > 0 && game.money          < m->target_money)       return 0;
    if (m->target_pop        > 0 && game.population     < m->target_pop)         return 0;
    if (m->target_food_stock > 0 && game.foodStock      < m->target_food_stock)  return 0;
    if (m->target_food_prod  > 0 && game.foodProduction < m->target_food_prod)   return 0;
    if (m->target_happiness  > 0 && game.avg_happiness  < m->target_happiness)   return 0;
    if (m->target_ore        > 0 && game.ore_stock      < m->target_ore)         return 0;
    if (m->target_culture    > 0 && game.culture_stock  < m->target_culture)     return 0;

    if (m->target_type != 0) {
        uint8_t cnt = 0;
        uint8_t i;
        for (i = 0; i < building_count; i++)
            if (building_registry[i].type == m->target_type) cnt++;
        if (cnt < m->target_count) return 0;
    }

    if (m->target_type2 != 0) {
        uint8_t cnt = 0;
        uint8_t i;
        for (i = 0; i < building_count; i++)
            if (building_registry[i].type == m->target_type2) cnt++;
        if (cnt < m->target_count2) return 0;
    }

    return 1;
}

// ============================================================
// LOGIQUE STORY PRINCIPALE
// Appelée chaque frame via call_story_mode_logic() (wrapper ROM0).
// ============================================================
void story_mode_logic(void) {
    if (game.game_mode != MODE_STORY) return;
    if (game.mission_id >= 4) return;

    // Lecture de l'étape courante depuis bank 4 (missions.c)
    MissionStep m_local;
    nb_get_mission_step_b4(game.current_step, &m_local);
    MissionStep *m = &m_local;

    // --- Dialogue initial de l'étape (pas encore vu) ---
    if (game.dialogue_seen == 0) {
        nb_story_dialogue_b2(m->portrait_id, m->dialogue_idx);
        game.dialogue_seen = 1;

        // pour les flash alerte manque d'élec (sprite écrasé sinon):
        //init_flash_pool();

        if (game.current_step > 0 || game.mission_id > 0) {
            // Retour propre à la map après dialogue mid-game ou début de mission suivante
            BGP_REG = 0xFF;
            move_win(7, 136);
            nb_update_hud_b2();
            fade_in();
        }
        return;
    }

    // --- Étape sans objectifs = dialogue de fin → passer immédiatement à la mission suivante ---
    {
        uint8_t has_any = (m->target_money      > 0) |
                          (m->target_pop        > 0) |
                          (m->target_food_stock > 0) |
                          (m->target_food_prod  > 0) |
                          (m->target_happiness  > 0) |
                          (m->target_ore        > 0) |
                          (m->target_culture    > 0) |
                          (m->target_type      != 0) |
                          (m->target_type2     != 0);
        if (!has_any) {
            game.mission_id++;
            game.current_step = 0;
            game.dialogue_seen = 0;
            nb_play_mission_b2();
            delay(250);
            if (game.mission_id >= 4) {
                // Dernière mission terminée : rétablir la palette au lieu de noircir
                BGP_REG = 0xE4;
                move_win(7, 136);
                nb_update_hud_b2();
                fade_in();
            } else {
                fade_out();
            }
            return;
        }
    }

    // --- Vérification mensuelle des objectifs ---
    if (!month_passed_flag) return;
    month_passed_flag = 0;

    if (!objectives_met(m)) return;

    // --- Objectifs remplis → étape suivante (dialogue de fin) ---
    game.current_step++;
    game.dialogue_seen = 0;
    nb_play_mission_b2();
    delay(250);
    fade_out();
}
