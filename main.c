#pragma bank 7
#include <gb/gb.h>
#include <stdint.h>
#include <gbdk/font.h>
#include "assets.h"
#include "constants.h"
#include "game_state.h"
#include "graphics.h"
#include "audio.h"
#include "logic.h"
#include "economy.h"
#include "save.h"
#include "menus.h"
#include "intro.h"
#include "minimap.h"
#include "story_mode.h"
#include "map_mission1_water.h"
#include "lang.h"
#include "plane.h"
extern void draw_menu_border(void);
extern void decrets_screen(void);


uint8_t curs_x = 10;
uint8_t curs_y = 10;

// Buffer WRAM pour les textes de mission (bank 4). Taille pour le plus long dialogue.
static char mission_text_buf[400];
// Buffer WRAM pour nb_get_text() (textes UI bank 2).
static char nb_get_text_buf[22];

// Copie le texte de mission idx depuis bank 5 dans mission_text_buf (WRAM).
// Sauvegarde et restaure la bank courante — safe depuis n'importe quelle bank.
char* nb_get_mission_text(uint8_t idx) __nonbanked {
    char *src, *dst;
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(5);
    src = (current_lang == LANG_FR) ? mission_strings_fr[idx] : mission_strings_en[idx];
    dst = mission_text_buf;
    while (*src) *dst++ = *src++;
    *dst = '\0';
    SWITCH_ROM(saved);
    return mission_text_buf;
}

// Wrapper NONBANKED pour get_text() (bank 2).
// Copie la chaîne en RAM avant de restaurer la bank — safe depuis n'importe quelle bank.
char* nb_get_text(uint8_t idx) __nonbanked {
    char *src;
    uint8_t i;
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(2);
    src = get_text(idx);
    for (i = 0; i < 21u && src[i] != '\0'; i++) nb_get_text_buf[i] = src[i];
    nb_get_text_buf[i] = '\0';
    SWITCH_ROM(saved);
    return nb_get_text_buf;
}

// Appelé depuis story_mode.c (bank 4) : lit l'étape courante en bank 5, restaure bank 4.
void nb_get_mission_step_b4(uint8_t step_idx, MissionStep *out) __nonbanked {
    SWITCH_ROM(5);
    copy_mission_step(step_idx, out);
    SWITCH_ROM(4);
}

// Wrapper NONBANKED : toujours en bank 0 (fixe), safe pour switcher vers bank 2
// intro_title_animation() est en bank 2 (#pragma bank 2 dans intro.c)
void call_intro_title_animation(void) __nonbanked {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(2);
    intro_title_animation();
    SWITCH_ROM(saved);
}

// Wrapper NONBANKED pour la minimap (bank 2)
void call_show_minimap(void) __nonbanked {
    SWITCH_ROM(2);
    show_minimap();
    SWITCH_ROM(1);
}

// ---- Wrappers NONBANKED pour story_mode.c (bank 4) ----

// Appelé depuis update_story_logic() (bank 1) : exécute story_mode_logic() en bank 4.
void call_story_mode_logic(void) __nonbanked {
    SWITCH_ROM(4);
    story_mode_logic();
    SWITCH_ROM(1);
}

// Appelé depuis story_mode_logic() (bank 4) : affiche le dialogue en bank 1.
// Copie d'abord le texte depuis bank 5 dans mission_text_buf (WRAM), puis switch bank 1.
// Après retour, remet bank 4 pour que story_mode_logic() puisse continuer.
void nb_story_dialogue_b2(uint8_t portrait_id, uint16_t text_idx) __nonbanked {
    nb_get_mission_text(text_idx);  // bank 5 → mission_text_buf, restaure bank 4
    SWITCH_ROM(1);
    load_lowercase_font();
    story_dialogue_animated(portrait_id, mission_text_buf);
    BGP_REG = 0xFF;  // blackout palette avant de recharger les tiles map
    restore_map_tiles();
    SWITCH_ROM(4);
}

// Appelé depuis logic.c (bank 1) : recharge les tiles voiture depuis bank 3.
void nb_reload_limousine_b1(void) __nonbanked {
    SWITCH_ROM(3);
    set_sprite_data(0x05, 1, car_tile_h);
    set_sprite_data(0x06, 1, car_tile_v);
    set_sprite_data(0x07, 1, car_tile_h2);
    set_sprite_data(0x08, 1, car_tile_v2);
    set_sprite_data(0x76, 4, tram); // tiles tram 0x76-0x79
    set_sprite_data(0x7Au, 2, boat);
    set_sprite_data(0x7Cu, 1, boat + 32u);
    set_sprite_data(0x7Eu, 1, boat + 48u);
    //set_sprite_data(0x72u, 4, boatBack);
    SWITCH_ROM(1);
}

// Appelé depuis menus.c (bank 2) : spawn le tram (bank 1).
void nb_tram_init_b2(void) __nonbanked { SWITCH_ROM(1); tram_init(); SWITCH_ROM(2); }
void nb_boat_init_b2(void) __nonbanked { SWITCH_ROM(1); boat_init(); SWITCH_ROM(2); }
// Safe depuis n'importe quelle bank (utilisé par decrets.c bank 4)
void nb_tram_init_safe(void) __nonbanked { uint8_t s = CURRENT_BANK; SWITCH_ROM(1); tram_init(); SWITCH_ROM(s); }

// Appelé depuis logic.c (bank 1) : recharge les tiles limo depuis bank 3.
void nb_reload_limo_tiles_b1(void) __nonbanked {
    SWITCH_ROM(3);
    set_sprite_data(9, 27, limousine);
    SWITCH_ROM(1);
}

// Appelé depuis boat.c (bank 1) : charge les tiles boat ou boatBack depuis bank 3.
void nb_reload_boat_tiles_b1(uint8_t fwd) __nonbanked {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(3);
    const unsigned char *src = fwd ? boat : boatBack;
    set_sprite_data(0x7Au, 2, src);
    set_sprite_data(0x7Cu, 1, src + 32u);
    set_sprite_data(0x7Eu, 1, src + 48u);
    SWITCH_ROM(saved);
}

