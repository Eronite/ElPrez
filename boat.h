#ifndef BOAT_H
#define BOAT_H
#include <stdint.h>
void boat_init(void);
void boat_update(int16_t cam_x, int16_t cam_y);
extern uint8_t boat_has_port;
#endif
