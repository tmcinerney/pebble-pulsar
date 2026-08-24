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

int pulsar_tuple_to_int(Tuple *tuple, int default_val) {
  if (!tuple) return default_val;
  switch (tuple->type) {
    case TUPLE_INT:
      if (tuple->length == 1) return (int)tuple->value->int8;
      if (tuple->length == 2) return (int)tuple->value->int16;
      if (tuple->length == 4) return (int)tuple->value->int32;
      return (int)tuple->value->int32;
    case TUPLE_UINT:
      if (tuple->length == 1) return (int)tuple->value->uint8;
      if (tuple->length == 2) return (int)tuple->value->uint16;
      if (tuple->length == 4) return (int)tuple->value->uint32;
      return (int)tuple->value->uint32;
    case TUPLE_CSTRING:
      if (tuple->length > 0) {
        return atoi(tuple->value->cstring);
      }
      break;
    default:
      break;
  }
  return default_val;
}

bool pulsar_tuple_to_bool(Tuple *tuple, bool default_val) {
  if (!tuple) return default_val;
  switch (tuple->type) {
    case TUPLE_INT:
    case TUPLE_UINT:
      return pulsar_tuple_to_int(tuple, default_val ? 1 : 0) != 0;
    case TUPLE_CSTRING:
      if (tuple->length > 0) {
        return (strcmp(tuple->value->cstring, "true") == 0 ||
                strcmp(tuple->value->cstring, "1") == 0 ||
                strcmp(tuple->value->cstring, "yes") == 0);
      }
      break;
    default:
      break;
  }
  return default_val;
}