// Appelé depuis menus.c (bank 2) : recharge les 9 tiles eau (0x6D-0x75) depuis bank 3.
// Nécessaire après show_minimap() qui écrase les slots 109-111 (=0x6D-0x6F).
void nb_reload_water_tiles_b2(void) __nonbanked {
    SWITCH_ROM(3);
    set_bkg_data(0x6D, 6, water);
    set_bkg_data(0x73, 3, road_inters); // road_inters écrase water aux slots 0x73-0x75
    SWITCH_ROM(2);
}

void nb_reload_port_tiles_b2(void) __nonbanked {
    SWITCH_ROM(3);
    set_bkg_data(0xFB, 4, port); // restaure 0xFB-0xFE écrasés par minimap (MM_TILE_BUILD/HEAVY)
    SWITCH_ROM(2);
}

// Appelé depuis story_mode_logic() (bank 4) et pause_menu() (bank 2) : met à jour le HUD (bank 1).
void nb_update_hud_b2(void) __nonbanked {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(1);
    update_hud();
    SWITCH_ROM(saved);
}

// Appelé depuis story_mode_logic() (bank 4) : joue le son de succès (bank 1).
void nb_play_mission_b2(void) __nonbanked {
    SWITCH_ROM(1);
    play_sound_mission_success();
    SWITCH_ROM(4);
}

// Appelé depuis main.c (bank 0) : met à jour l'avion (bank 3).
void nb_plane_update_b3(int16_t cam_x, int16_t cam_y) __nonbanked {
    uint8_t s = CURRENT_BANK;
    SWITCH_ROM(3);
    plane_update(cam_x, cam_y);
    SWITCH_ROM(s);
}
void nb_water_anim_update(void) __nonbanked {
    uint8_t s = CURRENT_BANK;
    SWITCH_ROM(3);
    water_anim_update();
    SWITCH_ROM(s);
}

// Lit l'étape courante depuis bank 5 et copie dans *out. Restaure bank 1.
void nb_get_story_step(MissionStep *out) __nonbanked {
    SWITCH_ROM(5);
    copy_mission_step(game.current_step, out);
    SWITCH_ROM(1);
}

// Appelé depuis menus.c/save.c (bank 2) : dessine la bordure menu (bank 4).
void nb_draw_menu_border(void) __nonbanked {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(4);
    draw_menu_border();
    SWITCH_ROM(saved);
}

// Appelé depuis menus.c (bank 2) : affiche l'écran des décrets (bank 4).
void nb_decrets_screen_b4(void) __nonbanked {
    SWITCH_ROM(4);
    decrets_screen();
    SWITCH_ROM(2);
}

// Appelé depuis menus.c (bank 2) : affiche l'almanach (bank 4).
void nb_stats_screen_b4(void) __nonbanked {
    SWITCH_ROM(4);
    stats_screen();
    SWITCH_ROM(2);
}

// Appelé depuis stats_screen.c (bank 4) : story_get_current_step() est en bank 4.
void nb_story_get_current_step_b2(MissionStep *out) __nonbanked {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(4);
    story_get_current_step(out);
    SWITCH_ROM(saved);
}

// Wrappers nonbanked : menus.c (bank 1) → save.c (bank 2)
uint8_t nb_load_saves_screen(void) __nonbanked { uint8_t r; SWITCH_ROM(2); r = load_saves_screen(); SWITCH_ROM(1); return r; }
void nb_load_story_game(uint8_t bank_index) __nonbanked { SWITCH_ROM(2); load_story_game(bank_index); SWITCH_ROM(1); }

// Wrappers nonbanked : save.c (bank 2) → bank 1
void nb_enable_sram_b2(void)             __nonbanked { SWITCH_ROM(1); enable_sram();               SWITCH_ROM(2); }
void nb_disable_sram_b2(void)            __nonbanked { SWITCH_ROM(1); disable_sram();              SWITCH_ROM(2); }
void nb_switch_ram_bank_b2(uint8_t bank) __nonbanked { SWITCH_ROM(1); switch_ram_bank(bank);       SWITCH_ROM(2); }
void nb_init_game_variables_b2(void)     __nonbanked { SWITCH_ROM(1); init_game_variables();       SWITCH_ROM(2); }
void nb_rebuild_registry_b2(void)        __nonbanked { SWITCH_ROM(1); rebuild_registry_from_map(); SWITCH_ROM(2); }
void nb_play_sound_erase_b2(void)        __nonbanked { SWITCH_ROM(1); play_sound_erase();          SWITCH_ROM(2); }
void nb_play_sound_build_b2(void)        __nonbanked { SWITCH_ROM(1); play_sound_build();          SWITCH_ROM(2); }
void nb_play_sound_build_b4(void)        __nonbanked { uint8_t s = CURRENT_BANK; SWITCH_ROM(1); play_sound_build(); SWITCH_ROM(s); }
void nb_play_sound_error_b2(void)        __nonbanked { SWITCH_ROM(1); play_sound_error();          SWITCH_ROM(2); }
void nb_perform_save_b2(void)            __nonbanked { SWITCH_ROM(1); perform_save();              SWITCH_ROM(2); }
void nb_setup_story_params_b2(uint8_t mission_id, uint8_t text_y) __nonbanked { SWITCH_ROM(1); setup_story_params(mission_id, text_y); SWITCH_ROM(2); }

// Wrapper NONBANKED : copie map_mission1_water (bank 4) dans ram_map, puis restaure bank 2
void nb_load_water_map_b2(void) __nonbanked {
    uint16_t i;
    SWITCH_ROM(4);
    for(i = 0; i < 4096; i++) ram_map[i] = map_mission1_water[i];
    SWITCH_ROM(2);
}

