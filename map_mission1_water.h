#ifndef MAP_MISSION1_WATER_H
#define MAP_MISSION1_WATER_H

#include <stdint.h>

// Carte eau mission 1 - generee depuis assets/waterMap.c
// Encodage: 0x6D=eau, 0x6E=bord_N, 0x6F=bord_S, 0x70=bord_W, 0x71=bord_E, 0x80=herbe
// Tableau defini en bank 4 (map_mission1_water.c)
extern const uint8_t map_mission1_water[4096];

#endif
