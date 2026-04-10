#pragma bank 2
#include <gb/gb.h>
#include "constants.h"
#include "graphics.h"
#include "game_state.h"
#include "missions.h"
#include "logic.h"
#include "save.h"
#include "menus.h"
#include "lang.h"
#include "minimap.h"
#include "story_mode.h"
#include <stdlib.h> // Nécessaire pour rand()

// Wrappers nonbanked définis dans main.c (ROM0) :
extern void nb_reload_water_tiles_b2(void);
extern void nb_reload_port_tiles_b2(void);
extern void nb_update_hud_b2(void);
extern void nb_play_sound_build_b2(void);
extern void nb_tram_init_b2(void);
extern void nb_play_sound_error_b2(void);
extern void nb_perform_save_b2(void);
extern void nb_enable_sram_b2(void);
extern void nb_disable_sram_b2(void);
extern void nb_switch_ram_bank_b2(uint8_t bank);
extern void nb_init_game_variables_b2(void);
extern void nb_rebuild_registry_b2(void);
extern void nb_setup_story_params_b2(uint8_t mission_id, uint8_t text_y);

void main_menu() {

    uint8_t selection = 0;
    uint8_t blink_counter = 0;
    uint8_t menu_running = 1;
    NR52_REG = 0x80; NR51_REG = 0xFF; NR50_REG = 0x77;
    clear_entire_window();
    move_win(7, 0);
    load_lowercase_font();
    load_menu_tiles();
    nb_draw_menu_border();
    draw_presidente_bkg(1);

    while(menu_running) {

        if (game.language == LANG_EN) {

            draw_text(5, 10, GET_TEXT(TXT_STORY), 1);
            draw_text(5, 12, GET_TEXT(TXT_FREEPLAY), 1);
            SHOW_WIN; SHOW_SPRITES; DISPLAY_ON;

            if (++blink_counter >= 30) blink_counter = 0;
            if (blink_counter < 15) move_sprite(0, 32, (selection == 0) ? 96 : 112);
            else move_sprite(0, 0, 0);
        } else {

            draw_text(4, 10, GET_TEXT(TXT_STORY), 1);
            draw_text(4, 12, GET_TEXT(TXT_FREEPLAY), 1);
            SHOW_WIN; SHOW_SPRITES; DISPLAY_ON;

            if (++blink_counter >= 30) blink_counter = 0;
            if (blink_counter < 15) move_sprite(0, 24, (selection == 0) ? 96 : 112);
            else move_sprite(0, 0, 0);
        }

        uint8_t joy = joypad();
        if (joy & J_DOWN && selection < 1) { selection++; waitpadup(); }
        if (joy & J_UP   && selection > 0) { selection--; waitpadup(); }

        if (joy & (J_START | J_A)) {

            waitpadup();

            if (selection == 0) {
                // --- MODE STORY ---
                if (story_sub_menu() == 1) {
                    menu_running = 0;
                    break;
                } else {
                    clear_entire_window();
                    nb_draw_menu_border();
                    draw_presidente_bkg(1);
                    move_sprite(0, 0, 0);
                }
            }

            if (selection == 1) {
                // --- MODE FREE PLAY ---
                if (load_saves_screen() == 1) {
                    menu_running = 0;
                    break;
                }
                clear_entire_window();
                load_lowercase_font();
                nb_draw_menu_border();
                draw_presidente_bkg(1);
            }
        }
        wait_vbl_done();
    }
    // Nettoyage final avant de rendre la main au main()
    clear_entire_window();   // Vide la Window avant de restaurer les tiles (évite flash de tiles map)
    restore_shop_tiles();
    restore_map_tiles();
    move_sprite(0, 0, 0);
}

// stats_screen() est en bank 4 (stats_screen.c) — appelée via nb_stats_screen_b4()
extern void nb_stats_screen_b4(void);


// decrets_screen() est en bank 4 (decrets.c) — appelée via nb_decrets_screen_b4()
extern void nb_decrets_screen_b4(void);

