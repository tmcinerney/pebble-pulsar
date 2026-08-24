#pragma once
#include <pebble.h>
#include "pulsar_palette.h"
#include "pulsar_layout.h"

void pulsar_draw_micro_beads_at_y(GContext *ctx, GRect bounds, int bead_y, const Colorway *palette, 
                                 bool is_active, const bool beads_lit[NUM_MICRO_BEADS]);

void pulsar_draw_micro_beads(GContext *ctx, GRect bounds, const Colorway *palette, 
                             bool is_active, const bool beads_lit[NUM_MICRO_BEADS]);

void pulsar_draw_tachymeter_beads_at_y(GContext *ctx, GRect bounds, int bead_y, const Colorway *palette, 
                                      bool is_active, uint32_t elapsed_ms, bool is_running);

void pulsar_draw_tachymeter_beads(GContext *ctx, GRect bounds, const Colorway *palette, 
                                  bool is_active, uint32_t elapsed_ms, bool is_running);

void pulsar_draw_progress_beads(GContext *ctx, GRect bounds, const Colorway *palette, 
                                bool is_active, int remaining_sec, int total_sec);
