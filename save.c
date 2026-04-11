#pragma bank 2
#include <gb/gb.h>
#include "game_state.h"
#include "graphics.h"
#include "constants.h"
#include "save.h"
#include "lang.h"
#include "logic.h"
// Wrappers nonbanked (définis dans main.c) pour appels depuis bank 2 → bank 1
extern void nb_load_water_map_b2(void);
extern void nb_enable_sram_b2(void);
extern void nb_disable_sram_b2(void);
extern void nb_switch_ram_bank_b2(uint8_t bank);
extern void nb_init_game_variables_b2(void);
extern void nb_rebuild_registry_b2(void);
extern void nb_play_sound_erase_b2(void);
extern void nb_play_sound_build_b2(void);
extern void nb_boat_init_b2(void);

uint8_t confirm_delete(uint8_t slot_idx) {
    uint8_t sub_selection = 1; // Par défaut sur "NO"
    //uint8_t blink_counter = 0;
    char slot_name[2] = "A";
    slot_name[0] = 'A' + slot_idx; // Transforme 0, 1, 2 en A, B, C

    // On prépare l'affichage de la box
    if (game.language == LANG_EN) {
        draw_text(4, 13, "Delete slot ", 1);
        draw_text(16, 13, slot_name, 1);
        draw_text(17, 13, "?", 1);
    } else {
        draw_text(1, 13, "Supprimer slot ", 1);
        draw_text(16, 13, slot_name, 1);
        draw_text(17, 13, "?", 1);
    }
    
    while(1) {
        if (game.language == LANG_EN) {
            draw_text(4, 15, sub_selection == 0 ? ">Yes" : " Yes", 1);
            draw_text(11, 15, sub_selection == 1 ? ">No " : " No ", 1);
        } else {
            draw_text(4, 15, sub_selection == 0 ? ">Oui" : " Oui", 1);
            draw_text(11, 15, sub_selection == 1 ? ">Non " : " Non ", 1); 
        }

        uint8_t joy = joypad();
        //blink_counter++;
        if (joy & J_LEFT)  { sub_selection = 0; nb_play_sound_build_b2(); waitpadup(); }
        if (joy & J_RIGHT) { sub_selection = 1; nb_play_sound_build_b2(); waitpadup(); }

        // AFFICHAGE DU CURSEUR
        // Si sub_selection = 0 (YES), X = 32. Si 1 (NO), X = 88.
        uint8_t curs_x = (sub_selection == 0) ? 32 : 88;
        uint8_t curs_y = 136; // Ligne 15 * 8 + 16px d'offset VBL

        /*if ((blink_counter / 10) % 2 == 0) {
            move_sprite(0, curs_x, curs_y);
        } else {
            move_sprite(0, 0, 0);
        }*/

        if (joy & J_A) {
            waitpadup();
            // Nettoyage de la zone de texte
            draw_text(1, 13, "                  ", 1);
            draw_text(1, 15, "                  ", 1);
            return (sub_selection == 0); 
        }
        if (joy & J_B) {
            nb_play_sound_build_b2();
            // Nettoyage avant de partir
            draw_text(1, 13, "                 ", 1);
            draw_text(1, 15, "                 ", 1);
            move_sprite(0, 0, 0);
            waitpadup();
            return 0;
        }
        wait_vbl_done();
    }
}

void wipe_slot_by_index(uint8_t slot) {
    nb_enable_sram_b2();
    nb_switch_ram_bank_b2(slot);
    
    // 1. On vide la carte
    //for (uint16_t i = 0; i < 4096; i++) SRAM_MAP_PTR[i] = TYPE_EMPTY;
    
    // 2. On invalide la signature de la structure GameState
    // On pointe après la carte (4096)
    GameState *sram_game = (GameState *)(0xA000 + 4096);
    sram_game->signature = 0; // 0 = Slot vide / Invalide

    // On vide aussi le tampon RAM de travail pour éviter les fuites entre slots
    for(uint16_t i=0; i<4096; i++) ram_map[i] = 128; // tile herbe
    
    nb_disable_sram_b2();
    nb_play_sound_erase_b2(); // Petit son de destruction pour confirmer
}

