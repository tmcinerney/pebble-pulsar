#pragma once
#include <pebble.h>

#if defined(PBL_PLATFORM_EMERY)
  #define DOT_RADIUS        2
  #define DOT_SPACING_X     7
  #define DOT_SPACING_Y     8
  #define DIGIT_GAP         14
  #define COLON_GAP         22
  #define TOP_MARGIN        70
  #define INDICATOR_RADIUS  2
  #define BEAD_SPACING      10
  #define BEAD_RADIUS       2
  #define BEAD_Y            148
  #define HEADER_Y          14
  #define FOOTER_Y          192
  #define SLANT_SCALE       7
#else // Basalt, Diorite, Aplite (144x168)
  #define DOT_RADIUS        1
  #define DOT_SPACING_X     5
  #define DOT_SPACING_Y     6
  #define DIGIT_GAP         10
  #define COLON_GAP         16
  #define TOP_MARGIN        48
  #define INDICATOR_RADIUS  1
  #define BEAD_SPACING      7
  #define BEAD_RADIUS       1
  #define BEAD_Y            108
  #define HEADER_Y          8
  #define FOOTER_Y          142
  #define SLANT_SCALE       5
#endif

#define DIGIT_WIDTH         5
#define DIGIT_HEIGHT        7
#define NUM_MICRO_BEADS     10
