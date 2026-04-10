#pragma bank 4
#include <gb/gb.h>
#include "constants.h"
#include "graphics.h"
#include "game_state.h"

// Wrappers nonbanked définis dans main.c
extern void nb_draw_menu_border(void);
extern void nb_tram_init_safe(void);

void decrets_screen(void) {
    uint8_t decree_cursor = 0;
    uint8_t running = 1;
    uint8_t blink_counter = 0;
    move_sprite(0, 0, 0);
    waitpadup();
    clear_entire_window();
    nb_draw_menu_border();
    if (game.language == LANG_EN) {
        draw_text(5, 2, "Decrees", 1);
        draw_text(2, 4, "Food for all", 1);
        draw_text(1, 5, game.decree_food_for_all ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
        draw_text(2, 8, "Free public", 1);
        draw_text(2, 9, "transport", 1);
        draw_text(1, 10, game.decree_tram ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
        draw_text(1, 14, "Up/down: select", 1);
        draw_text(1, 15, "A: activate/deact.", 1);
        draw_text(1, 16, "B: back", 1);
    } else {
        draw_text(5, 2, "D~crets", 1);
        draw_text(2, 4, "Nourriture", 1);
        draw_text(2, 5, "pour tous", 1);
        draw_text(1, 6, game.decree_food_for_all ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
        draw_text(2, 8, "Transports", 1);
        draw_text(2, 9, "gratuits", 1);
        draw_text(1, 10, game.decree_tram ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
        draw_text(1, 14, "Haut/bas: choisir", 1);
        draw_text(1, 15, "A: activer/desact.", 1);
        draw_text(1, 16, "B: retour", 1);
    }
    while (running) {
        // Curseur clignotant (sprite 0 = fleche, chargé par draw_presidente_bkg dans pause_menu)
        if (++blink_counter >= 30) blink_counter = 0;
        uint8_t curs_y = (decree_cursor == 0) ? 48 : 80;
        if (blink_counter < 15) move_sprite(0, 16, curs_y);
        else move_sprite(0, 0, 0);

        uint8_t joy = joypad();
        if (joy & J_B) { running = 0; waitpadup(); }
        if ((joy & J_UP) && decree_cursor != 0)   { decree_cursor = 0; waitpadup(); }
        if ((joy & J_DOWN) && decree_cursor != 1) { decree_cursor = 1; waitpadup(); }
        if (joy & J_A) {
            if (decree_cursor == 0) {
                game.decree_food_for_all = !game.decree_food_for_all;
                if (game.language == LANG_EN) {
                    draw_text(1, 5, game.decree_food_for_all ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
                } else {
                    draw_text(1, 6, game.decree_food_for_all ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
                }
            } else {
                game.decree_tram = !game.decree_tram;
                if (game.decree_tram) nb_tram_init_safe();
                draw_text(1, 10, game.decree_tram ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
            }
            waitpadup();
        }
        wait_vbl_done();
    }
}
