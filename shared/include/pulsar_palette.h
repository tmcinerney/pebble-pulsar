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
  GColor ghost;
  GColor accent;
  GColor outer_bg;
  GColor inner_bg;
  GColor text_outer;
} Colorway;

Colorway pulsar_get_palette(int colorway_index);

int pulsar_tuple_to_int(Tuple *tuple, int default_val);
bool pulsar_tuple_to_bool(Tuple *tuple, bool default_val);