void pause_menu() {

    uint8_t selection = 0;
    uint8_t menu_running = 1;
    uint8_t blink_counter = 0;
    uint8_t curs_x_pos, curs_y_pos;
    
    waitpadup(); // Sécurité pour ne pas valider instantanément

    // On remonte la Window pour couvrir l'écran
    move_win(7, 0); 
    
    // Nettoyage de l'écran de pause (tuiles vides)
    uint8_t blank_row[20] = {0};
    for(uint8_t i=0; i<18; i++) set_win_tiles(0, i, 20, 1, blank_row);

    // On cache les sprites 0 s'il sont utilisés
    move_sprite(1, 0, 0);     // Haut-Gauche
    move_sprite(2, 0, 0);     // Haut-Droite
    move_sprite(3, 0, 0); // Bas-Gauche
    move_sprite(4, 0, 0); // Bas-Droite
    load_lowercase_font();
    load_menu_tiles();
    nb_draw_menu_border();
    draw_presidente_bkg(1);

    while(menu_running) {

        if (game.language == LANG_EN) {

            draw_text(5, 5, GET_TEXT(TXT_STATS_ALMANACH), 1);
            draw_text(5, 7, "Decrees", 1);
            draw_text(5, 9, "Minimap", 1);
            draw_text(5, 11, GET_TEXT(TXT_SAVE), 1);
            draw_text(5, 13, GET_TEXT(TXT_RESUME), 1);
            draw_text(5, 15, GET_TEXT(TXT_TO_MAIN_MENU), 1);

            // --- GESTION DU CURSEUR (SPRITES) ---
            curs_y_pos = 56 + (selection * (8*2));
            curs_x_pos = 32;
        }else {

            draw_text(4, 5, GET_TEXT(TXT_STATS_ALMANACH), 1);
            draw_text(4, 7, "D~crets", 1);
            draw_text(4, 9, "Minimap", 1);
            draw_text(4, 11, GET_TEXT(TXT_SAVE), 1);
            draw_text(4, 13, GET_TEXT(TXT_RESUME), 1);
            draw_text(4, 15, GET_TEXT(TXT_TO_MAIN_MENU), 1);

            // --- GESTION DU CURSEUR (SPRITES) ---
            curs_y_pos = 56 + (selection * (8*2));
            curs_x_pos = 24;
        }

        if (++blink_counter >= 30) blink_counter = 0;
        if (blink_counter < 15) move_sprite(0, curs_x_pos, curs_y_pos);
        else move_sprite(0, 0, 0);

        uint8_t joy = joypad();

                // À insérer dans la boucle while(menu_running) de pause_menu
if (joy & J_B) {
    menu_running = 0; // On casse la boucle pour sortir de la fonction
    waitpadup();
}

        if (joy & J_DOWN && selection < 5) {
            selection++;
            waitpadup();
        }
        if (joy & J_UP && selection > 0) {
            selection--;
            waitpadup();
        }

        if (joy & J_A || joy & J_START) {
            if (selection == 0) {           // ALMANACH
                nb_stats_screen_b4();
                clear_entire_window();
                load_lowercase_font();
                nb_draw_menu_border();
                draw_presidente_bkg(1);
                waitpadup();
            }
            else if (selection == 1) {      // DECRETS
                nb_decrets_screen_b4();
                clear_entire_window();
                nb_draw_menu_border();
                draw_presidente_bkg(1);
                waitpadup();
            }
            else if (selection == 2) {      // MINIMAP
                restore_shop_tiles();
                show_minimap();
                nb_reload_water_tiles_b2();
                nb_reload_port_tiles_b2();
                load_lowercase_font();
                nb_draw_menu_border();
                draw_presidente_bkg(1);
                waitpadup();
            }
            else if (selection == 3) {      // SAUVEGARDER
                nb_perform_save_b2();
                uint8_t blank_save[20] = {0};
                /*set_win_tiles(1,  5, 18, 1, blank_save);
                set_win_tiles(1,  7, 18, 1, blank_save);
                set_win_tiles(1,  9, 18, 1, blank_save);
                set_win_tiles(1, 11, 18, 1, blank_save);
                set_win_tiles(1, 13, 19, 1, blank_save);
                set_win_tiles(1, 15, 19, 1, blank_save);*/
                move_sprite(0, 0, 0);
                for (uint8_t row = 4; row <= 16; row++) {
                    set_win_tiles(1, row, 18, 1, blank_save);
                }

                if (game.language == LANG_EN) {
                    draw_text(2, 8, "Save in progress", 1);
                } else {
                    draw_text(4, 8, "Sauvegarde", 1);
                    draw_text(4, 9, "en cours", 1);
                }
                delay(1000);
                set_win_tiles(1,8, 18, 1, blank_save);
                set_win_tiles(1, 9, 18, 1, blank_save);
                move_sprite(0, curs_x_pos, curs_y_pos);
                if (game.language == LANG_EN) {
                    draw_text(5, 5,  GET_TEXT(TXT_STATS_ALMANACH), 1);
                    draw_text(5, 7,  "Decrees", 1);
                    draw_text(5, 9,  "Minimap", 1);
                    draw_text(5, 11, GET_TEXT(TXT_SAVE), 1);
                    draw_text(5, 13, GET_TEXT(TXT_RESUME), 1);
                    draw_text(5, 15, GET_TEXT(TXT_TO_MAIN_MENU), 1);
                } else {
                    draw_text(4, 5,  GET_TEXT(TXT_STATS_ALMANACH), 1);
                    draw_text(4, 7,  "D~crets", 1);
                    draw_text(4, 9,  "Minimap", 1);
                    draw_text(4, 11, GET_TEXT(TXT_SAVE), 1);
                    draw_text(4, 13, GET_TEXT(TXT_RESUME), 1);
                    draw_text(4, 15, GET_TEXT(TXT_TO_MAIN_MENU), 1);
                }
            }
            else if (selection == 4) menu_running = 0; // RESUME
            else if (selection == 5) {      // MENU PRINCIPAL
                force_return_to_menu = 1;
                menu_running = 0;
                clear_entire_window();
                waitpadup();
            }
        }
        wait_vbl_done();
    }

    // --- SORTIE DU MENU ---
    waitpadup();

    // On cache les sprites du menu pour qu'ils ne restent pas figés à l'écran
    move_sprite(0, 0, 0);
    for(uint8_t i=1; i<5; i++) move_sprite(i, 0, 0);

    restore_shop_tiles();
    restore_map_tiles();
    clear_entire_window();
    if (force_return_to_menu) {
        fade_out(); // Fader pendant que la fenêtre couvre encore la map
    } else {
        move_win(7, (current_tool == TOOL_NONE) ? 144 : 136);
        nb_update_hud_b2();
    }
}

