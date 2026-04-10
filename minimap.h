#ifndef MINIMAP_H
#define MINIMAP_H

#include <stdint.h>

// Affiche la minimap plein écran (bank 2).
// Appelé via call_show_minimap() (wrapper nonbanked dans main.c).
void show_minimap(void);

// Wrapper nonbanked pour appeler show_minimap depuis bank 1 (menus.c).
void call_show_minimap(void);

#endif