// Wrapper NONBANKED pour l'économie (bank 5, depuis economy_update.c)
void call_update_economy(void) __nonbanked {
    SWITCH_ROM(5);
    update_economy();
    SWITCH_ROM(1);
}
void call_update_economy_start(void) __nonbanked {
    SWITCH_ROM(6);
    update_economy_start();
    SWITCH_ROM(1);
}
void call_update_economy_tick(void) __nonbanked {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(6);
    update_economy_tick();
    SWITCH_ROM(saved);
}

// Wrappers nonbanked pour appels depuis economy.c (bank 3) vers bank 1
// (après l'appel, on restaure bank 3 pour que l'appelant soit encore mappé)
void nb_play_sound_build(void) __nonbanked { SWITCH_ROM(1); play_sound_build(); SWITCH_ROM(3); }
void nb_play_sound_error(void) __nonbanked { SWITCH_ROM(1); play_sound_error(); SWITCH_ROM(3); }
void nb_update_hud_b3(void)    __nonbanked { SWITCH_ROM(1); update_hud();       SWITCH_ROM(3); }

// Wrappers nonbanked vers logic.c (bank 1) pour get_tool_size / get_tool_base_tile / get_erase_info
uint8_t nb_get_tool_size(uint8_t tool) __nonbanked {
    uint8_t s = CURRENT_BANK;
    SWITCH_ROM(1);
    uint8_t r = get_tool_size(tool);
    SWITCH_ROM(s);
    return r;
}
uint8_t nb_get_tool_base_tile(uint8_t tool) __nonbanked {
    uint8_t s = CURRENT_BANK;
    SWITCH_ROM(1);
    uint8_t r = get_tool_base_tile(tool);
    SWITCH_ROM(s);
    return r;
}
uint8_t nb_get_erase_info(uint8_t et, EraseInfo *out) __nonbanked {
    uint8_t s = CURRENT_BANK;
    SWITCH_ROM(1);
    uint8_t r = get_erase_info(et, out);
    SWITCH_ROM(s);
    return r;
}

// Force la connaissance de la fonction pour éviter l'erreur 101
uint8_t get_sub_tool_count(uint8_t category);
extern const uint8_t building_mapping[6][4];

uint8_t nb_get_building_tool_b7(uint8_t cat, uint8_t sub) __nonbanked {
    uint8_t r; SWITCH_ROM(1); r = building_mapping[cat][sub]; SWITCH_ROM(7); return r; }

// ---- Wrappers NONBANKED bank 1 → retour bank 7 (pour game_main()) ----
void nb_move_cursor_b7(uint8_t x, uint8_t y, int16_t cx, int16_t cy) __nonbanked {
    SWITCH_ROM(1); move_cursor(x, y, cx, cy); SWITCH_ROM(7); }
void nb_update_game_logic_b7(void) __nonbanked {
    SWITCH_ROM(1); update_game_logic(); SWITCH_ROM(7); }
void nb_update_story_logic_b7(void) __nonbanked {
    SWITCH_ROM(1); update_story_logic(); SWITCH_ROM(7); }
void nb_cars_update_b7(int16_t cx, int16_t cy) __nonbanked {
    SWITCH_ROM(1); cars_update(cx, cy); SWITCH_ROM(7); }
void nb_tram_update_b7(int16_t cx, int16_t cy) __nonbanked {
    SWITCH_ROM(1); tram_update(cx, cy); SWITCH_ROM(7); }
void nb_boat_update_b7(int16_t cx, int16_t cy) __nonbanked {
    SWITCH_ROM(1); boat_update(cx, cy); SWITCH_ROM(7); }
void nb_draw_hud_money_date_b7(uint8_t row) __nonbanked {
    SWITCH_ROM(1); draw_hud_money_date(row); SWITCH_ROM(7); }
void nb_refresh_flags_tick_b7(void) __nonbanked {
    SWITCH_ROM(1); refresh_flags_tick(); SWITCH_ROM(7); }
void nb_update_hud_b7(void) __nonbanked {
    SWITCH_ROM(1); update_hud(); SWITCH_ROM(7); }
void nb_update_road_display_b7(uint8_t x, uint8_t y) __nonbanked {
    SWITCH_ROM(1); update_road_display(x, y); SWITCH_ROM(7); }
void nb_update_road_display_safe(uint8_t x, uint8_t y) __nonbanked {
    uint8_t s = CURRENT_BANK; SWITCH_ROM(1); update_road_display(x, y); SWITCH_ROM(s); }
void nb_update_building_flags_b7(uint8_t idx) __nonbanked {
    SWITCH_ROM(1); update_building_flags(idx); SWITCH_ROM(7); }
void nb_add_building_b7(uint16_t idx, uint8_t type) __nonbanked {
    SWITCH_ROM(1); add_building(idx, type); SWITCH_ROM(7); }
void nb_remove_building_b7(uint16_t idx) __nonbanked {
    SWITCH_ROM(1); remove_building(idx); SWITCH_ROM(7); }
void nb_cars_init_b7(void) __nonbanked {
    SWITCH_ROM(1); cars_init(); SWITCH_ROM(7); }
void nb_play_sound_build_b7(void) __nonbanked {
    SWITCH_ROM(1); play_sound_build(); SWITCH_ROM(7); }
void nb_play_sound_erase_b7(void) __nonbanked {
    SWITCH_ROM(1); play_sound_erase(); SWITCH_ROM(7); }
void nb_play_sound_error_b7(void) __nonbanked {
    SWITCH_ROM(1); play_sound_error(); SWITCH_ROM(7); }
void nb_play_destruction_fx_b7(uint8_t x, uint8_t y, uint8_t s) __nonbanked {
    SWITCH_ROM(1); play_destruction_fx(x, y, s); SWITCH_ROM(7); }
uint8_t nb_get_sub_tool_count_b7(uint8_t cat) __nonbanked {
    uint8_t r; SWITCH_ROM(1); r = get_sub_tool_count(cat); SWITCH_ROM(7); return r; }