/*uint8_t load_saves_screen() {
    uint8_t slot_sel = 0;   // 0=A, 1=B, 2=C
    uint8_t action_sel = 0; // 0=Load, 1=Delete
    uint8_t blink_counter = 0;
    
    clear_entire_window();
    waitpadup();

    while(1) {
        draw_text(2, 1, "--- FREE PLAY ---", 1);
        draw_text(4, 4, "SELECT SLOT:", 1);
        
        // Affichage des lignes de slots
        //draw_text(4, 6, "SLOT A", 1);  draw_text(13, 6, "DEL", 1);

        // Vérification du contenu du slot en SRAM
        //nb_enable_sram_b2();
        //nb_switch_ram_bank_b2(0); // Slot A=0, B=1, C=2
        //GameState *sram_check0 = (GameState *)(0xA000 + 4096);
        
        //if (sram_check0->signature == 99) {
            // Texte si occupé (on décale d'une ligne vers le bas : Y + 1)
            //draw_text(4, 7, "Used", 1);
        //} else {
            // Texte si vide
            //draw_text(4, 7, "Empty", 1);
        //}
        //nb_disable_sram_b2();


        draw_text(4, 7, "SLOT 1", 1); draw_text(13, 7, "DEL.", 1);
        // Vérification du contenu du slot en SRAM
        nb_enable_sram_b2();
        nb_switch_ram_bank_b2(1); // Slot A=0, B=1, C=2
        GameState *sram_check1 = (GameState *)(0xA000 + 4096);
        
        if (sram_check1->signature == 99) {
            // Texte si occupé (on décale d'une ligne vers le bas : Y + 1)
            draw_text(4, 8, "Used", 1);
        } else {
            // Texte si vide
            draw_text(4, 8, "Empty", 1);
        }
        nb_disable_sram_b2();


        draw_text(4, 10, "SLOT 2", 1); draw_text(13, 10, "DEL.", 1);
                // Vérification du contenu du slot en SRAM
        nb_enable_sram_b2();
        nb_switch_ram_bank_b2(2); // Slot A=0, B=1, C=2
        GameState *sram_check2 = (GameState *)(0xA000 + 4096);
        
        if (sram_check2->signature == 99) {
            // Texte si occupé (on décale d'une ligne vers le bas : Y + 1)
            draw_text(4, 11, "Used", 1);
        } else {
            // Texte si vide
            draw_text(4, 11, "Empty", 1);
        }
        nb_disable_sram_b2();


        draw_text(5, 16, "B: BACK", 1);

        // Gestion du curseur (Sprite 0)
        blink_counter++;
        uint8_t curs_x_pos = (action_sel == 0) ? 24 : 96; // Gauche ou Droite
        uint8_t curs_y_pos = (slot_sel * 8 *3) + 72;
        
        if ((blink_counter / 20) % 2 == 0) move_sprite(0, curs_x_pos, curs_y_pos);
        else move_sprite(0, 0, 0);

        uint8_t joy = joypad();
        
        // Navigation verticale
        if (joy & J_DOWN && slot_sel < 1) { slot_sel++; play_sound_build(); waitpadup(); }
        if (joy & J_UP && slot_sel > 0)   { slot_sel--; play_sound_build(); waitpadup(); }
        
        // Navigation horizontale (entre Charger et Supprimer)
        if (joy & J_RIGHT && action_sel == 0) { action_sel = 1; play_sound_build(); waitpadup(); }
        if (joy & J_LEFT && action_sel == 1)  { action_sel = 0; play_sound_build(); waitpadup(); }

        if (joy & J_B) {
            clear_entire_window();
            waitpadup();
            move_sprite(0, 0, 0);
            return 0;
        }

        if (joy & J_A) {
            waitpadup();
            if (action_sel == 1) {
                // --- OPTION DELETE ---
                if (confirm_delete(slot_sel)) {
                    move_sprite(0, 0, 0);
                    wipe_slot_by_index(slot_sel);
                    draw_text(4, 14, "SLOT DELETED", 1);
                    delay(800);
                    draw_text(4, 14, "             ", 1);
                }
            } else {
                // --- OPTION LOAD ---
                current_save_slot = slot_sel;
                //is_story_mode = 0;
                clear_entire_window();

                nb_enable_sram_b2();
                nb_switch_ram_bank_b2(current_save_slot);
                GameState *sram_ptr = (GameState *)(0xA000 + 4096);

                if (sram_ptr->signature == 99 && sram_ptr->money > 0) { // le slot est valide, on charge
                    game = *sram_ptr;
                    for(uint16_t i=0; i<4096; i++) ram_map[i] = SRAM_MAP_PTR[i];
                    //nb_disable_sram_b2();
                } else { // slot "sale" ou vide
                    //nb_disable_sram_b2();
                    //wipe_slot_by_index(current_save_slot);
                    game.game_mode = MODE_FREEPLAY;
                    // IMPORTANT : Si le slot est vide, on doit vider la RAM de travail !
                    for(uint16_t i=0; i<4096; i++) ram_map[i] = 128; // 128 : herbe ?
                    nb_init_game_variables_b2();
                    nb_boat_init_b2();
                    //*sram_ptr = game;
                    //perform_save(); // on met la signature à 99 dans cette fonction
                }
                move_sprite(0, 0, 0);
                return 1; 
            }
        }
        wait_vbl_done();
    }
}*/

