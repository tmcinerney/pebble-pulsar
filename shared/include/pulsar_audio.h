#pragma once
#include <pebble.h>

void pulsar_audio_init(void);
void pulsar_sound_start(void);
void pulsar_sound_stop(void);
void pulsar_sound_lap(void);
void pulsar_sound_reset(void);
void pulsar_sound_countdown_tick(int sec_remaining);
void pulsar_sound_alarm_pulse(void);
