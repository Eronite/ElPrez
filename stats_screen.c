#pragma bank 4
#include <gb/gb.h>
#include "constants.h"
#include "graphics.h"
#include "game_state.h"
#include "missions.h"
#include "lang.h"

extern void nb_play_sound_build_b4(void);
extern void nb_story_get_current_step_b2(MissionStep *out);

void stats_screen() {
uint8_t stats_page = 0;
uint8_t menu_running = 1;
uint8_t redraw = 1;
uint8_t joy;
waitpadup();
move_win(7, 0); // On remonte la fenêtre (Window) pour couvrir l'écran

uint8_t nb_of_houses = 0, nb_of_farms = 0;
{ uint8_t ri; for (ri = 0; ri < building_count; ri++) {
    if (building_registry[ri].type == TILE_HOUSE_NW) nb_of_houses++;
    else if (building_registry[ri].type == TILE_FARM_NW) nb_of_farms++;
} }

clear_entire_window();
nb_draw_menu_border();
draw_text(1, 16, GET_TEXT(TXT_PAGE_NAV), 1);
while (menu_running) {
    if (redraw == 1) {
        // Effacer uniquement la zone de contenu intérieure (cols 1-18, rows 1-15)
        { uint8_t blank[18] = {0}; uint8_t ri; for (ri = 1u; ri < 16u; ri++) set_win_tiles(1, ri, 18, 1, blank); }
        move_sprite(0, 0, 0); // On cache le curseur pendant l'affichage
        // Effacer les sprites de coche (slots 1-7) à chaque changement de page
        { uint8_t si; for (si = 1u; si <= 7u; si++) move_sprite(si, 0u, 0u); }

        if (stats_page == 0) { // --- PAGE 1 : GENERAL ---
            if (game.language == LANG_EN) {
                draw_text(6, 1, "General", 1);
                draw_text(1, 5, GET_TEXT(TXT_STATS_DATE), 1);
                { uint8_t mo = game.month; char mm[3]; mm[0]='0'+mo/10; mm[1]='0'+mo%10; mm[2]='\0'; draw_text(12, 5, mm, 1); }
                draw_text(14, 5, "/", 1);
                draw_number(15, 5, game.year, 1);
                draw_text(1, 7, GET_TEXT(TXT_STATS_POPULATION), 1);
                draw_number(12, 7, game.population, 1);
                            draw_text(1, 9, "Happiness :", 1);
                draw_number(12, 9, game.avg_happiness, 1);
                draw_text(1, 11, "Money :", 1);
                draw_number(12, 11, (game.money > 999999L ? 999999L : game.money), 1);
            } else {
            //draw_text(6, 1, GET_TEXT(TXT_STATS_ALMANACH), 1);
                draw_text(6, 1, GET_TEXT(TXT_STATS_GENERAL), 1);
                draw_text(1, 5, GET_TEXT(TXT_STATS_DATE), 1);
                { uint8_t mo = game.month; char mm[3]; mm[0]='0'+mo/10; mm[1]='0'+mo%10; mm[2]='\0'; draw_text(12, 5, mm, 1); }
                draw_text(14, 5, "/", 1);
                draw_number(15, 5, game.year, 1);
                draw_text(1, 7, GET_TEXT(TXT_STATS_POPULATION), 1);
                draw_number(12, 7, game.population, 1);
                            draw_text(1, 9, GET_TEXT(TXT_STATS_HAPPINESS), 1);
                draw_number(12, 9, game.avg_happiness, 1);
                draw_text(1, 11, GET_TEXT(TXT_STATS_MONEY), 1);
                draw_number(12, 11, (game.money > 999999L ? 999999L : game.money), 1);
            }
        }
        else if (stats_page == 1) { // --- PAGE 2 : SOCIAL ---
            //draw_text(6, 1, GET_TEXT(TXT_STATS_ALMANACH), 1);
            if (game.language == LANG_EN) {
            draw_text(6, 1, GET_TEXT(TXT_STATS_SOCIAL), 1);
            draw_text(1, 5, "Homeless :", 1);
            draw_number(15, 5, game.homeless, 1);

            draw_text(1, 7, "Crime rate :", 1);
            draw_number(15, 7, game.crime_rate, 1);

            draw_text(1, 9, "Unemployment :", 1);
            draw_number(15, 9, game.unemployment_rate, 1);
            } else {
                draw_text(6, 1, GET_TEXT(TXT_STATS_SOCIAL), 1);
                draw_text(1, 5, "Sans abris :", 1);
                draw_number(15, 5, game.homeless, 1);

                draw_text(1, 7, "Taux crime :", 1);
                draw_number(15, 7, game.crime_rate, 1);

                draw_text(1, 9, "Taux chomage :", 1);
                draw_number(15, 9, game.unemployment_rate, 1);
            }
            //draw_text(1, 7, GET_TEXT(TXT_STATS_HOUSING_CAPACITY), 1);
            //draw_number(14, 8, game.housing_capacity, 1);
            //draw_text(1, 9, GET_TEXT(TXT_STATS_HAPPINESS), 1);
            //draw_number(12, 9, game.avg_happiness, 1);
            //draw_text(15, 9, "%", 1);
            //draw_text(1, 11, "food prod :", 1);
            //draw_number(12, 11, game.foodProduction, 1);
                        //draw_text(1, 12, "food conso :", 1);
            //draw_number(12, 12, game.foodConsumption, 1);
                        //draw_text(1, 13, "food stock :", 1);
            //draw_number(12, 13, game.foodStock, 1);
        }
        else if (stats_page == 2) { // --- PAGE 3 : ECONOMIE ---
            draw_text(6, 1, GET_TEXT(TXT_STATS_ALMANACH), 1);
            draw_text(6, 2, GET_TEXT(TXT_STATS_ECONOMY), 1);
            draw_text(1, 5, GET_TEXT(TXT_STATS_MONTHLY_EXPENSE), 1);
            draw_number(15, 5, (uint16_t)game.monthly_expenses, 1);
            draw_text(1, 7, GET_TEXT(TXT_STATS_MONTHLY_PROFIT), 1);
            draw_number(15, 7, game.last_month_profit, 1);

            draw_text(1, 8, "new :", 1);
            draw_number(15, 8, (uint16_t)game.monthly_revenue, 1);

            draw_text(1, 9, "total jobs :", 1);
            draw_number(15, 9, game.total_jobs, 1);
            draw_text(1, 10, "workers :", 1);
            draw_number(15, 10, (uint16_t)((uint16_t)((uint32_t)game.population * 70 / 100) - game.total_unemployed), 1);
            draw_text(1, 11, "total unemployed :", 1);
            draw_number(15, 12, game.total_unemployed, 1);
                        draw_text(1, 13, "chomage pctg :", 1);
            draw_number(12, 14, game.unemployment_rate, 1);
        }
        else if (stats_page == 3) { // --- PAGE 4 : MISSIONS ---
            restore_shop_tiles(); // recharge cursor_data en tile sprite 0 (écrasée par fleche)
            draw_text(6, 1, GET_TEXT(TXT_STATS_MISSION), 1);
            if (game.game_mode == MODE_STORY && game.mission_id >= 4) {
                draw_text(3, 4, "Toutes les", 1);
                draw_text(3, 6, "missions", 1);
                draw_text(3, 8, "accomplies!", 1);
            } else if (game.game_mode == MODE_STORY) {
                MissionStep cur;
                nb_story_get_current_step_b2(&cur);
                draw_text(3, 2, GET_TEXT(TXT_STATS_CURRENT_GOAL), 1);
                //draw_text(1, 4, GET_MISSION_TEXT(cur.goal_idx), 1);
                uint8_t row = 7;
                uint8_t chk_spr = 1u; // prochain slot sprite pour coche
                if (cur.target_money > 0) {
                    if (game.money >= cur.target_money) {
                    set_sprite_tile(chk_spr, TILE_HUD_TICK);
                    } else {
                        set_sprite_tile(chk_spr, 0); // 0 pour le curseur
                    }
                    set_sprite_prop(chk_spr, 0x00u);
                    move_sprite(chk_spr, 16u, (uint8_t)(row * 8u + 16u));
                    chk_spr++;
                    draw_text(3, row, "Argent:", 1);
                    draw_number(13, row, cur.target_money, 1);
                    row += 2;
                }
                if (cur.target_pop > 0) {
                    if (game.population >= cur.target_pop) {
                        set_sprite_tile(chk_spr, TILE_HUD_TICK);
                    } else {
                        set_sprite_tile(chk_spr, 0); // 0 pour le curseur
                    }
                    set_sprite_prop(chk_spr, 0x00u);
                    move_sprite(chk_spr, 16u, (uint8_t)(row * 8u + 16u));
                    chk_spr++;
                    draw_text(3, row, "Pop:", 1);
                    draw_number(13, row, cur.target_pop, 1);
                    row += 2;
                }
                if (cur.target_food_stock > 0) {
                    if (game.foodStock >= cur.target_food_stock) {
                        set_sprite_tile(chk_spr, TILE_HUD_TICK);
                    } else {
                        set_sprite_tile(chk_spr, 0); // 0 pour le curseur
                    }
                    set_sprite_prop(chk_spr, 0x00u);
                    move_sprite(chk_spr, 16u, (uint8_t)(row * 8u + 16u));
                    chk_spr++;
                    draw_text(3, row, "R~serves:", 1);
                    draw_number(14, row, cur.target_food_stock, 1);
                    row += 2;
                }
                if (cur.target_food_prod > 0) {
                    if (game.foodProduction >= cur.target_food_prod) {
                        set_sprite_tile(chk_spr, TILE_HUD_TICK);
                    } else {
                        set_sprite_tile(chk_spr, 0); // 0 pour le curseur
                    }
                    set_sprite_prop(chk_spr, 0x00u);
                    move_sprite(chk_spr, 16u, (uint8_t)(row * 8u + 16u));
                    chk_spr++;
                    draw_text(3, row, "Prod.food:", 1);
                    draw_number(15, row, cur.target_food_prod, 1);
                    row += 2;
                }
                if (cur.target_happiness > 0) {
                    if (game.avg_happiness >= cur.target_happiness) {
                        set_sprite_tile(chk_spr, TILE_HUD_TICK);
                    } else {
                        set_sprite_tile(chk_spr, 0); // 0 pour le curseur
                    }
                    set_sprite_prop(chk_spr, 0x00u);
                    move_sprite(chk_spr, 16u, (uint8_t)(row * 8u + 16u));
                    chk_spr++;
                    draw_text(3, row, "Bonheur:", 1);
                    draw_number(14, row, cur.target_happiness, 1);
                    row += 2;
                }
                if (cur.target_ore > 0) {
                    if (game.ore_stock >= cur.target_ore) {
                        set_sprite_tile(chk_spr, TILE_HUD_TICK);
                    } else {
                        set_sprite_tile(chk_spr, 0); // 0 pour le curseur
                    }
                    move_sprite(chk_spr, 16u, (uint8_t)(row * 8u + 16u));
                    chk_spr++;
                    draw_text(3, row, "Minerai:", 1);
                    draw_number(14, row, cur.target_ore, 1);
                    row += 2;
                }
                if (cur.target_culture > 0) {
                    if (game.culture_stock >= cur.target_culture) {
                        set_sprite_tile(chk_spr, TILE_HUD_TICK);
                    } else {
                        set_sprite_tile(chk_spr, 0); // 0 pour le curseur
                    }
                    set_sprite_prop(chk_spr, 0x00u);
                    move_sprite(chk_spr, 16u, (uint8_t)(row * 8u + 16u));
                    chk_spr++;
                    draw_text(5, row, "Culture:", 1);
                    draw_number(14, row, cur.target_culture, 1);
                    row += 2;
                }
                if (cur.target_type != 0) {
                    uint8_t bldg_cnt = 0;
                    uint8_t ri;
                    for (ri = 0; ri < building_count; ri++)
                        if (building_registry[ri].type == cur.target_type) bldg_cnt++;
                    if (bldg_cnt >= cur.target_count) {
                        set_sprite_tile(chk_spr, TILE_HUD_TICK);
                    } else {
                        set_sprite_tile(chk_spr, 0); // 0 pour le curseur
                    }
                    set_sprite_prop(chk_spr, 0x00u);
                    move_sprite(chk_spr, 16u, (uint8_t)(row * 8u + 16u));
                    chk_spr++;
                    if      (cur.target_type == TILE_FARM_NW)       draw_text(3, row, "Fermes:", 1);
                    else if (cur.target_type == TILE_HOUSE_NW)      draw_text(3, row, "Maisons:", 1);
                    else if (cur.target_type == TILE_MINE_NW)       draw_text(3, row, "Mines:", 1);
                    else if (cur.target_type == TILE_PLANTATION_NW) draw_text(3, row, "Plantation:", 1);
                    else if (cur.target_type == TILE_CHURCH_NW)     draw_text(3, row, "Eglise:", 1);
                    else if (cur.target_type == TILE_SCHOOL_NW)     draw_text(3, row, "Ecole:", 1);
                    else if (cur.target_type == TILE_HOSPITAL_NW)   draw_text(3, row, "Hopital:", 1);
                    else if (cur.target_type == TILE_POLICE_NW)     draw_text(3, row, "Police:", 1);
                    else if (cur.target_type == TILE_POWER_NW)      draw_text(3, row, "Centrale:", 1);
                    else                                             draw_text(3, row, "Batiment:", 1);
                    draw_number(15, row, cur.target_count, 1);
                    row += 2;
                }
                if (cur.target_type2 != 0) {
                    uint8_t bldg_cnt = 0;
                    uint8_t ri;
                    for (ri = 0; ri < building_count; ri++)
                        if (building_registry[ri].type == cur.target_type2) bldg_cnt++;
                    if (bldg_cnt >= cur.target_count2) {
                        set_sprite_tile(chk_spr, TILE_HUD_TICK);
                    } else {
                        set_sprite_tile(chk_spr, 0);
                    }
                    set_sprite_prop(chk_spr, 0x00u);
                    move_sprite(chk_spr, 16u, (uint8_t)(row * 8u + 16u));
                    chk_spr++;
                    if      (cur.target_type2 == TILE_FARM_NW)       draw_text(3, row, "Fermes:", 1);
                    else if (cur.target_type2 == TILE_HOUSE_NW)      draw_text(3, row, "Maisons:", 1);
                    else if (cur.target_type2 == TILE_MINE_NW)       draw_text(3, row, "Mines:", 1);
                    else if (cur.target_type2 == TILE_PLANTATION_NW) draw_text(3, row, "Plantation:", 1);
                    else if (cur.target_type2 == TILE_CHURCH_NW)     draw_text(3, row, "Eglise:", 1);
                    else if (cur.target_type2 == TILE_SCHOOL_NW)     draw_text(3, row, "Ecole:", 1);
                    else if (cur.target_type2 == TILE_HOSPITAL_NW)   draw_text(3, row, "Hopital:", 1);
                    else if (cur.target_type2 == TILE_POLICE_NW)     draw_text(3, row, "Police:", 1);
                    else if (cur.target_type2 == TILE_POWER_NW)      draw_text(3, row, "Centrale:", 1);
                    else                                              draw_text(3, row, "Batiment:", 1);
                    draw_number(15, row, cur.target_count2, 1);
                    row += 2;
                }
            } else {
                draw_text(1, 6, GET_TEXT(TXT_FREEPLAY), 1);
            }
        }

        else if (stats_page == 4) { // --- PAGE 5 : NOMBRE DE BATIMENTS ---
            draw_text(6, 1, GET_TEXT(TXT_STATS_ALMANACH), 1);
            draw_text(6, 2, "nb batiments", 1);
            draw_text(1, 5, "nb de farm", 1);
            draw_number(15, 5, nb_of_farms, 1);
            draw_text(1, 7, "nb de house", 1);
            draw_number(15, 7, nb_of_houses, 1);
        }

        else if (stats_page == 5) { // --- PAGE 6 ---
            draw_text(6, 1, GET_TEXT(TXT_STATS_ALMANACH), 1);
            draw_text(6, 2, "~lectricit~", 1);
            draw_text(1, 5, "produite :", 1);
            draw_number(15, 5, game.electricity_prod, 1);
            draw_text(1, 7, "consomm~e :", 1);
            draw_number(15, 7, game.electricity_cons, 1);
        }

        else if (stats_page == 6) { // --- PAGE 7 : REVENUS ---
            draw_text(6, 1, GET_TEXT(TXT_STATS_ALMANACH), 1);
            draw_text(5, 2, "revenus", 1);
            draw_text(1, 4, "loyers :", 1);
            draw_number(15, 4, game.rev_rents, 1);
            draw_text(1, 6, "food :", 1);
            draw_number(15, 6, game.rev_food, 1);
            draw_text(1, 8, "minerai :", 1);
            draw_number(15, 8, game.rev_ore, 1);
            draw_text(1, 10, "culture :", 1);
            draw_number(15, 10, game.rev_culture, 1);
            draw_text(1, 12, "magasin :", 1);
            draw_number(15, 12, game.rev_mall, 1);
            draw_text(1, 14, "bar :", 1);
            draw_number(15, 14, game.rev_bar, 1);
        }

        else if (stats_page == 7) { // --- PAGE 8 : DEPENSES ---
            draw_text(6, 1, GET_TEXT(TXT_STATS_ALMANACH), 1);
            draw_text(5, 2, "d~penses", 1);
            draw_text(1, 5, "maintenance :", 1);
            draw_number(15, 5, game.exp_maintenance, 1);
            draw_text(1, 7, "salaires :", 1);
            draw_number(15, 7, game.exp_salaries, 1);
            draw_text(1, 9, "construction :", 1);
            draw_number(15, 9, game.exp_construction, 1);
            if (game.decree_tram) {
                draw_text(1, 11, "tram :", 1);
                draw_number(15, 11, 1000, 1);
                draw_text(1, 13, "total :", 1);
                draw_number(15, 13, (uint16_t)game.monthly_expenses, 1);
            } else {
                draw_text(1, 11, "total :", 1);
                draw_number(15, 11, (uint16_t)game.monthly_expenses, 1);
            }
        }

        else if (stats_page == 8) { // --- PAGE 9 : FACTEURS BONHEUR 1 ---
            draw_text(6, 1, "Facteurs bonheur", 1);
            draw_text(4, 2, "bonheur (1/2)", 1);
            // Batiments (hap_d stocké directement depuis economy.c)
            {
                int16_t bldg_hap = game.hap_buildings;
                draw_text(1, 4, "batiments :", 1);
                if (bldg_hap >= 0) { draw_text(13, 4, "+", 1); draw_number(14, 4, (uint16_t)bldg_hap, 1); }
                else               { draw_text(13, 4, "-", 1); draw_number(14, 4, (uint16_t)(-bldg_hap), 1); }
            }
            // Crime
            {
                int16_t v = -(int16_t)(game.crime_rate / 5);
                draw_text(1, 6, "crime :", 1);
                if (v >= 0) { draw_text(13, 6, "+", 1); draw_number(14, 6, (uint16_t)v, 1); }
                else        { draw_text(13, 6, "-", 1); draw_number(14, 6, (uint16_t)(-v), 1); }
            }
            // Chomage
            {
                int16_t v = -(int16_t)(game.unemployment_rate / 5);
                draw_text(1, 8, "chomage :", 1);
                if (v >= 0) { draw_text(13, 8, "+", 1); draw_number(14, 8, (uint16_t)v, 1); }
                else        { draw_text(13, 8, "-", 1); draw_number(14, 8, (uint16_t)(-v), 1); }
            }
            // Sans-abri
            {
                int16_t pen = (int16_t)(game.homeless * 2);
                if (pen > 20) pen = 20;
                pen = -pen;
                draw_text(1, 10, "sans-abri :", 1);
                if (pen >= 0) { draw_text(13, 10, "+", 1); draw_number(14, 10, (uint16_t)pen, 1); }
                else          { draw_text(13, 10, "-", 1); draw_number(14, 10, (uint16_t)(-pen), 1); }
            }
            // Electricite
            {
                int16_t v = (game.electricity_prod < game.electricity_cons) ? -10 : 0;
                draw_text(1, 12, "~lectricit~ :", 1);
                if (v >= 0) { draw_text(13, 12, "+", 1); draw_number(14, 12, (uint16_t)v, 1); }
                else        { draw_text(13, 12, "-", 1); draw_number(14, 12, (uint16_t)(-v), 1); }
            }
        }

        else if (stats_page == 9) { // --- PAGE 10 : FACTEURS BONHEUR 2 ---
            draw_text(6, 1, GET_TEXT(TXT_STATS_ALMANACH), 1);
            draw_text(4, 2, "bonheur (2/2)", 1);
            // Sante
            {
                int8_t v = game.health_hap_bonus;
                draw_text(1, 4, "sant~ :", 1);
                if (v >= 0) { draw_text(13, 4, "+", 1); draw_number(14, 4, (uint16_t)v, 1); }
                else        { draw_text(13, 4, "-", 1); draw_number(14, 4, (uint16_t)(-v), 1); }
            }
            // Decret nourriture
            {
                int8_t v = game.decree_food_hap_bonus;
                draw_text(1, 6, "d~cret food :", 1);
                if (v >= 0) { draw_text(13, 6, "+", 1); draw_number(14, 6, (uint16_t)v, 1); }
                else        { draw_text(13, 6, "-", 1); draw_number(14, 6, (uint16_t)(-v), 1); }
            }
            // Famine
            {
                draw_text(1, 8, "famine :", 1);
                if (game.is_in_famine) { draw_text(13, 8, "-", 1); draw_number(14, 8, 1, 1); }
                else                   { draw_text(13, 8, "0", 1); }
            }
            // Bonheur total actuel
            draw_text(1, 11, "bonheur total :", 1);
            draw_number(15, 12, game.avg_happiness, 1);
        }

        redraw = 0; // On arrête de dessiner jusqu'au prochain changement
    }

    joy = joypad();

    if (joy & J_B) {
        menu_running = 0;
        waitpadup();
    }

    if (joy & J_RIGHT) {
        stats_page++;
        if(stats_page > 9) stats_page = 0;
        nb_play_sound_build_b4();
        redraw = 1; // On demande de redessiner
        waitpadup();
    }

    if (joy & J_LEFT) {
        if(stats_page == 0) stats_page = 9;
        else stats_page--;
        nb_play_sound_build_b4();
        redraw = 1; // On demande de redessiner
        waitpadup();
    }

    wait_vbl_done();
}

// Sortie de l'Almanach
{ uint8_t si; for (si = 1u; si <= 7u; si++) move_sprite(si, 0u, 0u); }
// Restaurer les tiles des sprites curseur (slots 1-4) écrasées par les coches d'objectifs
set_sprite_tile(1u, 1u); set_sprite_tile(2u, 2u); set_sprite_tile(3u, 3u); set_sprite_tile(4u, 4u);
}