uint8_t load_saves_screen() {
    uint8_t slot_sel = 0;   // 0 (affiche Slot 1), 1 (affiche Slot 2)
    uint8_t action_sel = 0; // 0=Load, 1=Delete
    uint8_t blink_counter = 0;
    
    clear_entire_window();
    nb_draw_menu_border();
    draw_presidente_bkg(1);
    waitpadup();

    // On dessine le texte statique UNE SEULE FOIS avant la boucle
    if (game.language == LANG_EN) {
    draw_text(4, 7, "Select slot:", 1);
    draw_text(4, 10, "Slot A", 1); draw_text(13, 10, "Del.", 1);
    draw_text(4, 13, "Slot B", 1); draw_text(13, 13, "Del.", 1);
    draw_text(4, 16, "B: Back", 1);
    } else {
    draw_text(2, 7, "Choisis un slot:", 1);
    draw_text(3, 10, "Slot A", 1); draw_text(12, 10, "Suppr.", 1);
    draw_text(3, 13, "Slot B", 1); draw_text(12, 13, "Suppr.", 1);
    draw_text(2, 16, "B: Retour", 1);
    }

    while(1) {
        // --- MISE À JOUR DE L'ÉTAT (Used/Empty) ---
        // On ne le fait que si nécessaire ou une fois de temps en temps pour éviter le flicker
        if (blink_counter % 30 == 0) { 
            nb_enable_sram_b2();
            for (uint8_t i = 0; i < 2; i++) {
                nb_switch_ram_bank_b2(i + 1); // Vérifie banque 1 puis banque 2
                GameState *s_check = (GameState *)(0xA000 + 4096);
                if (s_check->signature == 99) {
                    if (game.language == LANG_EN) {
                        draw_text(4, 11 + (i * 3), "Used ", 1);
                    } else {
                        draw_text(3, 11 + (i * 3), "Utilis~ ", 1);
                    }
                } else {
                    if (game.language == LANG_EN) {
                        draw_text(4, 11 + (i * 3), "Empty", 1);
                    } else {
                        draw_text(3, 11 + (i * 3), "Vide", 1);
                    }
                }
            }
            nb_disable_sram_b2();
        }

        // --- GESTION DU CURSEUR ---
        if (++blink_counter >= 30) blink_counter = 0;
        uint8_t curs_x_pos = (action_sel == 0) ? 24 : 96;
        uint8_t curs_y_pos = (slot_sel == 0) ? 96 : 120;

        if (blink_counter < 15) move_sprite(0, curs_x_pos, curs_y_pos);
        else move_sprite(0, 0, 0);

        uint8_t joy = joypad();
        
        if (joy & J_DOWN && slot_sel < 1) { slot_sel++; nb_play_sound_build_b2(); waitpadup(); }
        if (joy & J_UP && slot_sel > 0)   { slot_sel--; nb_play_sound_build_b2(); waitpadup(); }
        if (joy & J_RIGHT && action_sel == 0) { action_sel = 1; nb_play_sound_build_b2(); waitpadup(); }
        if (joy & J_LEFT && action_sel == 1)  { action_sel = 0; nb_play_sound_build_b2(); waitpadup(); }

        if (joy & J_B) {
            clear_entire_window();
            waitpadup();
            move_sprite(0, 0, 0);
            restore_shop_tiles();
            return 0;
        }

        if (joy & J_A) {
            waitpadup();
            current_save_slot = slot_sel + 1; // IMPORTANT : C'est ici le décalage +1

            if (action_sel == 1) {
                // Effacer tout le texte du menu avant la popup
                clear_entire_window();
                nb_draw_menu_border();
                draw_presidente_bkg(1);
                move_sprite(0, 0, 0);

                uint8_t confirmed = confirm_delete(slot_sel);

                // Réafficher tout le texte du menu après la popup
                if (game.language == LANG_EN) {
                    draw_text(4, 7, "Select slot:", 1);
                    draw_text(4, 10, "Slot A", 1); draw_text(13, 10, "Del.", 1);
                    draw_text(4, 13, "Slot B", 1); draw_text(13, 13, "Del.", 1);
                    draw_text(4, 16, "B: Back", 1);
                } else {
                    draw_text(2, 7, "Choisis un slot:", 1);
                    draw_text(3, 10, "Slot A", 1); draw_text(12, 10, "Suppr.", 1);
                    draw_text(3, 13, "Slot B", 1); draw_text(12, 13, "Suppr.", 1);
                    draw_text(2, 16, "B: Retour", 1);
                }
                blink_counter = 0; // Force re-check Used/Vide immédiatement

                if (confirmed) {
                    wipe_slot_by_index(current_save_slot); // On wipe la banque 1 ou 2
                }
            } else {
                // CHARGEMENT
                nb_enable_sram_b2();
                nb_switch_ram_bank_b2(current_save_slot);
                GameState *sram_ptr = (GameState *)(0xA000 + 4096);

                if (sram_ptr->signature == 99) {
                    uint8_t saved_lang = game.language;
                    game = *sram_ptr;
                    game.language = saved_lang;
                    for(uint16_t i=0; i<4096; i++) ram_map[i] = SRAM_MAP_PTR[i];
                    nb_rebuild_registry_b2();
                    // Restaurer les flags par map_idx (format : count, [idx_hi, idx_lo, flags, salary, occupants]...)
                    {
                        uint8_t *sram_upg = (uint8_t *)(0xA000 + 4096) + sizeof(GameState);
                        uint8_t saved_count = *sram_upg++;
                        uint8_t k, j;
                        for (k = 0; k < saved_count; k++) {
                            uint16_t sidx = ((uint16_t)sram_upg[0] << 8) | sram_upg[1];
                            uint8_t  sflg = sram_upg[2];
                            uint8_t  ssal = sram_upg[3];
                            uint8_t  socc = sram_upg[4];
                            sram_upg += 5;
                            for (j = 0; j < building_count; j++) {
                                if (building_registry[j].map_idx == sidx) {
                                    building_registry[j].flags |= sflg & (BLDG_UPG1_APPLIED | BLDG_UPG2_APPLIED | BLDG_FLAG_HAS_ORE);
                                    building_salary[j]              = ssal;
                                    building_registry[j].occupants  = socc;
                                    break;
                                }
                            }
                        }
                    }
                    curs_x = game.saved_curs_x;
                    curs_y = game.saved_curs_y;
                    cam_x  = game.saved_cam_x;
                    cam_y  = game.saved_cam_y;
                    cam_target_x = cam_x; cam_target_y = cam_y;
                    nb_boat_init_b2();
                } else {
                    // Nouveau jeu sur ce slot
                    game.game_mode = MODE_FREEPLAY;
                    nb_load_water_map_b2();
                    nb_init_game_variables_b2();
                    nb_boat_init_b2();
                    game.money = 5000;
                    curs_x = 32; curs_y = 32;
                    cam_x = 176; cam_y = 184;
                    cam_target_x = cam_x; cam_target_y = cam_y;
                }
                nb_disable_sram_b2();
                move_sprite(0, 0, 0);
                restore_shop_tiles();
                return 1;
            }
        }
        wait_vbl_done();
    }
}


