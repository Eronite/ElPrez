#pragma bank 2
#include <gb/gb.h>
#include <stdint.h>
#include "minimap.h"
#include "constants.h"
#include "game_state.h"  // ram_map, game
#include "graphics.h"    // curs_x, curs_y

// ============================================================
// TUILES CUSTOM MINIMAP  (slots VRAM 250, 251, 252 — libres)
// 2bpp Game Boy : par paire d'octets (lo_plane, hi_plane)
// BGP=0xE4 : couleur 0=blanc, 1=gris clair, 2=gris foncé, 3=noir
// ============================================================

// Slot 250 — Bâtiment de ressource (ferme/plantation/scierie) : diamant en couleur 1
static const uint8_t mm_tile_resource[16] = {
    0x00,0x00,  // row 0
    0x18,0x00,  // row 1 : pointe haute (cols 3-4)
    0x3C,0x00,  // row 2 : (cols 2-5)
    0x7E,0x00,  // row 3 : (cols 1-6)
    0x7E,0x00,  // row 4 : (cols 1-6)
    0x3C,0x00,  // row 5 : (cols 2-5)
    0x18,0x00,  // row 6 : pointe basse (cols 3-4)
    0x00,0x00   // row 7
};

// Slot 251 — Bâtiment standard : carré plein 4×4 en couleur 2 (gris foncé)
static const uint8_t mm_tile_build[16] = {
    0x00,0x00,  // row 0
    0x00,0x00,  // row 1
    0x00,0x3C,  // row 2 : cols 2-5
    0x00,0x3C,  // row 3
    0x00,0x3C,  // row 4
    0x00,0x3C,  // row 5
    0x00,0x00,  // row 6
    0x00,0x00   // row 7
};

// Slot 252 — Bâtiment lourd (usine/centrale/mine) : cadre creux 6×6 en couleur 3 (noir)
static const uint8_t mm_tile_heavy[16] = {
    0x00,0x00,  // row 0
    0x7E,0x7E,  // row 1 : cols 1-6 plein
    0x42,0x42,  // row 2 : cols 1 et 6 seulement
    0x42,0x42,  // row 3
    0x42,0x42,  // row 4
    0x42,0x42,  // row 5
    0x7E,0x7E,  // row 6 : cols 1-6 plein
    0x00,0x00   // row 7
};

// Slot 112 — Eau : carré noir plein 8×8 en couleur 3
static const uint8_t mm_tile_water[16] = {
    0xFF,0xFF,
    0xFF,0xFF,
    0xFF,0xFF,
    0xFF,0xFF,
    0xFF,0xFF,
    0xFF,0xFF,
    0xFF,0xFF,
    0xFF,0xFF
};

#define MM_TILE_EMPTY    0          // tile 0 = blanc (déjà en VRAM)
#define MM_TILE_RESOURCE 250        // ferme / plantation / scierie
#define MM_TILE_BUILD    251        // bâtiments standards (maison, bar, école...)
#define MM_TILE_HEAVY    252        // bâtiments lourds (usine, hôpital, mine, centrale, mall)
#define MM_TILE_WATER    112        // carré noir plein (chargé en VRAM au démarrage minimap)

// Minimap : position dans la Window (centré sur 20 colonnes)
// Cols 2-17 (16 tuiles), Rows 1-16 (16 tuiles)
#define MM_WIN_COL  2
#define MM_WIN_ROW  1

// ============================================================
// Classification d'une tile de ram_map pour la minimap
// Routes non affichées (traité comme vide)
// ============================================================
static uint8_t mm_classify(uint8_t t) {
    // Vide et routes → non affichés
    if (IS_EMPTY(t)) return MM_TILE_EMPTY;
    if (t == VAL_ROAD) return MM_TILE_EMPTY;

    // Bâtiments de ressource (ferme=160-168, plantation=169-172, scierie=173-188)
    if ((t >= 160 && t <= 168) ||
        (t >= 169 && t <= 172) ||
        (t >= 173 && t <= 188))
        return MM_TILE_RESOURCE;

    // Bâtiments lourds (mall=37-52, usine=144-159, hôpital=202-217, centrale=218-233, mine=234-249)
    if ((t >= 37  && t <= 52)  ||
        (t >= 144 && t <= 159) ||
        (t >= 202 && t <= 217) ||
        (t >= 218 && t <= 233) ||
        (t >= 234 && t <= 249))
        return MM_TILE_HEAVY;

    // Bâtiments standards : maison=135-143, bar=129-134, police=0xBD-0xC0,
    //   église=0xC1-0xC9, école=0x3D-0x40, disco=0x59-0x62, baraque=0x63-0x66,
    //   onetilehome=0xFF
    // Note : port/scierie (0x76-0x78) omis car collision avec tiles côte water map
    if ((t >= 129 && t <= 143) ||
        (t >= 0x3D && t <= 0x40) ||
        (t >= 0x59 && t <= 0x62) ||
        (t >= 0x63 && t <= 0x66) ||
        (t >= 0xBD && t <= 0xC9) ||
        t == 0xFF)
        return MM_TILE_BUILD;

    // Tiles eau/côte (0x6D-0x78) → noir
    if (t >= 0x6D && t <= 0x78) return MM_TILE_WATER;

    // Tout le reste (transitions, décor) → vide
    return MM_TILE_EMPTY;
}

