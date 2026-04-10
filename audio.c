#pragma bank 1
#include <gb/gb.h>
#include "audio.h"

void play_sound_build() { NR41_REG = 0x01; NR42_REG = 0x91; NR43_REG = 0x43; NR44_REG = 0x80; }
void play_sound_erase() { NR41_REG = 0x1F; NR42_REG = 0xF1; NR43_REG = 0x54; NR44_REG = 0x80; }
void play_sound_alert() { NR51_REG |= 0x22; NR21_REG = 0x81; NR22_REG = 0xA2; NR23_REG = 0x44; NR24_REG = 0x87; }
void play_sound_mission_success() { NR10_REG = 0x15; NR11_REG = 0x81; NR12_REG = 0x43; NR13_REG = 0x73; NR14_REG = 0x86; }
void play_sound_dialogue_next() { NR10_REG = 0x15; NR11_REG = 0x81; NR12_REG = 0x51; NR13_REG = 0x80; NR14_REG = 0x86; }

int16_t cam_x = 0;
int16_t cam_y = 0;
int16_t cam_target_x = 0;
int16_t cam_target_y = 0;

void play_destruction_fx(uint8_t x, uint8_t y, uint8_t size) {
    uint8_t i;
    uint8_t sx = (uint8_t)(x * 8 - cam_x + 8);
    uint8_t sy = (uint8_t)(y * 8 - cam_y + 16);
    uint8_t sz = (size - 1) * 8;
    //play_sound_build();
    play_sound_erase();
    for(i = 0; i < 8; i++) {
        if (i % 2 == 0) SCX_REG += 1; else SCX_REG -= 1;
        if (i % 2 == 0) {
            move_sprite(1, sx, sy); move_sprite(2, sx + sz, sy);
            move_sprite(3, sx, sy + sz); move_sprite(4, sx + sz, sy + sz);
        } else {
            for(uint8_t j=1; j<5; j++) move_sprite(j, 0, 0);
        }
        wait_vbl_done();
    }
    SCX_REG = (uint8_t)cam_x;
}

void play_sound_error() {
    NR52_REG = 0x80; // Assure que le son est ON
    NR51_REG |= 0x11; // Canal 1
    NR10_REG = 0x00; // Pas de sweep
    NR11_REG = 0x81; // Duty cycle
    NR12_REG = 0x41; // Volume moyen, décrue rapide
    NR13_REG = 0x20; // Fréquence basse
    NR14_REG = 0x82; // Trigger
}

void play_engine_sound(void) {
    NR41_REG = 0x00; NR42_REG = 0x41; NR43_REG = 0x72; NR44_REG = 0x80;
}
void stop_engine_sound(void) {
    NR42_REG = 0x00; NR44_REG = 0x80;
}