void save_story_game() {
    nb_enable_sram_b2();
    nb_switch_ram_bank_b2(3); // Toujours bank 3
    nb_disable_sram_b2();
}

// Profondeur de chaque bord : nombre de tiles d'eau pleine avant la tile de transition.
// Valeurs 2..5 (transition = +1, donc eau visible sur 2..5 tiles + 1 tile de transition).
// Aspect organique : varie doucement, jamais de saut de plus de 1 entre cases adjacentes.

// Bord NORD : profondeur pour chaque colonne x=0..63
static const uint8_t wdepth_N[64] = {
    3,3,4,4,5,5,4,3, 3,2,2,3,4,5,5,4,
    4,3,3,4,5,5,4,3, 3,3,4,4,3,3,2,2,
    2,3,3,4,4,5,4,3, 3,2,2,3,4,4,3,3,
    3,4,4,5,5,4,3,3, 2,2,3,3,4,4,3,3
};
// Bord SUD : profondeur pour chaque colonne x=0..63 (depuis y=63 vers le haut)
static const uint8_t wdepth_S[64] = {
    2,3,3,4,4,3,3,2, 3,3,4,4,5,4,3,3,
    3,2,2,3,4,5,4,3, 3,3,4,4,3,2,2,3,
    4,4,3,3,2,3,3,4, 4,5,4,3,3,2,3,3,
    4,4,3,3,2,2,3,4, 4,3,3,2,3,3,4,4
};
// Bord OUEST : profondeur pour chaque ligne y=0..63 (depuis x=0 vers la droite)
static const uint8_t wdepth_W[64] = {
    3,3,4,4,5,4,3,3, 2,2,3,4,4,3,2,3,
    3,4,4,5,4,3,3,2, 2,3,3,4,3,3,2,2,
    3,3,4,5,5,4,3,3, 2,2,3,3,4,4,3,2,
    3,3,4,4,5,4,3,3, 2,3,3,4,4,3,3,2
};
// Bord EST : profondeur pour chaque ligne y=0..63 (depuis x=63 vers la gauche)
static const uint8_t wdepth_E[64] = {
    2,3,3,4,5,4,3,2, 2,3,4,4,3,3,2,3,
    4,4,5,4,3,3,2,2, 3,3,4,4,3,2,2,3,
    3,4,5,5,4,3,3,2, 2,3,3,4,4,3,2,3,
    3,4,4,5,4,3,2,2, 3,3,4,4,3,3,2,3
};

