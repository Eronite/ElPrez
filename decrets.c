#pragma bank 4
#include <gb/gb.h>
#include "constants.h"
#include "graphics.h"
#include "game_state.h"

// Wrappers nonbanked définis dans main.c
extern void nb_draw_menu_border(void);
extern void nb_tram_init_safe(void);

static void clear_content(void) {
    uint8_t blank[18] = {0};
    uint8_t ri;
    for (ri = 1u; ri < 16u; ri++) set_win_tiles(1, ri, 18, 1, blank);
}

static void draw_page0(void) {
    clear_content();
    if (game.language == LANG_EN) {
        draw_text(5, 2, "Decrees 1/3", 1);
        draw_text(2, 4, "Food for all", 1);
        draw_text(1, 5, game.decree_food_for_all ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
        draw_text(1, 7, "Feeds citizens who", 1);
        draw_text(1, 8, "lack food", 1);
        draw_text(1, 14, "A: activate/deact.", 1);
    } else {
        draw_text(4, 2, "D~crets 1/3", 1);
        draw_text(2, 4, "Nourriture", 1);
        draw_text(2, 5, "pour tous", 1);
        draw_text(1, 6, game.decree_food_for_all ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
        draw_text(1, 8, "Alimente les", 1);
        draw_text(1, 9, "habitants sans", 1);
        draw_text(1, 10, "nourriture", 1);
        draw_text(1, 15, "A: activer/desact.", 1);
    }
}

static void draw_page1(void) {
    clear_content();
    if (game.language == LANG_EN) {
        draw_text(5, 2, "Decrees 2/3", 1);
        draw_text(2, 4, "Free public", 1);
        draw_text(2, 5, "transport", 1);
        draw_text(1, 6, game.decree_tram ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
        draw_text(1, 8, "Improves citizen", 1);
        draw_text(1, 9, "happiness", 1);
        draw_text(1, 14, "A: activate/deact.", 1);
    } else {
        draw_text(4, 2, "D~crets 2/3", 1);
        draw_text(2, 4, "Transports", 1);
        draw_text(2, 5, "gratuits", 1);
        draw_text(1, 6, game.decree_tram ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
        draw_text(1, 8, "Am~liore le", 1);
        draw_text(1, 9, "bonheur des", 1);
        draw_text(1, 10, "habitants", 1);
        draw_text(1, 15, "A: activer/desact.", 1);
    }
}

static void draw_page2(void) {
    clear_content();
    if (game.language == LANG_EN) {
        draw_text(5, 2, "Decrees 3/3", 1);
        draw_text(2, 4, "Housing for all", 1);
        draw_text(1, 5, game.decree_housing ? "  [ON]  -1500/mois" : "  [OFF] -1500/mois", 1);
        draw_text(1, 7, "Reduces homeless", 1);
        draw_text(1, 8, "each month active", 1);
        draw_text(1, 14, "A: activate/deact.", 1);
    } else {
        draw_text(4, 2, "D~crets 3/3", 1);
        draw_text(2, 4, "Logement", 1);
        draw_text(2, 5, "pour tous", 1);
        draw_text(1, 6, game.decree_housing ? "  [ON]  -1500/mois" : "  [OFF] -1500/mois", 1);
        draw_text(1, 8, "R~duit sans-abris", 1);
        draw_text(1, 9, "chaque mois actif", 1);
        draw_text(1, 15, "A: activer/desact.", 1);
    }
}

void decrets_screen(void) {
    uint8_t decree_page = 0;
    uint8_t running = 1;
    waitpadup();
    move_sprite(0, 0, 0);
    clear_entire_window();
    nb_draw_menu_border();
    if (game.language == LANG_EN) {
        draw_text(1, 16, "Left/Right: page B:back", 1);
    } else {
        draw_text(1, 16, "G/D:page  B:retour", 1);
    }
    draw_page0();

    while (running) {
        uint8_t joy = joypad();

        if (joy & J_B) { running = 0; waitpadup(); }

        if (joy & J_RIGHT) {
            decree_page++;
            if (decree_page > 2) decree_page = 0;
            if (decree_page == 0) draw_page0();
            else if (decree_page == 1) draw_page1();
            else draw_page2();
            waitpadup();
        }
        if (joy & J_LEFT) {
            if (decree_page == 0) decree_page = 2;
            else decree_page--;
            if (decree_page == 0) draw_page0();
            else if (decree_page == 1) draw_page1();
            else draw_page2();
            waitpadup();
        }

        if (joy & J_A) {
            if (decree_page == 0) {
                game.decree_food_for_all = !game.decree_food_for_all;
                if (game.language == LANG_EN) {
                    draw_text(1, 5, game.decree_food_for_all ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
                } else {
                    draw_text(1, 6, game.decree_food_for_all ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
                }
            } else if (decree_page == 1) {
                game.decree_tram = !game.decree_tram;
                if (game.decree_tram) nb_tram_init_safe();
                draw_text(1, 6, game.decree_tram ? "  [ON]  -1000/mois" : "  [OFF] -1000/mois", 1);
            } else {
                game.decree_housing = !game.decree_housing;
                if (game.language == LANG_EN) {
                    draw_text(1, 5, game.decree_housing ? "  [ON]  -1500/mois" : "  [OFF] -1500/mois", 1);
                } else {
                    draw_text(1, 6, game.decree_housing ? "  [ON]  -1500/mois" : "  [OFF] -1500/mois", 1);
                }
            }
            waitpadup();
        }

        wait_vbl_done();
    }
}
