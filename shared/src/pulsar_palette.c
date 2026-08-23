#include "pulsar_palette.h"

Colorway pulsar_get_palette(int colorway_index) {
  Colorway p;
#if defined(PBL_COLOR)
  p.outer_bg = GColorBlack;
  p.inner_bg = GColorBlack;
  p.text_outer = GColorWhite;
  
  switch (colorway_index) {
    case COLORWAY_DEEP_RED:
      // Hot Lava Orange (Vintage LED)
      p.lit = GColorOrange;
      p.ghost = GColorBulgarianRose;
      p.accent = GColorSunsetOrange;
      break;
    case COLORWAY_PROTOTYPE_GREEN:
      // Authentic 1975 GaP Phosphor Green
      p.lit = GColorBrightGreen;
      p.ghost = GColorDarkGreen;
      p.accent = GColorSpringBud;
      break;
    case COLORWAY_AMBER_GOLD:
      // Amber Gold (HP-01 Space-Age LED)
      p.lit = GColorYellow;
      p.ghost = GColorArmyGreen;
      p.accent = GColorChromeYellow;
      break;
    case COLORWAY_COBALT_BLUE:
      // Electric Cyan / Blue (Radiant on Reflective LCD)
      p.lit = GColorCyan;
      p.ghost = GColorMidnightGreen;
      p.accent = GColorVividCerulean;
      break;
    case COLORWAY_LUNAR_WHITE:
      // Lunar White / Silver
      p.lit = GColorWhite;
      p.ghost = GColorDarkGray;
      p.accent = GColorLightGray;
      break;
    case COLORWAY_INVERTED_PAPER:
      // Inverted E-Paper
      p.outer_bg = GColorWhite;
      p.inner_bg = GColorWhite;
      p.text_outer = GColorBlack;
      p.lit = GColorBlack;
      p.ghost = GColorLightGray;
      p.accent = GColorDarkGray;
      break;
    case COLORWAY_VIBRANT_RUBY:
    default:
      // Classic 1972 GaAsP Ruby Red
      p.lit = GColorRed;
      p.ghost = GColorBulgarianRose;
      p.accent = GColorRed;
      break;
  }
#else
  if (colorway_index == COLORWAY_INVERTED_PAPER) {
    p.outer_bg = GColorWhite;
    p.inner_bg = GColorWhite;
    p.text_outer = GColorBlack;
    p.lit = GColorBlack;
    p.ghost = GColorWhite;
    p.accent = GColorBlack;
  } else {
    p.lit = GColorWhite;
    p.ghost = GColorBlack;
    p.accent = GColorWhite;
    p.outer_bg = GColorBlack;
    p.inner_bg = GColorBlack;
    p.text_outer = GColorWhite;
  }
#endif
  return p;
}
