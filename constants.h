#ifndef CONSTANTS_H
#define CONSTANTS_H

#define WORLD_WIDTH 64
#define WORLD_HEIGHT 64

// --- ESPACE CARTE (4096 octets) ---
#define SRAM_MAP_PTR       ((uint8_t *)0xA000)

// GRAPHISMES (VRAM)
#define TYPE_EMPTY 128
#define TYPE_EMPTY2 0   // tile constructible (herbe, issu de waterMap2 0x06)
#define TILE_ORE     0xFA  // gisement de minerai (constructible)
#define IS_EMPTY(t) ((t) == TYPE_EMPTY || (t) == TYPE_EMPTY2 || (t) == TILE_ORE)
#define TYPE_ROAD  133
#define TYPE_ROAD_VERTI  134
#define TILE_FARM_NW 160 
#define TYPE_MALL_NW 0x25
#define TYPE_FACTORY_NW 0x90  
//#define TYPE_MALL_NW 144
//#define TYPE_BANK2_NW 37 
#define TILE_HOUSE_NW 135
//#define TILE_POLICE_NW 129
#define TILE_PLANTATION_NW 0xA9
#define TILE_WOOD_NW 0xAD
#define TILE_POLICE_NW 0xBD
#define TILE_CHURCH_NW 0xC1
#define TILE_HOSPITAL_NW 0xCA
#define TILE_SCHOOL_NW 0x3D
#define TILE_POWER_NW 0xDA
#define TILE_MINE_NW 0xEA
#define TILE_BAR_NW 129
#define TILE_MEDIADISCO_NW 0x59
#define TILE_BARAQUE_NW 0x63
#define TILE_PORT_SAWMILL_NW 0x76
#define TILE_PORT_NW         0xFB  // port (2x2, tiles 0xFB-0xFE)

// Tiles eau/mer (VRAM BG 0x6D–0x75, chargées via set_bkg_data(0x6D, 9, water))
// Ordre = ordre du tableau water[] dans assets.c
#define TILE_WATER    0x6Du  // eau pleine
#define TILE_WATER_N  0x6Eu  // bord nord  (eau en bas,   terre en haut)
#define TILE_WATER_S  0x6Fu  // bord sud   (eau en haut,  terre en bas)
#define TILE_WATER_W  0x70u  // bord ouest (eau à droite, terre à gauche)
#define TILE_WATER_E  0x71u  // bord est   (eau à gauche, terre à droite)
#define TILE_WATER_NW 0x72u  // coin NW    (terre haut-gauche, eau bas-droite)
#define TILE_WATER_NE 0x73u  // coin NE    (terre haut-droite, eau bas-gauche)
#define TILE_WATER_SW 0x74u  // coin SW    (terre bas-gauche,  eau haut-droite)
#define TILE_WATER_SE 0x75u  // coin SE    (terre bas-droite,  eau haut-gauche)

#define TYPE_ROAD_BASE 0x67  // première tuile de route
#define VAL_ROAD       1     // Identifiant "Route" dans la ram_map

// OUTILS
/*#define TOOL_ROAD 0
#define TOOL_FARM 1
#define TOOL_FACTORY 2
#define TOOL_MALL 3
#define TOOL_BANK2 4
#define TOOL_ERASE 5*/


#define TOOL_ROAD 0
#define TOOL_ERASE 1
#define TOOL_HOUSE 2
#define TOOL_MALL 3
#define TOOL_FACTORY 4
#define TOOL_FARM 5
#define TOOL_PLANTATION 6
#define TOOL_WOOD 7
#define TOOL_POLICE 8
#define TOOL_CHURCH 9
#define TOOL_HOSPITAL 10
#define TOOL_SCHOOL 11
#define TOOL_POWER 12
#define TOOL_MINE 13
#define TOOL_SAWMILL 14
#define TOOL_BAR 15
#define TOOL_MEDIA 16
#define TOOL_NONE    17  // Aucun outil sélectionné (après fermeture HUD avec B)
#define TOOL_UPGRADE 18  // Outil amélioration bâtiment
#define TOOL_ONETILEHOME 19  // Logement simple (2x2, tile unique 0xFF)

#define TILE_ONETILEHOME 0xFF  // logement simple (2x2, toutes les tuiles = 0xFF)


#define PENULTIMO_TILE_START 9
#define REVENUE_INTERVAL 600
#define MONTH_INTERVAL 1500

#define GROWTH_THRESHOLD 4 // Il faut 4 "points" pour un nouvel habitant

#endif