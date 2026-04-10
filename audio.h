#ifndef AUDIO_H
#define AUDIO_H
void play_sound_build();
void play_sound_erase();
void play_sound_alert();
void play_sound_mission_success();
void play_sound_dialogue_next();
void play_destruction_fx(uint8_t x, uint8_t y, uint8_t size);
void play_engine_sound(void);
void stop_engine_sound(void);
void play_sound_error();

extern int16_t cam_x;
extern int16_t cam_y;

#endif