#include "pulsar_micro_bar.h"

void pulsar_draw_micro_beads(GContext *ctx, GRect bounds, const Colorway *palette, 
                             bool is_active, const bool beads_lit[NUM_MICRO_BEADS]) {
  int num_beads = NUM_MICRO_BEADS;
  int bead_spacing = bounds.size.w > 180 ? 10 : 7;
  int bead_radius = bounds.size.w > 180 ? 2 : 1;
  int total_bead_width = (num_beads - 1) * bead_spacing;
  int start_x = (bounds.size.w - total_bead_width) / 2;
  int bead_y = bounds.size.w > 180 ? 148 : 108;

  for (int i = 0; i < num_beads; i++) {
    bool lit = is_active && beads_lit[i];
    int bx = start_x + (i * bead_spacing);
    
#if defined(PBL_COLOR)
    if (palette->outer_bg.argb == GColorWhite.argb) {
      // Inverted paper mode
      if (lit) {
        graphics_context_set_fill_color(ctx, GColorBlack);
        graphics_fill_circle(ctx, GPoint(bx, bead_y), bead_radius);
      }
    } else {
      GColor bead_color = lit ? GColorWhite : palette->ghost;
      graphics_context_set_fill_color(ctx, bead_color);
      graphics_fill_circle(ctx, GPoint(bx, bead_y), lit ? bead_radius : 1);
    }
#else
    if (lit) {
      graphics_context_set_fill_color(ctx, palette->lit);
      graphics_fill_circle(ctx, GPoint(bx, bead_y), bead_radius);
    }
#endif
  }
}

void pulsar_draw_tachymeter_beads(GContext *ctx, GRect bounds, const Colorway *palette, 
                                  bool is_active, uint32_t elapsed_ms, bool is_running) {
  bool beads[NUM_MICRO_BEADS] = {false};
  if (is_running) {
    // 1970s LED Chaser / Tachymeter sweep (1-second full sweep across 10 beads)
    int phase = (elapsed_ms % 1000) / 100; // 0 to 9
    beads[phase] = true;
    if (phase > 0) beads[phase - 1] = true;
  }
  pulsar_draw_micro_beads(ctx, bounds, palette, is_active, beads);
}

void pulsar_draw_progress_beads(GContext *ctx, GRect bounds, const Colorway *palette, 
                                bool is_active, int remaining_sec, int total_sec) {
  bool beads[NUM_MICRO_BEADS] = {false};
  if (total_sec > 0 && remaining_sec > 0) {
    int lit_count = (remaining_sec * NUM_MICRO_BEADS + (total_sec / 2)) / total_sec;
    if (lit_count > NUM_MICRO_BEADS) lit_count = NUM_MICRO_BEADS;
    for (int i = 0; i < lit_count; i++) {
      beads[i] = true;
    }
  }
  pulsar_draw_micro_beads(ctx, bounds, palette, is_active, beads);
}
