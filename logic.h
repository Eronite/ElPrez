#ifndef LOGIC_H
#define LOGIC_H

#include <stdint.h>

void update_game_logic();
void update_story_logic();
void story_dialogue_animated(uint8_t portrait_id, char *text);
void logo_screen(uint8_t text_y);
void setup_story_params(uint8_t mission_id, uint8_t text_y);
void move_cursor(uint8_t x, uint8_t y, int16_t cx, int16_t cy);
void update_hud();
void draw_hud_money_date(uint8_t row);
void trigger_starvation_alert(void);
void trigger_alert(char* message);
void plot_building(uint8_t x, uint8_t y, uint8_t base_tile, uint8_t size);
//uint8_t spawn_homeless_shack(void);
void update_road_display(uint8_t wx, uint8_t wy);
void cars_init(void);
void cars_update(int16_t cam_x, int16_t cam_y);
void tram_init(void);
void tram_update(int16_t cam_x, int16_t cam_y);
extern uint8_t tram_needs_spawn;
void boat_init(void);
void boat_update(int16_t cam_x, int16_t cam_y);

extern uint8_t current_tool;
extern uint8_t fast_move;
extern uint8_t is_selecting_sub;
extern uint8_t current_sub_tool;
extern uint8_t current_step;

void nb_reload_limousine_b1(void);
void nb_reload_limo_tiles_b1(void);

// Wrapper ROM0 : lit mission1[game.current_step] depuis bank 2 dans *out.
// Utilisé par menus.c pour afficher les objectifs de l'étape courante.
#include "missions.h"
void nb_get_story_step(MissionStep *out);

typedef struct {
    uint8_t base_t;
    uint8_t size;
    int32_t cost;
} EraseInfo;

uint8_t get_tool_size(uint8_t tool);
uint8_t get_tool_base_tile(uint8_t tool);
uint8_t get_erase_info(uint8_t et, EraseInfo *out);

uint8_t get_building_size(uint8_t type);
uint8_t get_max_capacity(uint8_t type);
void    add_building(uint16_t map_idx, uint8_t type);
void    remove_building(uint16_t map_idx);
void    update_building_flags(uint8_t reg_idx);
uint8_t refresh_flags_tick(void);
extern int16_t flags_step;
void    rebuild_registry_from_map(void);


extern uint16_t alert_timer; // 0 = pas d'alerte (pour le flash électricité)
extern uint8_t cars_no_road; // 1 = pas de route trouvée, spawn bloqué

#endif