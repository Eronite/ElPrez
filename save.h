#ifndef SAVE_H
#define SAVE_H
#include <stdint.h>

// Fonctions en bank 2 (appel direct depuis bank 2 uniquement)
uint8_t load_saves_screen();
void save_story_game();
uint8_t confirm_delete(uint8_t slot_idx);
void wipe_slot_by_index(uint8_t slot);
void load_story_game(uint8_t bank_index);

// Wrappers nonbanked (bank 0) pour appels depuis bank 1
uint8_t nb_load_saves_screen(void);
void nb_load_story_game(uint8_t bank_index);

#endif