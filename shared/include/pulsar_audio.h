#pragma once
#include <pebble.h>

void pulsar_audio_init(void);
void pulsar_sound_start(bool audio, bool vibe);
void pulsar_sound_stop(bool audio, bool vibe);
void pulsar_sound_lap(bool audio, bool vibe);
void pulsar_sound_reset(bool audio, bool vibe);
void pulsar_sound_countdown_tick(int sec_remaining, bool audio, bool vibe);
void pulsar_sound_alarm_pulse(bool audio, bool vibe);
