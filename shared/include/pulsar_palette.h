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
  // AIDEV-NOTE: Hot core of a lit dot. GColorRed is already the palette's max red (#FF0000, luminance
  // ~0.21), so brightness has to come from a higher-luminance centre pixel, not a "brighter red".
  // Set equal to `lit` to disable the core (see COLORWAY_INVERTED_PAPER and all 1-bit platforms).
  GColor lit_core;
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

// AIDEV-NOTE: The 64-colour MiP panel builds colour from an RGB subpixel filter, so reflectance tracks how
// many subpixels are lit: GColorRed (R only) is ~1/3, yellow (R+G) ~2/3, white 3/3. Brightness and
// saturation are therefore in direct tension, and the hardware really only offers two useful positions.
// Measured off the framebuffer for ruby: Red renders #E35462 (luminance 0.236, 63% saturation) and Melon
// renders #EFB5B8 (0.549, 24%) -- 2.33x brighter. The intermediate SunsetOrange sat at just 1.29x, below
// the point where the change is noticeable on a dot this small, so it was dropped rather than shipped as
// a third position that does nothing. Global state, applied in pulsar_get_palette.
void pulsar_set_bright_leds(bool bright);
bool pulsar_bright_leds(void);

Colorway pulsar_get_palette(int colorway_index);

// Backlight tint matching a colourway, packed 0x00RRGGBB.
uint32_t pulsar_get_backlight_rgb(int colorway_index);

// AIDEV-NOTE: OPT-IN ONLY (default off). The watch has its own backlight-colour setting, so tinting is
// overriding a preference the wearer already made -- acceptable when they ask for it, not by default.
// Passing enabled=false restores their system colour rather than merely ceasing to update ours.
void pulsar_apply_backlight_tint(int colorway_index, bool enabled);

int pulsar_tuple_to_int(Tuple *tuple, int default_val);
bool pulsar_tuple_to_bool(Tuple *tuple, bool default_val);
