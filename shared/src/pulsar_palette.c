#include "pulsar_palette.h"

static bool s_bright_leds = false;

void pulsar_set_bright_leds(bool bright) {
  s_bright_leds = bright;
}

bool pulsar_bright_leds(void) {
  return s_bright_leds;
}

#if defined(PBL_COLOR)
// Truest hue, then the brightest usable variant of the same hue.
static void apply_ramp(Colorway *p, GColor classic, GColor c0, GColor bright, GColor c1) {
  p->lit      = s_bright_leds ? bright : classic;
  p->lit_core = s_bright_leds ? c1 : c0;
}

// Mid-tone between the lit colour and the background, used for the bloom halo.
static void set_glow(Colorway *p, GColor glow) {
  p->glow = glow;
}
#endif

Colorway pulsar_get_palette(int colorway_index) {
  Colorway p;
#if defined(PBL_COLOR)
  p.outer_bg = GColorBlack;
  p.inner_bg = GColorBlack;
  p.text_outer = GColorWhite;
  
  switch (colorway_index) {
    case COLORWAY_DEEP_RED:
      // Hot Lava Orange (Vintage LED)
      apply_ramp(&p, GColorOrange, GColorOrange,
                     GColorPastelYellow, GColorPastelYellow);
      set_glow(&p, GColorWindsorTan);
      p.ghost = GColorBulgarianRose;
      p.accent = GColorSunsetOrange;
      break;
    case COLORWAY_PROTOTYPE_GREEN:
      // Authentic 1975 GaP Phosphor Green
      apply_ramp(&p, GColorBrightGreen,   GColorBrightGreen,
                     GColorMintGreen,     GColorMintGreen);
      set_glow(&p, GColorIslamicGreen);
      p.ghost = GColorDarkGreen;
      p.accent = GColorSpringBud;
      break;
    case COLORWAY_AMBER_GOLD:
      // Amber Gold (HP-01 Space-Age LED)
      apply_ramp(&p, GColorYellow,       GColorYellow,
                     GColorPastelYellow, GColorPastelYellow);
      set_glow(&p, GColorLimerick);
      p.ghost = GColorArmyGreen;
      p.accent = GColorChromeYellow;
      break;
    case COLORWAY_COBALT_BLUE:
      // Electric Cyan / Blue (Radiant on Reflective LCD)
      apply_ramp(&p, GColorCyan,    GColorCyan,
                     GColorWhite,   GColorWhite);
      set_glow(&p, GColorTiffanyBlue);
      p.ghost = GColorMidnightGreen;
      p.accent = GColorVividCerulean;
      break;
    case COLORWAY_LUNAR_WHITE:
      // Lunar White / Silver
      p.lit = GColorWhite;
      p.lit_core = GColorWhite;       // already max luminance
      set_glow(&p, GColorLightGray);
      p.ghost = GColorDarkGray;
      p.accent = GColorLightGray;
      break;
    case COLORWAY_INVERTED_PAPER:
      // Inverted E-Paper
      p.outer_bg = GColorWhite;
      p.inner_bg = GColorWhite;
      p.text_outer = GColorBlack;
      p.lit = GColorBlack;
      p.lit_core = GColorBlack;       // no core: dark-on-light colourway
      set_glow(&p, GColorLightGray);
      p.ghost = GColorLightGray;
      p.accent = GColorDarkGray;
      break;
    case COLORWAY_VIBRANT_RUBY:
    default:
      // Classic 1972 GaAsP Ruby Red
      apply_ramp(&p, GColorRed,          GColorRed,
                     GColorMelon,        GColorMelon);
      set_glow(&p, GColorDarkCandyAppleRed);
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
    p.lit_core = GColorBlack;
    p.glow = GColorBlack;
    p.ghost = GColorWhite;
    p.accent = GColorBlack;
  } else {
    p.lit = GColorWhite;
    p.lit_core = GColorWhite;
    p.glow = GColorWhite;
    p.ghost = GColorBlack;
    p.accent = GColorWhite;
    p.outer_bg = GColorBlack;
    p.inner_bg = GColorBlack;
    p.text_outer = GColorWhite;
  }
#endif
  return p;
}

// The backlight LED cannot be addressed per pixel, but on emery/flint/gabbro it is colour-tintable at
// 8 bits per channel. Tinting to the colourway makes the lit dots noticeably more vivid -- least so for
// red, whose LED is both the least efficient and the one the eye is least sensitive to.
uint32_t pulsar_get_backlight_rgb(int colorway_index) {
  switch (colorway_index) {
    case COLORWAY_DEEP_RED:        return 0xFF3300;  // hot lava orange
    case COLORWAY_PROTOTYPE_GREEN: return 0x22FF22;  // GaP phosphor green
    case COLORWAY_AMBER_GOLD:      return 0xFFAA00;  // amber
    case COLORWAY_COBALT_BLUE:     return 0x00CCFF;  // electric cyan
    case COLORWAY_LUNAR_WHITE:     return 0xFFFFFF;
    case COLORWAY_INVERTED_PAPER:  return 0xFFFFFF;  // dark-on-light wants a neutral wash
    case COLORWAY_VIBRANT_RUBY:
    default:                       return 0xFF0000;  // GaAsP ruby red
  }
}

void pulsar_apply_backlight_tint(int colorway_index, bool enabled) {
#if PBL_API_EXISTS(light_set_color_rgb888)
  if (enabled) {
    light_set_color_rgb888(pulsar_get_backlight_rgb(colorway_index));
  } else {
    light_set_system_color();
  }
#endif
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