// ============================================================
// show_minimap() — affichage complet
// ============================================================
void show_minimap(void) {
    // 1. Charger les 3 tuiles custom en VRAM (slots 250-252)
    //    MM_TILE_WATER (0x6D) est déjà en VRAM, pas besoin de le recharger
    set_bkg_data(MM_TILE_RESOURCE, 1, mm_tile_resource);
    set_bkg_data(MM_TILE_BUILD,    1, mm_tile_build);
    set_bkg_data(MM_TILE_HEAVY,    1, mm_tile_heavy);
    set_bkg_data(MM_TILE_WATER,    1, mm_tile_water);

    // 2. Remplir la Window avec la tile eau (bordure autour de la minimap)
    {
        uint8_t water_row[20];
        uint8_t i;
        for (i = 0; i < 20; i++) water_row[i] = MM_TILE_WATER;
        for (i = 0; i < 18; i++) set_win_tiles(0, i, 20, 1, water_row);
    }

    // 3. Construction de la minimap 16×16
    // Chaque tuile Window = 4×4 tiles de ram_map (64/16 = 4)
    for (uint8_t wy = 0; wy < 16; wy++) {
        for (uint8_t wx = 0; wx < 16; wx++) {
            uint8_t best = MM_TILE_EMPTY;

            // Sonde les 4×4 tiles de la map pour cette cellule
            for (uint8_t dy = 0; dy < 4 && best != MM_TILE_HEAVY; dy++) {
                for (uint8_t dx = 0; dx < 4 && best != MM_TILE_HEAVY; dx++) {
                    uint8_t t = ram_map[(uint16_t)(wy * 4 + dy) * 64 + (wx * 4 + dx)];
                    uint8_t cls = mm_classify(t);
                    if (cls == MM_TILE_HEAVY)                                   { best = MM_TILE_HEAVY; break; }
                    if (cls == MM_TILE_BUILD     && best != MM_TILE_HEAVY)        best = MM_TILE_BUILD;
                    else if (cls == MM_TILE_RESOURCE && best == MM_TILE_EMPTY)    best = MM_TILE_RESOURCE;
                    else if (cls == MM_TILE_WATER    && best == MM_TILE_EMPTY)    best = MM_TILE_WATER;
                }
            }

            set_win_tiles(MM_WIN_COL + wx, MM_WIN_ROW + wy, 1, 1, &best);
        }
    }

    // 4. Position du curseur joueur sur la minimap (sprite 0, clignotant)
    uint8_t sp_x = (uint8_t)((MM_WIN_COL + curs_x / 4) * 8 + 8);
    uint8_t sp_y = (uint8_t)((MM_WIN_ROW + curs_y / 4) * 8 + 16);

    // 5. Afficher (Window couvre tout l'écran)
    move_sprite(0, 0, 0);
    SHOW_WIN;
    move_win(7, 0);
    BGP_REG = 0xE4;

    // 6. Boucle d'attente — B pour quitter
    uint8_t blink = 0;
    waitpadup();
    while (!(joypad() & J_B)) {
        blink++;
        if ((blink / 15) % 2 == 0) move_sprite(0, sp_x, sp_y);
        else                        move_sprite(0, 0, 0);
        wait_vbl_done();
    }
    move_sprite(0, 0, 0);
    waitpadup();

    // 7. Nettoyage avant retour (l'appelant gère move_win + update_hud)
    uint8_t clr[20] = {0};
    for (uint8_t r = 0; r < 18; r++) set_win_tiles(0, r, 20, 1, clr);
}