void nb_force_cursor_update_b7(void) __nonbanked {
    SWITCH_ROM(1); force_cursor_update(); SWITCH_ROM(7); }
void nb_update_sub_tool_icons_b7(void) __nonbanked {
    SWITCH_ROM(1); update_sub_tool_icons(); SWITCH_ROM(7); }
void nb_enable_sram_b7(void) __nonbanked {
    SWITCH_ROM(1); enable_sram(); SWITCH_ROM(7); }
void nb_disable_sram_b7(void) __nonbanked {
    SWITCH_ROM(1); disable_sram(); SWITCH_ROM(7); }
void nb_switch_ram_bank_b7(uint8_t bank) __nonbanked {
    SWITCH_ROM(1); switch_ram_bank(bank); SWITCH_ROM(7); }
void nb_trigger_alert_b7(char* msg) __nonbanked {
    SWITCH_ROM(1); trigger_alert(msg); SWITCH_ROM(7); }

// Charge tous les assets initiaux depuis bank 3 (appelé depuis game_main() bank 7)
void nb_load_all_assets_b7(void) __nonbanked {
    SWITCH_ROM(3);
    set_bkg_data(128, 32, tile_data);
    set_bkg_data(160, 9, farmish);
    set_bkg_data(169, 4, plantation);
    set_bkg_data(173, 16, sawmill);
    set_bkg_data(189, 4, police);
    set_bkg_data(193, 9, church);
    set_bkg_data(202, 16, hospital);
    set_bkg_data(61, 4, basicCom);
    set_bkg_data(218, 16, power);
    set_bkg_data(234, 16, mine);
    set_bkg_data(0x59, 9, mediaDisco);
    set_bkg_data(0x63, 4, baraque);
    set_bkg_data(0x6D, 6, water);
    set_bkg_data(0x67, 6, road_x6);
    set_bkg_data(0x73, 3, road_inters);
    set_bkg_data(0xFF, 1, oneTileHome);
    set_bkg_data(37, 16, shop);
    set_bkg_data(53, 7, specialChars);
    set_bkg_data(60, 1, fleche);
    set_bkg_data(65, 4, iconCatRoadDel);
    set_bkg_data(69, 4, iconBuildings);
    set_bkg_data(73, 4, iconRes);
    set_bkg_data(77, 4, iconGov);
    set_bkg_data(81, 4, iconMines);
    set_bkg_data(85, 4, iconCatLoisirs);
    set_bkg_data(0x62, 1, dollar);
    set_bkg_data(0x76, 3, troisCotesMer);
    set_bkg_data(0x6D, 5, water);
    set_bkg_data(0xFA, 1, charb);
    set_bkg_data(0xFB, 4, port);
    set_sprite_data(PENULTIMO_TILE_START, 14, penultimoBig);
    set_sprite_data(5, 4, icon01);
    set_sprite_data(9, 27, limousine);
    set_sprite_data(0, 5, cursor_data);
    set_sprite_data(0x28, 4, iconSubRoad);
    set_sprite_data(0x2C, 4, iconSubDel);
    set_sprite_data(0x30, 4, iconSubHouse);
    set_sprite_data(0x34, 4, iconSubShop);
    set_sprite_data(0x38, 4, iconSubFactory);
    set_sprite_data(0x3C, 4, iconSubFarm);
    set_sprite_data(0x40, 4, iconSubPlantation);
    set_sprite_data(0x44, 4, iconSubWood);
    set_sprite_data(0x48, 4, iconSubPolice);
    set_sprite_data(0x4C, 4, iconSubChurch);
    set_sprite_data(0x50, 4, iconSubHospital);
    set_sprite_data(0x54, 4, iconSubSchool);
    set_sprite_data(0x58, 4, iconEnergy);
    set_sprite_data(0x5C, 4, iconSubMines);
    set_sprite_data(0x64, 4, iconSubBar);
    set_sprite_data(0x68, 4, iconSubMediaDisco);
    set_sprite_data(0x6C, 4, iconSubUpgrade);
    set_sprite_data(0x60, 4, iconSubPort);
    set_sprite_data(0x24, 4, iconSubOneTileHome);
    set_sprite_data(50, 4, titleEl);
    set_sprite_data(54, 18, titlePresidente);
    set_sprite_data(0x7D, 1, cursor_tick_tile);
    set_sprite_data(0x7F, 1, flash);
    SWITCH_ROM(7);
}

// Recharge les sprites après l'intro (bank 3 → bank 7)
void nb_reload_after_intro_b7(void) __nonbanked {
    SWITCH_ROM(3);
    set_sprite_data(0x05, 1, car_tile_h);
    set_sprite_data(0x06, 1, car_tile_v);
    set_sprite_data(0x07, 1, car_tile_h2);
    set_sprite_data(0x08, 1, car_tile_v2);
    set_sprite_data(0x76, 4, tram);
    set_sprite_data(0x7Au, 2, boat);
    set_sprite_data(0x7Cu, 1, boat + 32u);
    set_sprite_data(0x7Eu, 1, boat + 48u);
    set_sprite_data(0x30, 4, iconSubHouse);
    set_sprite_data(0x34, 4, iconSubShop);
    set_sprite_data(0x38, 4, iconSubFactory);
    set_sprite_data(0x3C, 4, iconSubFarm);
    set_sprite_data(0x40, 4, iconSubPlantation);
    set_sprite_data(0x44, 4, iconSubWood);
    SWITCH_ROM(7);
}

// Recharge la limo depuis bank 3 (boucle de jeu, bank 7)
void nb_reload_limo_b7(void) __nonbanked {
    SWITCH_ROM(3);
    set_sprite_data(9, 27, limousine);
    SWITCH_ROM(7);
}

// Lance plane_init() depuis bank 3 (boucle de jeu, bank 7)
void nb_plane_init_b7(void) __nonbanked {
    SWITCH_ROM(3);
    plane_init();
    SWITCH_ROM(7);
}