static void init_water_border(void) {
    uint8_t x, y, d;

    // --- Bord NORD : eau depuis y=0, transition à y=depth ---
    for(x = 1; x < 63; x++) {
        d = wdepth_N[x];
        for(y = 0; y < d; y++)
            ram_map[(uint16_t)y*64 + x] = TILE_WATER;
        ram_map[(uint16_t)d*64 + x] = TILE_WATER_N;
    }

    // --- Bord SUD : eau depuis y=63, transition à y=63-depth ---
    for(x = 1; x < 63; x++) {
        d = wdepth_S[x];
        for(y = 0; y < d; y++)
            ram_map[(uint16_t)(63-y)*64 + x] = TILE_WATER;
        ram_map[(uint16_t)(63-d)*64 + x] = TILE_WATER_S;
    }

    // --- Bord OUEST : eau depuis x=0, transition à x=depth ---
    for(y = 1; y < 63; y++) {
        d = wdepth_W[y];
        for(x = 0; x < d; x++)
            ram_map[(uint16_t)y*64 + x] = TILE_WATER;
        ram_map[(uint16_t)y*64 + d] = TILE_WATER_W;
    }

    // --- Bord EST : eau depuis x=63, transition à x=63-depth ---
    for(y = 1; y < 63; y++) {
        d = wdepth_E[y];
        for(x = 0; x < d; x++)
            ram_map[(uint16_t)y*64 + (63-x)] = TILE_WATER;
        ram_map[(uint16_t)y*64 + (63-d)] = TILE_WATER_E;
    }

    // --- Coins : eau pleine dans les 4 angles, tile de coin à l'intersection ---
    // NW : eau sur le coin, coin NW là où les deux transitions se rejoignent
    {
        uint8_t dN0  = wdepth_N[1];   // profondeur N à x=1 (première colonne intérieure)
        uint8_t dW0  = wdepth_W[1];   // profondeur W à y=1

        // Remplir l'angle NW d'eau pleine
        for(y = 0; y < dN0; y++)
            for(x = 0; x < dW0; x++)
                ram_map[(uint16_t)y*64 + x] = TILE_WATER;
        // Tile coin NW à l'intersection
        ram_map[(uint16_t)dN0*64 + dW0] = TILE_WATER_NW;
        // Bord N prolongé sur la colonne 0
        for(y = 0; y < dN0; y++) ram_map[(uint16_t)y*64 + 0] = TILE_WATER;
        ram_map[(uint16_t)dN0*64 + 0] = TILE_WATER_N;
        // Bord W prolongé sur la ligne 0
        for(x = 0; x < dW0; x++) ram_map[x] = TILE_WATER;
        ram_map[dW0] = TILE_WATER_W;
    }
    {
        uint8_t dN63 = wdepth_N[62];  // profondeur N à x=62
        uint8_t dE0  = wdepth_E[1];   // profondeur E à y=1

        for(y = 0; y < dN63; y++)
            for(x = 0; x < dE0; x++)
                ram_map[(uint16_t)y*64 + (63-x)] = TILE_WATER;
        ram_map[(uint16_t)dN63*64 + (63-dE0)] = TILE_WATER_NE;
        for(y = 0; y < dN63; y++) ram_map[(uint16_t)y*64 + 63] = TILE_WATER;
        ram_map[(uint16_t)dN63*64 + 63] = TILE_WATER_N;
        for(x = 0; x < dE0; x++) ram_map[63-x] = TILE_WATER;
        ram_map[63-dE0] = TILE_WATER_E;
    }
    {
        uint8_t dS0  = wdepth_S[1];
        uint8_t dW63 = wdepth_W[62];

        for(y = 0; y < dS0; y++)
            for(x = 0; x < dW63; x++)
                ram_map[(uint16_t)(63-y)*64 + x] = TILE_WATER;
        ram_map[(uint16_t)(63-dS0)*64 + dW63] = TILE_WATER_SW;
        for(y = 0; y < dS0; y++) ram_map[(uint16_t)(63-y)*64 + 0] = TILE_WATER;
        ram_map[(uint16_t)(63-dS0)*64 + 0] = TILE_WATER_S;
        for(x = 0; x < dW63; x++) ram_map[(uint16_t)63*64 + x] = TILE_WATER;
        ram_map[(uint16_t)63*64 + dW63] = TILE_WATER_W;
    }
    {
        uint8_t dS63 = wdepth_S[62];
        uint8_t dE63 = wdepth_E[62];

        for(y = 0; y < dS63; y++)
            for(x = 0; x < dE63; x++)
                ram_map[(uint16_t)(63-y)*64 + (63-x)] = TILE_WATER;
        ram_map[(uint16_t)(63-dS63)*64 + (63-dE63)] = TILE_WATER_SE;
        for(y = 0; y < dS63; y++) ram_map[(uint16_t)(63-y)*64 + 63] = TILE_WATER;
        ram_map[(uint16_t)(63-dS63)*64 + 63] = TILE_WATER_S;
        for(x = 0; x < dE63; x++) ram_map[(uint16_t)63*64 + (63-x)] = TILE_WATER;
        ram_map[(uint16_t)63*64 + (63-dE63)] = TILE_WATER_E;
    }
}

