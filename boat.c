#pragma bank 1
#include <gb/gb.h>
#include <stdint.h>
#include "boat.h"
#include "constants.h"

extern uint8_t is_menu_open;
extern uint8_t current_tool;

extern void nb_reload_boat_tiles_b1(uint8_t fwd);

#define BOAT_SPR_BASE  29u
#define BOAT_TILE_0    0x7Au
#define BOAT_TILE_1    0x7Bu
#define BOAT_TILE_2    0x7Cu
#define BOAT_TILE_3    0x7Eu

#define BOAT_START_X   30
#define BOAT_START_Y   65
#define BOAT_END_X     48
#define BOAT_END_Y     47
#define BOAT_PX_STEP   6u
#define BOAT_PAUSE_DUR 600u

static int16_t  boat_px;
static int16_t  boat_py;
static uint16_t boat_pause_timer;
static uint8_t  boat_going_fwd;
static uint8_t  boat_px_timer;
uint8_t         boat_has_port = 0;

void boat_init(void) {
    boat_px          = (int16_t)BOAT_START_X * 8;
    boat_py          = (int16_t)BOAT_START_Y * 8;
    boat_going_fwd   = 1;
    boat_pause_timer = 0;
    boat_px_timer    = 0;
    nb_reload_boat_tiles_b1(1);
}

void boat_update(int16_t cam_x, int16_t cam_y) {
    if (!boat_has_port) {
        move_sprite(BOAT_SPR_BASE,   0u, 0u);
        move_sprite(BOAT_SPR_BASE+1, 0u, 0u);
        move_sprite(BOAT_SPR_BASE+2, 0u, 0u);
        move_sprite(BOAT_SPR_BASE+3, 0u, 0u);
        return;
    }
    // --- MOUVEMENT ---
    if (boat_pause_timer > 0) {
        boat_pause_timer--;
        if (boat_pause_timer == 0) nb_reload_boat_tiles_b1(boat_going_fwd);
    } else {
        boat_px_timer++;
        if (boat_px_timer >= BOAT_PX_STEP) {
            boat_px_timer = 0;
            int8_t dx = boat_going_fwd ? 1 : -1;
            int8_t dy = boat_going_fwd ? -1 : 1;
            boat_px += dx;
            boat_py += dy;
            if (boat_going_fwd) {
                if (boat_px >= (int16_t)BOAT_END_X * 8 && boat_py <= (int16_t)BOAT_END_Y * 8) {
                    boat_px          = (int16_t)BOAT_END_X * 8;
                    boat_py          = (int16_t)BOAT_END_Y * 8;
                    boat_pause_timer = BOAT_PAUSE_DUR;
                    boat_going_fwd   = 0;
                }
            } else {
                if (boat_px <= (int16_t)BOAT_START_X * 8 && boat_py >= (int16_t)BOAT_START_Y * 8) {
                    boat_px          = (int16_t)BOAT_START_X * 8;
                    boat_py          = (int16_t)BOAT_START_Y * 8;
                    boat_pause_timer = BOAT_PAUSE_DUR;
                    boat_going_fwd   = 1;
                }
            }
        }
    }

    // --- AFFICHAGE ---
    if (is_menu_open) {
        move_sprite(BOAT_SPR_BASE,   0u, 0u);
        move_sprite(BOAT_SPR_BASE+1, 0u, 0u);
        move_sprite(BOAT_SPR_BASE+2, 0u, 0u);
        move_sprite(BOAT_SPR_BASE+3, 0u, 0u);
    } else {
        uint8_t hud_oam_y = (current_tool < 17u || current_tool == TOOL_ONETILEHOME) ? 144u : 152u;

        int16_t sx  = boat_px - cam_x + 8;
        int16_t sy  = boat_py - cam_y + 16;
        int16_t sx1 = sx + 8;
        int16_t sy2 = sy + 8;

        // Clip tout le métasprite d'un coup
        if (sx < 0 || sx1 > 167 || sy < 8 || sy2 >= (int16_t)hud_oam_y) {
            move_sprite(BOAT_SPR_BASE,   0u, 0u);
            move_sprite(BOAT_SPR_BASE+1, 0u, 0u);
            move_sprite(BOAT_SPR_BASE+2, 0u, 0u);
            move_sprite(BOAT_SPR_BASE+3, 0u, 0u);
        } else {
            set_sprite_tile(BOAT_SPR_BASE,   BOAT_TILE_0); move_sprite(BOAT_SPR_BASE,   (uint8_t)sx,  (uint8_t)sy);
            set_sprite_tile(BOAT_SPR_BASE+1, BOAT_TILE_1); move_sprite(BOAT_SPR_BASE+1, (uint8_t)sx1, (uint8_t)sy);
            set_sprite_tile(BOAT_SPR_BASE+2, BOAT_TILE_2); move_sprite(BOAT_SPR_BASE+2, (uint8_t)sx,  (uint8_t)sy2);
            set_sprite_tile(BOAT_SPR_BASE+3, BOAT_TILE_3); move_sprite(BOAT_SPR_BASE+3, (uint8_t)sx1, (uint8_t)sy2);
        }
    }
}
