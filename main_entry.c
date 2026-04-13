// main_entry.c — PAS de #pragma bank → bank 0 (entrypoint fixe)
#include <gb/gb.h>

void game_main(void);

void main(void) {
    SWITCH_ROM(7);
    game_main();
}