static void init_map_mission1(void) {
    nb_load_water_map_b2();

    // Route horizontale (y=32, x=28..40)
    for(uint8_t x = 28; x <= 40; x++)
        ram_map[(uint16_t)32 * 64 + x] = VAL_ROAD;

    // Maison 1 NW en (28, 29) — rangée du bas (row+2 = 31) adjacent à route y=32
    ram_map[(uint16_t)29*64 + 28] = TILE_HOUSE_NW + 0;
    ram_map[(uint16_t)29*64 + 29] = TILE_HOUSE_NW + 1;
    ram_map[(uint16_t)29*64 + 30] = TILE_HOUSE_NW + 2;
    ram_map[(uint16_t)30*64 + 28] = TILE_HOUSE_NW + 3;
    ram_map[(uint16_t)30*64 + 29] = TILE_HOUSE_NW + 4;
    ram_map[(uint16_t)30*64 + 30] = TILE_HOUSE_NW + 5;
    ram_map[(uint16_t)31*64 + 28] = TILE_HOUSE_NW + 6;
    ram_map[(uint16_t)31*64 + 29] = TILE_HOUSE_NW + 7;
    ram_map[(uint16_t)31*64 + 30] = TILE_HOUSE_NW + 8;

    // Maison 2 NW en (32, 29)
    ram_map[(uint16_t)29*64 + 32] = TILE_HOUSE_NW + 0;
    ram_map[(uint16_t)29*64 + 33] = TILE_HOUSE_NW + 1;
    ram_map[(uint16_t)29*64 + 34] = TILE_HOUSE_NW + 2;
    ram_map[(uint16_t)30*64 + 32] = TILE_HOUSE_NW + 3;
    ram_map[(uint16_t)30*64 + 33] = TILE_HOUSE_NW + 4;
    ram_map[(uint16_t)30*64 + 34] = TILE_HOUSE_NW + 5;
    ram_map[(uint16_t)31*64 + 32] = TILE_HOUSE_NW + 6;
    ram_map[(uint16_t)31*64 + 33] = TILE_HOUSE_NW + 7;
    ram_map[(uint16_t)31*64 + 34] = TILE_HOUSE_NW + 8;

    // Maison 3 NW en (36, 29)
    ram_map[(uint16_t)29*64 + 36] = TILE_HOUSE_NW + 0;
    ram_map[(uint16_t)29*64 + 37] = TILE_HOUSE_NW + 1;
    ram_map[(uint16_t)29*64 + 38] = TILE_HOUSE_NW + 2;
    ram_map[(uint16_t)30*64 + 36] = TILE_HOUSE_NW + 3;
    ram_map[(uint16_t)30*64 + 37] = TILE_HOUSE_NW + 4;
    ram_map[(uint16_t)30*64 + 38] = TILE_HOUSE_NW + 5;
    ram_map[(uint16_t)31*64 + 36] = TILE_HOUSE_NW + 6;
    ram_map[(uint16_t)31*64 + 37] = TILE_HOUSE_NW + 7;
    ram_map[(uint16_t)31*64 + 38] = TILE_HOUSE_NW + 8;
}