// Cherche le bâtiment sous le curseur et ouvre le menu contextuel (bank 3, bank 7)
uint8_t nb_find_and_context_menu_b7(void) __nonbanked {
    uint8_t bldg_idx;
    SWITCH_ROM(3);
    bldg_idx = find_building_at_cursor();
    if (bldg_idx != 0xFF) {
        show_building_context_menu(bldg_idx);
    }
    SWITCH_ROM(7);
    return bldg_idx;
}

// Appels bank 2 depuis game_main() (bank 7)
void nb_language_selection_screen_b7(void) __nonbanked { SWITCH_ROM(2); language_selection_screen(); SWITCH_ROM(7); }
void nb_main_menu_b7(void) __nonbanked { SWITCH_ROM(2); main_menu(); SWITCH_ROM(7); }
void nb_pause_menu_b7(void) __nonbanked { SWITCH_ROM(2); pause_menu(); SWITCH_ROM(7); }
void nb_show_menu_b7(void) __nonbanked {
    SWITCH_ROM(1); show_menu(); SWITCH_ROM(7); }
void nb_hide_menu_b7(void) __nonbanked {
    SWITCH_ROM(1); hide_menu(); SWITCH_ROM(7); }

void game_main(void) {
    DISPLAY_OFF;
    
    //init_game_variables();

    // 1. INITIALISATION AUDIO (DOIT ÊTRE FAITE AVANT LOGO_SCREEN)
    NR52_REG = 0x80; // Allume le processeur sonore
    NR50_REG = 0x77; // Volume max
    NR51_REG = 0xFF; // Tous les canaux activés

    // Chargement de la police dans les tiles de Sprites (0x8000+)
    // On décale de -49 pour que '0' tombe sur l'indice 1 (VRAM 0x8010)
    font_init();
    font_set(font_load(font_min) - 49);

    // Variables pour suivre la pose des routes
    uint8_t last_placed_x = 255;
    uint8_t last_placed_y = 255;

    // Toutes les données de tiles/sprites sont en bank 3 (assets.c)
    nb_load_all_assets_b7();

    call_intro_title_animation();

    // Après l'intro, les iconSub 0x30-0x44 ont été écrasés par titleEl/titlePresidente : on les recharge
    // On charge aussi les tiles voiture en 0x05-0x08
    nb_reload_after_intro_b7();

    // Restauration curseur après intro
    set_sprite_tile(0, 0);

    nb_language_selection_screen_b7();

    // INITIALISATION DE LA SRAM (Slots vides) ---
    // On active la banque de registres de la cartouche pour autoriser la SRAM
    *(volatile uint8_t *)0x6000 = 0x01;

    nb_enable_sram_b7();
    for(uint8_t slot = 0; slot < 4; slot++) {
        nb_switch_ram_bank_b7(slot);

        // On pointe sur la structure en SRAM pour vérifier la signature
        GameState *sram_ptr = (GameState *)(0xA000 + 4096);

        if (sram_ptr->signature != 99) {
            // Initialisation d'une carte vide pour les nouveaux slots
            for (uint16_t i = 0; i < 4096; i++) ram_map[i] = TYPE_EMPTY;

            // On met une signature à 0 pour dire "Vide"
            sram_ptr->signature = 0;
        }
    }

    nb_disable_sram_b7();

    while(1) { // Boucle "Application"
        force_return_to_menu = 0;
        fade_in();
        nb_main_menu_b7();
        fade_out();
        DISPLAY_OFF;
        move_win(7, game.game_mode == MODE_STORY ? 144 : 136); update_view(); SCX_REG = (uint8_t)cam_x; SCY_REG = (uint8_t)cam_y; cam_target_x = cam_x; cam_target_y = cam_y; nb_update_hud_b7();
        
//===============================================================================================================
        //===================================================================================================================
        // sortir tout ça de la boucle
        //===============================================================================================================
        //===============================================================================================================

        // sprites du curseur
        /*set_sprite_tile(0, 0); set_sprite_tile(1, 1); set_sprite_tile(2, 2); 
        set_sprite_tile(3, 3); set_sprite_tile(4, 4);

        // sprites du l'icone de test 1 pour sub category
        set_sprite_tile(5, 5);
    set_sprite_tile(6, 6);
    set_sprite_tile(7, 7);
    set_sprite_tile(8, 8);

        // sprites du l'icone de test 2 pour sub category
        set_sprite_tile(9, 5);
    set_sprite_tile(10, 6);
    set_sprite_tile(11, 7);
    set_sprite_tile(12, 8);

        // sprites du l'icone de test 3 pour sub category
        set_sprite_tile(13, 5);
    set_sprite_tile(14, 6);
    set_sprite_tile(15, 7);
    set_sprite_tile(16, 8);*/

        // LCDC_REG : Bit 4 à 1 pour que le Window utilise la zone 0x8000 (Sprites)
        LCDC_REG = 0xE3;
        SHOW_BKG; SHOW_WIN; SHOW_SPRITES; DISPLAY_ON;
        fade_in();
        if (game.game_mode == MODE_STORY) move_win(7, 136);

        // Recharge les tiles limo (peuvent être écrasées par le portrait Penultimo lors d'une partie précédente)
        nb_reload_limo_b7();

        // Recharge les tiles bateau (peuvent être écrasées par le menu principal)
        nb_reload_boat_tiles_b1(1);

        nb_cars_init_b7();
        nb_plane_init_b7();

        uint8_t move_timer = 0;
        uint8_t hud_money_dirty = 0;
        uint8_t hud_throttle = 0;
        // (flags_step est global dans logic.c, pas besoin de déclaration locale)
        while (1) { // boucle de jeu



            // 1. DÉPLACEMENT DU CURSEUR (Toujours actif, même si A est pressé)
    // Cette fonction doit lire le joypad et changer curs_x/curs_y
    nb_move_cursor_b7(curs_x, curs_y, cam_target_x, cam_target_y);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


            if (force_return_to_menu) {
                for (uint8_t i=1 ; i<5 ; i++) { move_sprite(i, 0, 0);}
                hide_vehicle_sprites();
                DISPLAY_OFF;
                break;
            }

            uint8_t joy = joypad();

            if (joy & J_START && !is_menu_open) {
                uint8_t hi;
                for (hi = 21u; hi <= 39u; hi++) move_sprite(hi, 0u, 0u);
                nb_pause_menu_b7();
                nb_update_hud_b7();
            }

            if (joy & J_SELECT) {
                if (!is_menu_open)
                current_tool = 0; // remet le curseur sur la 1ere catégorie
                nb_show_menu_b7(); // ça passe is_menu_open à 1 : elle sert à afficher le hud mais introuvable dans le code mdr
                nb_force_cursor_update_b7();

                nb_update_sub_tool_icons_b7(); // Affiche les icônes immédiatement

                waitpadup();
            }

            uint8_t prev_road_x = curs_x;
            uint8_t prev_road_y = curs_y;

            if (!is_menu_open) {
                uint8_t d = joy & (J_LEFT | J_RIGHT | J_UP | J_DOWN);
                if (d) {
                    if (move_timer == 0) {
                        {
                            uint8_t cur_sz = (joy & J_B) ? 4u : nb_get_tool_size(current_tool);
                            uint8_t cur_off = (cur_sz >= 3) ? 1 : 0;
                            uint8_t max_cx = (uint8_t)(WORLD_WIDTH  - cur_sz + cur_off);
                            uint8_t max_cy = (uint8_t)(WORLD_HEIGHT - 1 - cur_sz + cur_off);
                            if (curs_x < cur_off) curs_x = cur_off;
                            if (curs_x > max_cx)  curs_x = max_cx;
                            if (curs_y < cur_off) curs_y = cur_off;
                            if (curs_y > max_cy)  curs_y = max_cy;
                            if      (joy & J_RIGHT && curs_x < max_cx)  curs_x++;
                            else if (joy & J_LEFT  && curs_x > cur_off) curs_x--;
                            if      (joy & J_DOWN  && curs_y < max_cy)  curs_y++;
                            else if (joy & J_UP    && curs_y > cur_off) curs_y--;
                        }
                        {
                            int16_t tx = (int16_t)(curs_x * 8) - 80;
                            int16_t ty = (int16_t)(curs_y * 8) - 72;
                            if (tx < 0) tx = 0; if (tx > 352) tx = 352;
                            if (ty < 0) ty = 0; if (ty > 368) ty = 368;
                            cam_target_x = tx; cam_target_y = ty;
                        }
                        move_timer = (joy & J_B) ? 4 : 8;
                    } else {
                        move_timer--;
                    }
                } else {
                    move_timer = 0;
                }
                fast_move = (d && (joy & J_B)) ? 1 : 0;
            }

                if (joy & J_A) {
                    if (!is_menu_open && (!(joy & (J_LEFT | J_RIGHT | J_UP | J_DOWN)) || current_tool == TOOL_ROAD || current_tool == TOOL_ERASE)){
                    nb_enable_sram_b7();
                    nb_switch_ram_bank_b7(current_save_slot);
                    uint16_t idx = ((uint16_t)curs_y * WORLD_WIDTH) + curs_x;
                    uint8_t t = ram_map[idx];
                    int32_t money_before = game.money;


                    if (current_tool == TOOL_ROAD)
                    {
                        uint8_t cost = 1;
                        uint8_t can_build = 1;
                        uint16_t road_idx = ((uint16_t)prev_road_y << 6) + prev_road_x;
                        if(!IS_EMPTY(ram_map[road_idx])) can_build = 0;
                        if (can_build && game.money >= cost) {
                            game.money -= cost;
                            game.monthly_expenses += cost;
                            game.exp_construction  += cost;

                            // Si on a bougé le curseur sur une nouvelle case
                            if (prev_road_x != last_placed_x || prev_road_y != last_placed_y) {

                                ram_map[road_idx] = VAL_ROAD;
                                if (game.decree_tram) tram_needs_spawn = 1;

                                // On force l'affichage immédiat (ça écrase la tuile "1")
                                nb_update_road_display_b7(prev_road_x, prev_road_y);

                                // On met à jour les voisins
                                if (prev_road_y > 0)  nb_update_road_display_b7(prev_road_x, prev_road_y - 1);
                                if (prev_road_x < 63) nb_update_road_display_b7(prev_road_x + 1, prev_road_y);
                                if (prev_road_y < 63) nb_update_road_display_b7(prev_road_x, prev_road_y + 1);
                                if (prev_road_x > 0)  nb_update_road_display_b7(prev_road_x - 1, prev_road_y);

                                // Recalcul des flags des bâtiments adjacents à la route posée
                                for (uint8_t _ri = 0; _ri < building_count; _ri++)
                                    nb_update_building_flags_b7(_ri);

                                last_placed_x = prev_road_x;
                                last_placed_y = prev_road_y;

                                // Débloquer le spawn des voitures si elles attendaient une route
                                if (cars_no_road) { cars_no_road = 0; nb_cars_init_b7(); }
                            }

                            else {
                                // Reset si on lâche le bouton
                                last_placed_x = 255;
                            }
                        }
                    }
// logique de destruction (pour la taille du curseur sur la map, c'est dans logic.c) ==================================
                    else if (current_tool == TOOL_ERASE) {
                        uint8_t erase_x = curs_x, erase_y = curs_y;
                        if (joy & J_RIGHT) erase_x--;
                        else if (joy & J_LEFT)  erase_x++;
                        if (joy & J_DOWN)  erase_y--;
                        else if (joy & J_UP)    erase_y++;
                        uint16_t erase_idx = ((uint16_t)erase_y * WORLD_WIDTH) + erase_x;
                        uint8_t et = ram_map[erase_idx];
                        if (!IS_EMPTY(et)) {
                            uint8_t size = 0, base_t = 0;
                            int32_t cost = 0;
                            if (et == TILE_ONETILEHOME) {
                                size = 2; base_t = TILE_ONETILEHOME; cost = 80;
                                // Cherche le coin NW (les 4 tuiles sont identiques)
                                if (erase_x > 0 && ram_map[erase_idx - 1] == TILE_ONETILEHOME) erase_x--;
                                if (erase_y > 0 && ram_map[erase_idx - 64] == TILE_ONETILEHOME) erase_y--;
                            } else {
                                EraseInfo ei;
                                if (nb_get_erase_info(et, &ei)) { size = ei.size; base_t = ei.base_t; cost = ei.cost; }
                            }

                            if (size > 0) {
                                // En glissement, éviter d'effacer la même case deux fois
                                if (size == 1 && (joy & (J_LEFT | J_RIGHT | J_UP | J_DOWN))) {
                                    if (erase_x == last_placed_x && erase_y == last_placed_y) goto erase_done;
                                    last_placed_x = erase_x; last_placed_y = erase_y;
                                }
                                uint8_t off = et - base_t;
                                uint8_t rx = erase_x - (off % size), ry = erase_y - (off / size);
                                if (!(joy & (J_LEFT | J_RIGHT | J_UP | J_DOWN))) nb_play_destruction_fx_b7(erase_x, erase_y, 1);
                                else nb_play_sound_erase_b7();
                                for(uint8_t j=0; j<size; j++) for(uint8_t i=0; i<size; i++) {
                                    ram_map[((uint16_t)(ry+j)*64)+rx+i] = TYPE_EMPTY;
                                    draw_tile(rx+i, ry+j);
                                }
                                game.money += cost;
                                // Mise à jour du registre
                                if (base_t == TILE_BARAQUE_NW) {
                                    if (baraque_count > 0) baraque_count--;
                                } else if (base_t == TYPE_ROAD || base_t == VAL_ROAD || base_t == TYPE_ROAD_VERTI) {
                                    // Recalcule la tile des routes voisines (la route supprimée n'est plus dans la RAM)
                                    if (rx > 0)  nb_update_road_display_b7(rx - 1, ry);
                                    if (rx < 63) nb_update_road_display_b7(rx + 1, ry);
                                    if (ry > 0)  nb_update_road_display_b7(rx, ry - 1);
                                    if (ry < 63) nb_update_road_display_b7(rx, ry + 1);
                                    flags_step = 0; // déclenche le refresh étalé
                                } else {
                                    nb_remove_building_b7(((uint16_t)ry * 64) + rx);
                                }
                            }
                            erase_done:;
                        }
                    } else if (current_tool == TOOL_UPGRADE) {
                        move_win(7, 144);
                        // Outil amélioration : cherche le bâtiment sous le curseur (economy.c = bank 3)
                        waitpadup(); // A relâché avant d'entrer dans le menu (appelé depuis bank 0)
                        // Cacher les sprites de voiture et de tram pendant le menu
                        hide_vehicle_sprites();
                        nb_disable_sram_b7();
                        {
                            uint8_t bldg_idx = nb_find_and_context_menu_b7();
                            if (bldg_idx == 0xFF) {
                                nb_play_sound_error_b7();
                            }
                        }
                        nb_update_hud_b7();
                        nb_enable_sram_b7();
                        nb_switch_ram_bank_b7(current_save_slot);
                    } else if (current_tool != TOOL_NONE) { // pose de bâtiment ==================
                        uint8_t size = nb_get_tool_size(current_tool);
                        

                        // LUT indexée par TOOL_* (ordre : ROAD=0,ERASE=1,HOUSE=2,MALL=3,FACTORY=4,FARM=5,
                        // PLANTATION=6,WOOD=7,POLICE=8,CHURCH=9,HOSPITAL=10,SCHOOL=11,POWER=12,MINE=13,
                        // SAWMILL=14,BAR=15,MEDIA=16,NONE=17,UPGRADE=18,ONETILEHOME=19)
                        static const uint16_t build_cost_lut[] = {
                            1, 0, 200, 1500, 700, 200, 150, 700, 1000, 600,
                            2500, 1200, 6000, 2500, 700, 500, 1000, 0, 0, 80
                        };
                        int32_t cost = (int32_t)build_cost_lut[current_tool];

                        
                        //===============================================================================================================
                        // logique de pose batiment =====================================================================================
                        //===============================================================================================================

                        uint8_t b_off = (size >= 3) ? 1 : 0;
                        uint8_t bx = curs_x - b_off;
                        uint8_t by = curs_y - b_off;
                        uint16_t bidx = ((uint16_t)by * WORLD_WIDTH) + bx;

                        uint8_t can_build = 1;
                        if (current_tool != TOOL_ROAD) {
                            for(uint8_t j=0; j<size; j++) for(uint8_t i=0; i<size; i++)
                                if(!IS_EMPTY(ram_map[bidx+i+(j*64)])) can_build = 0;
                        } else if(!IS_EMPTY(ram_map[bidx])) can_build = 0;

                        // pour mine, conserverie et mediadisco : on vérifie qu'on a de l'électricité sinon on peut pas build :
                        if ((current_tool == TOOL_MEDIA) || (current_tool == TOOL_MINE) || (current_tool == TOOL_WOOD)) {
                            if (game.electricity_cons >= game.electricity_prod) {
                                nb_trigger_alert_b7("MANQUE D'ELECTRICITE");
                                can_build = 0;
                            }
                        }

                        if (can_build && game.money >= cost) {
                            game.money -= cost;
                            game.monthly_expenses += cost;
                            game.exp_construction  += cost;
                            /*if (current_tool == TOOL_HOUSE) {
                            game.housing_capacity += 10;
                            }*/
                            nb_play_sound_build_b7();
                            //if (current_tool == TOOL_ROAD) { ram_map[idx] = TYPE_ROAD; draw_tile(curs_x, curs_y); }
                            //else { // on définit la tuile à afficher en fonction du sub tool sélectionné ===================
                                uint8_t base_tile = nb_get_tool_base_tile(current_tool);

                                // Compter les tiles 0xFA avant de les écraser (mine)
                                uint8_t ore_count = 0;
                                if (base_tile == TILE_MINE_NW) {
                                    uint8_t mi, mj;
                                    for (mj = 0; mj < 4; mj++)
                                        for (mi = 0; mi < 4; mi++)
                                            if (ram_map[bidx + mi + (uint16_t)mj * 64] == TILE_ORE) ore_count++;
                                }
                                for(uint8_t j=0; j<size; j++) for(uint8_t i=0; i<size; i++) {
                                    ram_map[bidx+i+(j*64)] = (base_tile == TILE_ONETILEHOME) ? TILE_ONETILEHOME : base_tile + (i+(j*size));
                                    draw_tile(bx+i, by+j);
                                }
                                nb_add_building_b7(bidx, base_tile);
                                if (ore_count >= 4)
                                    building_registry[building_count - 1].flags |= BLDG_FLAG_HAS_ORE;
                                //play_construction_fx(curs_x, curs_y, size);
                            //}
                        }
                    }

                    nb_disable_sram_b7();
                    if (game.money != money_before) hud_money_dirty = 1;

                }
            }

            //==============================================================================================
            // assignation du current_tool (la CATEGORIE)
            //==============================================================================================
            if (is_menu_open) {
                if (!is_selecting_sub) {
                    // NAVIGATION BAS (Catégories)
                    if (joy & J_RIGHT && current_tool < 5) { current_tool++; nb_update_sub_tool_icons_b7(); waitpadup(); }
                    if (joy & J_LEFT && current_tool > 0) { current_tool--; nb_update_sub_tool_icons_b7(); waitpadup(); }

                    // Monter dans le sous-menu
                    if (joy & J_A || joy & J_UP) {
                        is_selecting_sub = 1;
                        current_sub_tool = 0;

                        nb_force_cursor_update_b7();
                        waitpadup(); 
                    }

                    if (joy & J_B) { current_tool = TOOL_NONE; nb_hide_menu_b7(); move_win(7, 144); } // passe is_menu_open à 0 ; cache le hud bas
                } 
                else {

                    
                    // 1. DÉCLARATION EN PREMIER (impératif pour SDCC ? )
    uint8_t max_tools;
    uint8_t current_final_tool; // inutilisé ?
                    // NAVIGATION HAUT (Sous-outils)
                    max_tools = nb_get_sub_tool_count_b7(current_tool);
//==================================================================================================================
// assignation de current_sub_tool
//==================================================================================================================
                    if (joy & J_RIGHT && current_sub_tool < (max_tools - 1)) { current_sub_tool++; nb_force_cursor_update_b7(); waitpadup(); }
                    if (joy & J_LEFT && current_sub_tool > 0) { current_sub_tool--; nb_force_cursor_update_b7(); waitpadup(); }

                    // Redescendre aux catégories
                    if (joy & J_DOWN || joy & J_B) {
                        is_selecting_sub = 0;
                        // Cacher les sprites du haut quand on redescend ========================== ne fonctionne pas
                        for(uint8_t i=5; i<21; i++) move_sprite(i, 0, 0);
                        nb_force_cursor_update_b7();
                        waitpadup(); 
                    }

                    // Valider l'outil final
                    if (joy & (J_A | J_START)) {

                        // On récupère le bon outil final grâce au tableau et aux index actuels
                        current_tool = nb_get_building_tool_b7(current_tool, current_sub_tool);

                        is_selecting_sub = 0;
                        for(uint8_t i=0; i<21; i++) move_sprite(i, 0, 0); // Cache curseur + icônes HUD
                        nb_hide_menu_b7();
                        waitpadup();

                        // Clamp cursor to new tool's valid range
                        {
                            uint8_t new_sz = nb_get_tool_size(current_tool);
                            uint8_t new_off = (new_sz >= 3) ? 1 : 0;
                            uint8_t new_max_cx = (uint8_t)(WORLD_WIDTH  - new_sz + new_off);
                            uint8_t new_max_cy = (uint8_t)(WORLD_HEIGHT - 1 - new_sz + new_off);
                            if (curs_x < new_off) curs_x = new_off;
                            if (curs_x > new_max_cx) curs_x = new_max_cx;
                            if (curs_y < new_off) curs_y = new_off;
                            if (curs_y > new_max_cy) curs_y = new_max_cy;
                        }

                    }
                }
            }
            
            nb_update_game_logic_b7();
            nb_update_story_logic_b7();

            //update_building_alerts(cam_x, cam_y, power_ok);
            //uint8_t power_ok = (game.electricity_prod >= game.electricity_cons);

            scroll_camera_step();
            if (((joy & J_B) || ((current_tool == TOOL_ROAD || current_tool == TOOL_ERASE) && (joy & J_A))) && !is_menu_open) scroll_camera_step();
            if (!is_menu_open) nb_cars_update_b7(cam_x, cam_y);
            nb_tram_update_b7(cam_x, cam_y);
            nb_boat_update_b7(cam_x, cam_y);
            nb_plane_update_b3(cam_x, cam_y);
            nb_water_anim_update();
            if (hud_money_dirty) {
                hud_throttle++;
                if (hud_throttle >= 16u) {
                    hud_throttle = 0;
                    hud_money_dirty = 0;
                    if (!is_menu_open) {
                        uint8_t _row = 0u;
                        nb_draw_hud_money_date_b7(_row);
                    }
                }
            }
            if (flags_step >= 0) nb_refresh_flags_tick_b7();
            call_update_economy_tick();
            wait_vbl_done();
        }
    }
}