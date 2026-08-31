#pragma once
#include <pebble.h>

/**
 * Authentic 1970s Hamilton Pulsar / Retro Space-Age Colorways
 */
enum ColorwayId {
  COLORWAY_VIBRANT_RUBY = 0,
  COLORWAY_DEEP_RED = 1,
  COLORWAY_PROTOTYPE_GREEN = 2,
  COLORWAY_AMBER_GOLD = 3,
  COLORWAY_COBALT_BLUE = 4,
  COLORWAY_LUNAR_WHITE = 5,
  COLORWAY_INVERTED_PAPER = 6
};
#define NUM_COLORWAYS 7

typedef struct {
  GColor lit;
  // AIDEV-NOTE: Halo drawn one pixel proud of each lit dot to fake LED/neon bloom. The panel's red
  // primary reproduces as ~#E35462 (measured), so saturation cannot be increased by colour choice --
  // the glow gradient is what actually reads as "glowing neon" rather than a flat disc.
  GColor glow;
  GColor ghost;
  GColor accent;
  GColor outer_bg;
  GColor inner_bg;
  GColor text_outer;
} Colorway;

Colorway pulsar_get_palette(int colorway_index);

// Backlight tint matching a colourway, packed 0x00RRGGBB.
uint32_t pulsar_get_backlight_rgb(int colorway_index);

// AIDEV-NOTE: OPT-IN ONLY (default off). The watch has its own backlight-colour setting, so tinting is
// overriding a preference the wearer already made -- acceptable when they ask for it, not by default.
// Passing enabled=false restores their system colour rather than merely ceasing to update ours.
void pulsar_apply_backlight_tint(int colorway_index, bool enabled);

int pulsar_tuple_to_int(Tuple *tuple, int default_val);
bool pulsar_tuple_to_bool(Tuple *tuple, bool default_val);