uint8_t confirm_story_overwrite(void) {
    uint8_t sub_selection = 1; // Par défaut sur "NO"
    clear_entire_window();
    
    if (game.language == LANG_EN) {
        draw_text(1, 5, "Story in progress", 1);
        draw_text(1, 7, "Overwrite save ?", 1);
    } else {
        draw_text(2, 5, "Partie existante", 1);
        draw_text(2, 7, "Ecraser ?", 1);
    }
    
    while(1) {
        if (game.language == LANG_EN) {
            draw_text(4, 11, sub_selection == 0 ? ">Yes" : " Yes", 1);
            draw_text(11, 11, sub_selection == 1 ? ">No " : " No ", 1);
        } else {
            draw_text(4, 11, sub_selection == 0 ? ">Oui" : " Oui", 1);
            draw_text(11, 11, sub_selection == 1 ? ">Non " : " Non ", 1);
        }
        uint8_t joy = joypad();
        if (joy & J_LEFT)  { sub_selection = 0; nb_play_sound_build_b2(); waitpadup(); }
        if (joy & J_RIGHT) { sub_selection = 1; nb_play_sound_build_b2(); waitpadup(); }

        if (joy & (J_A | J_START)) {
            waitpadup();
            clear_entire_window();
            return (sub_selection == 0); 
        }
        if (joy & J_B) {
            waitpadup();
            clear_entire_window();
            return 0;
        }
        wait_vbl_done();
    }
}