void load_story_game(uint8_t bank_index) {



    current_save_slot = bank_index; // Ce sera 3

    nb_enable_sram_b2();
    nb_switch_ram_bank_b2(current_save_slot);

    // 2. On pointe vers la struct en SRAM
    GameState *sram_game = (GameState *)(0xA000 + 4096);

    // 3. On charge tout le bloc d'un coup
    if (sram_game->signature == 99) {
        uint8_t current_lang = game.language; // On mémorise la langue choisie au menu

        game = *sram_game; // On charge les données (argent, population, etc.)

        game.language = current_lang;         // On restaure la langue mémorisée
        
        // Et n'oubliez pas de charger la map de la SRAM vers votre tampon RAM :
        /*uint8_t *sram_map_ptr = (uint8_t *)0xA000;
        for (uint16_t i = 0; i < 4096; i++) {
            ram_map[i] = sram_map_ptr[i];
        }*/

        for(uint16_t i=0; i<4096; i++) ram_map[i] = SRAM_MAP_PTR[i];
        nb_boat_init_b2();
        nb_rebuild_registry_b2();
        // Restaurer les flags par map_idx (format : count, [idx_hi, idx_lo, flags, salary, occupants]...)
        {
            uint8_t *sram_upg = (uint8_t *)(0xA000 + 4096) + sizeof(GameState);
            uint8_t saved_count = *sram_upg++;
            uint8_t k, j;
            for (k = 0; k < saved_count; k++) {
                uint16_t sidx = ((uint16_t)sram_upg[0] << 8) | sram_upg[1];
                uint8_t  sflg = sram_upg[2];
                uint8_t  ssal = sram_upg[3];
                uint8_t  socc = sram_upg[4];
                sram_upg += 5;
                for (j = 0; j < building_count; j++) {
                    if (building_registry[j].map_idx == sidx) {
                        building_registry[j].flags |= sflg & (BLDG_UPG1_APPLIED | BLDG_UPG2_APPLIED | BLDG_FLAG_HAS_ORE);
                        building_salary[j]             = ssal;
                        building_registry[j].occupants = socc;
                        break;
                    }
                }
            }
        }
        curs_x = game.saved_curs_x;
        curs_y = game.saved_curs_y;
        cam_x  = game.saved_cam_x;
        cam_y  = game.saved_cam_y;
        cam_target_x = cam_x; cam_target_y = cam_y;

        // RÈGLE ERGONOMIQUE : On ne veut pas revoir le dialogue immédiatement
        // car on "continue" une partie.
        game.dialogue_seen = 1; 
        } else {
        // Slot vide : charger la map de départ de la mission 1
        init_map_mission1();
        nb_init_game_variables_b2();
        nb_boat_init_b2();
        nb_rebuild_registry_b2();
        curs_x = 32; curs_y = 32;
        cam_x = 176; cam_y = 184;
        cam_target_x = cam_x; cam_target_y = cam_y;
        }

    nb_disable_sram_b2();
}