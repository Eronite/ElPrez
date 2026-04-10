#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdint.h>
#include "missions.h"

void draw_text(uint8_t x, uint8_t y, char *str, uint8_t target_win);
// Charge les tuiles minuscules a-z (font_ibm) dans VRAM 128-153.
// À appeler à l'entrée de tout menu plein écran / dialogue.
void load_lowercase_font(void);
// Restaure les tuiles de carte dans VRAM 128-153 (après load_lowercase_font).
void restore_map_tiles(void);
void draw_number(uint8_t x, uint8_t y, int32_t value, uint8_t target);
void clear_entire_window();
void fade_out();
void fade_in();
void draw_penultimo_large(uint8_t x, uint8_t y);
void animate_penultimo_jaw_bouncing(uint8_t open, uint8_t x, uint8_t y);
void draw_tile(uint8_t tx, uint8_t ty);
void update_view();
void draw_text_animated(uint8_t x, uint8_t y, char *str, uint8_t target_win, uint8_t delay_frames);
void load_penultimo_large(void);
void load_portrait(uint8_t portrait_id);
void show_menu(void);
void hide_menu(void);
void hide_vehicle_sprites(void);

#define TILE_HUD_TICK  0x7Du  // tick/coche objectif rempli (page Mission)

#define MENU_TILE_START  156u  // premier slot VRAM des tiles de bordure menu (156-163)
#define MENU_TILE_COUNT    8u
void load_menu_tiles(void);
void nb_draw_menu_border(void);
void draw_presidente_bkg(uint8_t row1);
void restore_shop_tiles(void);
//void init_flash_pool();
//void update_building_alerts(int camera_x, int camera_y, uint8_t power_ok);


extern int16_t cam_x, cam_y;
extern int16_t cam_target_x, cam_target_y;
void scroll_camera_step(void);
extern uint8_t curs_x, curs_y;
extern uint8_t is_menu_open;

// Nombre max d'éclairs affichables simultanément
//#define MAX_VISIBLE_FLASHES 5

// Structure pour suivre l'utilisation des sprites
typedef struct {
    uint8_t sprite_id;
    uint8_t is_active;
} FlashSprite;

//extern FlashSprite flash_pool[MAX_VISIBLE_FLASHES];

#endif