uint8_t story_sub_menu() {
    uint8_t selection = 0;
    uint8_t menu_running = 1;
    uint8_t blink_counter = 0;

    // On fixe le slot story sur la banque 3
    current_save_slot = 3;
    
    // Initialisation de l'affichage
    move_win(7, 0); // On s'assure que la fenêtre est en haut
    clear_entire_window();
    nb_draw_menu_border();
    draw_presidente_bkg(1);

    if (game.language == LANG_EN) {

        draw_text(5, 10, GET_TEXT(TXT_NEW_GAME), 1);
        draw_text(5, 12, GET_TEXT(TXT_CONTINUE), 1);
        draw_text(5, 15, GET_TEXT(TXT_BACK), 1);
    } else {

        draw_text(3, 10, GET_TEXT(TXT_NEW_GAME), 1);
        draw_text(3, 12, GET_TEXT(TXT_CONTINUE), 1);
        draw_text(3, 15, GET_TEXT(TXT_BACK), 1);
    }

    waitpadup();

    while(menu_running) {
        uint8_t joy = joypad();

        if (++blink_counter >= 30) blink_counter = 0;
        uint8_t curs_y_pos = ((10 + (selection * 2)) * 8) + 16;
        if (game.language == LANG_EN) {
            if (blink_counter < 15) {
                move_sprite(0, 32, curs_y_pos);
            } else {
                move_sprite(0, 0, 0);
            }
        } else {
            if (blink_counter < 15) {
                move_sprite(0, 16, curs_y_pos);
            } else {
                move_sprite(0, 0, 0);
            }
        }

        // 1. Navigation
        if (joy & J_DOWN && selection < 1) {
            selection++;
            nb_play_sound_build_b2();    
            waitpadup();
        }
        if (joy & J_UP && selection > 0) {
            selection--;
            nb_play_sound_build_b2(); 
            waitpadup();
        }

        // 3. Validation (Bouton A)
        if (joy & (J_A | J_START)) {
            waitpadup();
            move_sprite(0, 0, 0); // Nettoyage curseur

            /*if (selection == 0) { // NEW GAME
                current_step = 0;
                setup_story_params(0); 
                return 1; 
            }
            else if (selection == 1) { // CONTINUE
                nb_enable_sram_b2();
                nb_switch_ram_bank_b2(3);
                GameState *sram_ptr = (GameState *)(0xA000 + 4096);
                
                if (sram_ptr->signature == 99) {
                    nb_disable_sram_b2();
                    load_story_game(3);
                    return 1;
                } else {
                    nb_disable_sram_b2();
                    nb_play_sound_error_b2();
                    if (game.language == LANG_EN) {
                        draw_text(5, 15, GET_TEXT(TXT_NO_SAVE), 1);
                    } else {
                        draw_text(2, 15, GET_TEXT(TXT_NO_SAVE), 1); 
                    }
                    delay(1000);
                    draw_text(1, 15, "                             ", 1);
                }
            }*/

            if (selection == 0) { // --- NEW GAME ---
        
            // 1. VERIFICATION SI UNE SAUVEGARDE EXISTE DEJA
            uint8_t already_exists = 0;
            nb_enable_sram_b2();
            nb_switch_ram_bank_b2(3); // Le slot Story est toujours en banque 3
            GameState *sram_ptr = (GameState *)(0xA000 + 4096);
            if (sram_ptr->signature == 99) {
                already_exists = 1;
            }
            nb_disable_sram_b2();

            // 2. SI ELLE EXISTE, ON DEMANDE CONFIRMATION
            if (already_exists) {
                if (confirm_story_overwrite() == 0) {
                    // L'utilisateur a choisi NO : on redessine le menu Story et on reste là
                    clear_entire_window();
                    nb_draw_menu_border();
                    draw_presidente_bkg(1);
                    if (game.language == LANG_EN) {
                        draw_text(5, 10, GET_TEXT(TXT_NEW_GAME), 1);
                        draw_text(5, 12, GET_TEXT(TXT_CONTINUE), 1);
                        draw_text(5, 15, GET_TEXT(TXT_BACK), 1);
                    } else {
                        draw_text(3, 10, GET_TEXT(TXT_NEW_GAME), 1);
                        draw_text(3, 12, GET_TEXT(TXT_CONTINUE), 1);
                        draw_text(3, 15, GET_TEXT(TXT_BACK), 1);
                    }
                    // On continue la boucle while(menu_running)
                } else {
                    // L'utilisateur a dit YES : effacer la signature SRAM, puis init map
                    nb_enable_sram_b2();
                    nb_switch_ram_bank_b2(3);
                    ((GameState *)(0xA000 + 4096))->signature = 0;
                    nb_disable_sram_b2();
                    load_story_game(3);
                    current_step = 0;
                    move_win(7, 0);
                    nb_setup_story_params_b2(0, 5);
                    restore_shop_tiles();
                    return 1;
                }
            } else {
                // Pas de sauvegarde existante : init map directement
                load_story_game(3);
                current_step = 0;
                nb_setup_story_params_b2(0, 5);
                restore_shop_tiles();
                return 1;
            }
            } else if (selection == 1) { // CONTINUE
                nb_enable_sram_b2();
                nb_switch_ram_bank_b2(3);
                GameState *sram_ptr = (GameState *)(0xA000 + 4096);

                if (sram_ptr->signature == 99) {
                    nb_disable_sram_b2();
                    load_story_game(3);
                    restore_shop_tiles();
                    return 1;
                } else {
                    nb_disable_sram_b2();
                    nb_play_sound_error_b2();
                    if (game.language == LANG_EN) {
                        draw_text(5, 15, GET_TEXT(TXT_NO_SAVE), 1);
                    } else {
                        draw_text(2, 15, GET_TEXT(TXT_NO_SAVE), 1); 
                    }
                    delay(1000);
                    draw_text(1, 15, "                  ", 1);
                }
            }
        }
        // 4. Retour (Bouton B)
        if (joy & J_B) {
            nb_play_sound_build_b2();
            waitpadup();
            move_sprite(0, 0, 0);
            clear_entire_window();
            return 0;
        }

        wait_vbl_done(); // Un seul appel ici pour stabiliser le clignotement
    }
}

void language_selection_screen(void) {
    uint8_t sel = 0;
    clear_entire_window();
    move_win(7, 0);
    SHOW_WIN;
    load_lowercase_font();
    load_menu_tiles();
    nb_draw_menu_border();

    draw_presidente_bkg(1);
    draw_text(2, 7, "Select language", 1);
    draw_text(6, 10, "French", 1);
    draw_text(6, 12, "English", 1);

    uint8_t blink_counter = 0;
    while(1) {
        uint8_t joy = joypad();
        if (joy & J_UP)   { sel = 0; waitpadup(); }
        if (joy & J_DOWN) { sel = 1; waitpadup(); }

        // Curseur clignotant
        if (++blink_counter >= 30) blink_counter = 0;
        if (blink_counter < 15) move_sprite(0, 40, (sel == 0 ? 96 : 112));
        else move_sprite(0, 0, 0);

        if (joy & J_A) {
            current_lang = (sel == 0 ? LANG_FR : LANG_EN);
            game.language = current_lang;
            nb_perform_save_b2();
            waitpadup();
            break;
        }
        wait_vbl_done();
    }
    fade_out();
    move_sprite(0, 0, 0);
    clear_entire_window();
    restore_shop_tiles();
    restore_map_tiles();
}