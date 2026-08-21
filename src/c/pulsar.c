#include <pebble.h>

/**
 * 1970s Hamilton Pulsar Watchface for Pebble Time / Pebble Time 2
 * 
 * Inspired by the Pulsar P1 / P2 / P3 "Time Computer".
 * Features:
 * - Procedural GaAsP 5x7 Red Dot-Matrix LED digits.
 * - Unlit "ghost" LED dies rendered beneath the synthetic ruby crystal.
 * - Dynamic geometry scaling for Pebble Time 2 (Emery: 200x228) and Pebble Time (Basalt: 144x168).
 * - Multi-Mode Display Engine: Time, Live Seconds (:SS), Date (MM DD), Steps (08420).
 * - Clay Settings: Operating Mode (Always-On vs Stealth Push-to-Wake), Colorways, Flick Actions, Hourly Vibes, Step Beads.
 * - Dynamic Colorways: Ruby Red (1972), Prototype Green (1975), Amber Gold (HP-01), Cobalt Blue.
 * - 10-Dot Micro-LED Step Progress Bar.
 * - Hourly vibration chime.
 * - Persistent storage & AppMessage settings listener.
 */

#if defined(PBL_PLATFORM_EMERY)
  #define DOT_RADIUS        2
  #define DOT_SPACING_X     7
  #define DOT_SPACING_Y     8
  #define DIGIT_GAP         10
  #define COLON_GAP         16
  #define TOP_MARGIN        76
  #define INDICATOR_RADIUS  2
#else // Basalt, Diorite, Aplite, Chalk (144x168)
  #define DOT_RADIUS        2
  #define DOT_SPACING_X     5
  #define DOT_SPACING_Y     7
  #define DIGIT_GAP         7
  #define COLON_GAP         12
  #define TOP_MARGIN        54
  #define INDICATOR_RADIUS  1
#endif

#define DIGIT_WIDTH 5
#define DIGIT_HEIGHT 7
#define WAKE_DURATION_MS 6000

// Storage & Message Keys
#define STORAGE_KEY_OPERATING_MODE   10000
#define STORAGE_KEY_COLORWAY         10001
#define STORAGE_KEY_FLICK_ACTION     10002
#define STORAGE_KEY_HOURLY_VIBE      10003
#define STORAGE_KEY_SHOW_STEP_BEADS  10004
#define STORAGE_KEY_ITALIC_SLANT     10005
#define STORAGE_KEY_FOOTER_STYLE     10006
#define STORAGE_KEY_STEP_GOAL        10007

#ifndef MESSAGE_KEY_AppKeyOperatingMode
#define MESSAGE_KEY_AppKeyOperatingMode   10000
#define MESSAGE_KEY_AppKeyColorway       10001
#define MESSAGE_KEY_AppKeyFlickAction     10002
#define MESSAGE_KEY_AppKeyHourlyVibe      10003
#define MESSAGE_KEY_AppKeyShowStepBeads  10004
#define MESSAGE_KEY_AppKeyItalicSlant    10005
#define MESSAGE_KEY_AppKeyFooterStyle    10006
#define MESSAGE_KEY_AppKeyStepGoal       10007
#endif

// Operating Modes
enum OperatingMode {
  MODE_ALWAYS_ON = 0,
  MODE_STEALTH = 1
};

// Footer Styles
enum FooterStyle {
  FOOTER_STYLE_TIME_COMPUTER = 0,
  FOOTER_STYLE_HAMILTON = 1,
  FOOTER_STYLE_PULSAR = 2,
  FOOTER_STYLE_NONE = 3
};

// Colorways
enum ColorwayId {
  COLORWAY_VIBRANT_RUBY = 0,
  COLORWAY_DEEP_RED = 1,
  COLORWAY_PROTOTYPE_GREEN = 2,
  COLORWAY_AMBER_GOLD = 3,
  COLORWAY_COBALT_BLUE = 4,
  COLORWAY_LUNAR_WHITE = 5
};
#define NUM_COLORWAYS 6

// Display Modes
enum DisplayMode {
  DISPLAY_MODE_TIME = 0,
  DISPLAY_MODE_SECONDS = 1,
  DISPLAY_MODE_DATE = 2,
  DISPLAY_MODE_STEPS = 3,
  DISPLAY_MODE_BATTERY = 4
};

// Flick Actions
enum FlickAction {
  FLICK_ACTION_CYCLE = 0,
  FLICK_ACTION_SECONDS = 1,
  FLICK_ACTION_DATE = 2,
  FLICK_ACTION_STEPS = 3,
  FLICK_ACTION_BATTERY = 4
};

// Hourly Vibe
enum HourlyVibe {
  HOURLY_VIBE_OFF = 0,
  HOURLY_VIBE_SINGLE = 1,
  HOURLY_VIBE_DOUBLE = 2
};

typedef struct {
  GColor lit;
  GColor ghost;
  GColor accent;
  GColor outer_bg;
  GColor inner_bg;
  GColor text_outer;
} Colorway;

static const Colorway PALETTES[NUM_COLORWAYS] = {
  [COLORWAY_VIBRANT_RUBY] = {
#if defined(PBL_COLOR)
    .lit = GColorSunsetOrange,
    .ghost = GColorBulgarianRose,
    .accent = GColorSunsetOrange,
    .outer_bg = GColorBulgarianRose,
    .inner_bg = GColorBlack,
    .text_outer = GColorLightGray
#else
    .lit = GColorWhite,
    .ghost = GColorBlack,
    .accent = GColorWhite,
    .outer_bg = GColorBlack,
    .inner_bg = GColorBlack,
    .text_outer = GColorWhite
#endif
  },
  [COLORWAY_DEEP_RED] = {
#if defined(PBL_COLOR)
    .lit = GColorRed,
    .ghost = GColorBulgarianRose,
    .accent = GColorDarkCandyAppleRed,
    .outer_bg = GColorBulgarianRose,
    .inner_bg = GColorBlack,
    .text_outer = GColorLightGray
#else
    .lit = GColorWhite,
    .ghost = GColorBlack,
    .accent = GColorWhite,
    .outer_bg = GColorBlack,
    .inner_bg = GColorBlack,
    .text_outer = GColorWhite
#endif
  },
  [COLORWAY_PROTOTYPE_GREEN] = {
#if defined(PBL_COLOR)
    .lit = GColorMintGreen,
    .ghost = GColorDarkGreen,
    .accent = GColorIslamicGreen,
    .outer_bg = GColorDarkGreen,
    .inner_bg = GColorBlack,
    .text_outer = GColorLightGray
#else
    .lit = GColorWhite,
    .ghost = GColorBlack,
    .accent = GColorWhite,
    .outer_bg = GColorBlack,
    .inner_bg = GColorBlack,
    .text_outer = GColorWhite
#endif
  },
  [COLORWAY_AMBER_GOLD] = {
#if defined(PBL_COLOR)
    .lit = GColorChromeYellow,
    .ghost = GColorWindsorTan,
    .accent = GColorWindsorTan,
    .outer_bg = GColorWindsorTan,
    .inner_bg = GColorBlack,
    .text_outer = GColorLightGray
#else
    .lit = GColorWhite,
    .ghost = GColorBlack,
    .accent = GColorWhite,
    .outer_bg = GColorBlack,
    .inner_bg = GColorBlack,
    .text_outer = GColorWhite
#endif
  },
  [COLORWAY_COBALT_BLUE] = {
#if defined(PBL_COLOR)
    .lit = GColorElectricUltramarine,
    .ghost = GColorOxfordBlue,
    .accent = GColorVividCerulean,
    .outer_bg = GColorOxfordBlue,
    .inner_bg = GColorBlack,
    .text_outer = GColorLightGray
#else
    .lit = GColorWhite,
    .ghost = GColorBlack,
    .accent = GColorWhite,
    .outer_bg = GColorBlack,
    .inner_bg = GColorBlack,
    .text_outer = GColorWhite
#endif
  },
  [COLORWAY_LUNAR_WHITE] = {
#if defined(PBL_COLOR)
    .lit = GColorWhite,
    .ghost = GColorDarkGray,
    .accent = GColorLightGray,
    .outer_bg = GColorBlack,
    .inner_bg = GColorBlack,
    .text_outer = GColorLightGray
#else
    .lit = GColorWhite,
    .ghost = GColorBlack,
    .accent = GColorWhite,
    .outer_bg = GColorBlack,
    .inner_bg = GColorBlack,
    .text_outer = GColorWhite
#endif
  }
};

// State Variables
static Window *s_main_window;
static Layer *s_canvas_layer;
static AppTimer *s_mode_timer = NULL;
static int s_display_mode = DISPLAY_MODE_TIME;
static bool s_stealth_awake = false;

static int s_operating_mode = MODE_ALWAYS_ON;
static int s_colorway = COLORWAY_VIBRANT_RUBY;
static int s_flick_action = FLICK_ACTION_CYCLE;
static int s_hourly_vibe = HOURLY_VIBE_OFF;
static bool s_show_step_beads = true;
static bool s_italic_slant = true;
static int s_footer_style = FOOTER_STYLE_TIME_COMPUTER;
static int s_step_goal = 10000;

static bool s_bluetooth_connected = true;
static int s_battery_level = 100;
static int s_last_vibe_hour = -1;

// Authentic 1970s Hamilton Pulsar Rigid 5x7 GaAsP LED Matrix Font (0-9, Blank, B, A, T, %)
static const uint8_t FONT_5X7[15][7] = {
    {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F}, // 0 - Classic rigid rectangular frame
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}, // 1 - Clean centered digital 1
    {0x1F, 0x01, 0x01, 0x1F, 0x10, 0x10, 0x1F}, // 2 - Space-age block 2
    {0x1F, 0x01, 0x01, 0x1F, 0x01, 0x01, 0x1F}, // 3 - Space-age block 3
    {0x11, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x01}, // 4 - Classic digital 4
    {0x1F, 0x10, 0x10, 0x1F, 0x01, 0x01, 0x1F}, // 5 - Space-age block 5
    {0x1F, 0x10, 0x10, 0x1F, 0x11, 0x11, 0x1F}, // 6 - Space-age block 6
    {0x1F, 0x01, 0x01, 0x02, 0x04, 0x04, 0x04}, // 7 - Clean digital 7 with centered stem
    {0x1F, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x1F}, // 8 - Space-age block 8
    {0x1F, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x1F}, // 9 - Space-age block 9
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 10 = Blank
    {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E}, // 11 = 'B'
    {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}, // 12 = 'A'
    {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}, // 13 = 'T'
    {0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03}  // 14 = '%'
};

static int get_step_count(void) {
  int steps = 0;
#if defined(PBL_HEALTH)
  HealthMetric metric = HealthMetricStepCount;
  time_t start = time_start_of_today();
  time_t end = time(NULL);
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, start, end);
  if (mask & HealthServiceAccessibilityMaskAvailable) {
    steps = (int)health_service_sum_today(metric);
  }
#endif
  return steps;
}

static void mode_timer_callback(void *data) {
  s_mode_timer = NULL;
  s_stealth_awake = false;
  s_display_mode = DISPLAY_MODE_TIME;
  layer_mark_dirty(s_canvas_layer);
}

static void trigger_display_change(int mode) {
  s_display_mode = mode;
  if (s_mode_timer) {
    app_timer_cancel(s_mode_timer);
  }
  s_mode_timer = app_timer_register(WAKE_DURATION_MS, mode_timer_callback, NULL);
  layer_mark_dirty(s_canvas_layer);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  if (s_operating_mode == MODE_STEALTH) {
    s_stealth_awake = true;
    if (s_flick_action == FLICK_ACTION_CYCLE) {
      s_display_mode = (s_display_mode + 1) % 5;
    } else if (s_flick_action == FLICK_ACTION_SECONDS) {
      s_display_mode = DISPLAY_MODE_SECONDS;
    } else if (s_flick_action == FLICK_ACTION_DATE) {
      s_display_mode = DISPLAY_MODE_DATE;
    } else if (s_flick_action == FLICK_ACTION_STEPS) {
      s_display_mode = DISPLAY_MODE_STEPS;
    } else if (s_flick_action == FLICK_ACTION_BATTERY) {
      s_display_mode = DISPLAY_MODE_BATTERY;
    } else {
      s_display_mode = DISPLAY_MODE_TIME;
    }
    
    if (s_mode_timer) {
      app_timer_cancel(s_mode_timer);
    }
    s_mode_timer = app_timer_register(WAKE_DURATION_MS, mode_timer_callback, NULL);
    layer_mark_dirty(s_canvas_layer);
  } else {
    // MODE_ALWAYS_ON
    if (s_flick_action == FLICK_ACTION_CYCLE) {
      s_display_mode = (s_display_mode + 1) % 5;
      if (s_mode_timer) {
        app_timer_cancel(s_mode_timer);
        s_mode_timer = NULL;
      }
      if (s_display_mode != DISPLAY_MODE_TIME) {
        s_mode_timer = app_timer_register(WAKE_DURATION_MS, mode_timer_callback, NULL);
      }
      layer_mark_dirty(s_canvas_layer);
    } else if (s_flick_action == FLICK_ACTION_SECONDS) {
      trigger_display_change(DISPLAY_MODE_SECONDS);
    } else if (s_flick_action == FLICK_ACTION_DATE) {
      trigger_display_change(DISPLAY_MODE_DATE);
    } else if (s_flick_action == FLICK_ACTION_STEPS) {
      trigger_display_change(DISPLAY_MODE_STEPS);
    } else if (s_flick_action == FLICK_ACTION_BATTERY) {
      trigger_display_change(DISPLAY_MODE_BATTERY);
    }
  }
}

static void bluetooth_callback(bool connected) {
  s_bluetooth_connected = connected;
  layer_mark_dirty(s_canvas_layer);
}

static void battery_callback(BatteryChargeState state) {
  s_battery_level = state.charge_percent;
  layer_mark_dirty(s_canvas_layer);
}

#if defined(PBL_HEALTH)
static void health_handler(HealthEventType event, void *context) {
  if (event == HealthEventMovementUpdate) {
    layer_mark_dirty(s_canvas_layer);
  }
}
#endif

static void draw_matrix_digit(GContext *ctx, int x_offset, int y_offset, int digit_index, const Colorway *palette, bool is_active, int bounds_w) {
  if (digit_index < 0 || digit_index > 14) digit_index = 10;
  
  int slant_scale = (bounds_w > 180) ? 3 : 2;
  
  for (int r = 0; r < DIGIT_HEIGHT; r++) {
    uint8_t row_bits = FONT_5X7[digit_index][r];
    int slant_x = s_italic_slant ? (((DIGIT_HEIGHT - 1 - r) * slant_scale) / 6) : 0;
    
    for (int c = 0; c < DIGIT_WIDTH; c++) {
      bool is_lit = is_active && ((row_bits >> (4 - c)) & 0x01);
      int dot_x = x_offset + (c * DOT_SPACING_X) + slant_x;
      int dot_y = y_offset + (r * DOT_SPACING_Y);
      
      if (is_lit) {
        graphics_context_set_fill_color(ctx, palette->lit);
        graphics_fill_circle(ctx, GPoint(dot_x, dot_y), DOT_RADIUS);
      } else {
        // Unlit GaAsP ghost die
        graphics_context_set_fill_color(ctx, palette->ghost);
        graphics_fill_circle(ctx, GPoint(dot_x, dot_y), 1);
      }
    }
  }
}

#define GLYPH_3X5(r0, r1, r2, r3, r4) \
  (((r0) << 12) | ((r1) << 9) | ((r2) << 6) | ((r3) << 3) | (r4))

static uint16_t get_glyph_3x5(char c) {
  if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
  switch (c) {
    case 'A': return GLYPH_3X5(2, 5, 7, 5, 5);
    case 'B': return GLYPH_3X5(6, 5, 6, 5, 6);
    case 'C': return GLYPH_3X5(3, 4, 4, 4, 3);
    case 'D': return GLYPH_3X5(6, 5, 5, 5, 6);
    case 'E': return GLYPH_3X5(7, 4, 6, 4, 7);
    case 'F': return GLYPH_3X5(7, 4, 6, 4, 4);
    case 'G': return GLYPH_3X5(3, 4, 5, 5, 3);
    case 'H': return GLYPH_3X5(5, 5, 7, 5, 5);
    case 'I': return GLYPH_3X5(7, 2, 2, 2, 7);
    case 'J': return GLYPH_3X5(1, 1, 1, 5, 2);
    case 'K': return GLYPH_3X5(5, 6, 4, 6, 5);
    case 'L': return GLYPH_3X5(4, 4, 4, 4, 7);
    case 'M': return GLYPH_3X5(5, 7, 5, 5, 5);
    case 'N': return GLYPH_3X5(5, 6, 5, 3, 5);
    case 'O': return GLYPH_3X5(7, 5, 5, 5, 7);
    case 'P': return GLYPH_3X5(6, 5, 6, 4, 4);
    case 'Q': return GLYPH_3X5(7, 5, 5, 6, 3);
    case 'R': return GLYPH_3X5(6, 5, 6, 5, 5);
    case 'S': return GLYPH_3X5(7, 4, 7, 1, 7);
    case 'T': return GLYPH_3X5(7, 2, 2, 2, 2);
    case 'U': return GLYPH_3X5(5, 5, 5, 5, 7);
    case 'V': return GLYPH_3X5(5, 5, 5, 5, 2);
    case 'W': return GLYPH_3X5(5, 5, 5, 7, 5);
    case 'X': return GLYPH_3X5(5, 5, 2, 5, 5);
    case 'Y': return GLYPH_3X5(5, 5, 2, 2, 2);
    case 'Z': return GLYPH_3X5(7, 1, 2, 4, 7);
    case '/': return GLYPH_3X5(1, 1, 2, 4, 4);
    case '-': return GLYPH_3X5(0, 0, 7, 0, 0);
    case '.': return GLYPH_3X5(0, 0, 0, 0, 2);
    case ':': return GLYPH_3X5(0, 2, 0, 2, 0);
    case '%': return GLYPH_3X5(5, 1, 2, 4, 5);
    case '0': return GLYPH_3X5(2, 5, 5, 5, 2);
    case '1': return GLYPH_3X5(2, 2, 2, 2, 2);
    case '2': return GLYPH_3X5(7, 1, 7, 4, 7);
    case '3': return GLYPH_3X5(7, 1, 3, 1, 7);
    case '4': return GLYPH_3X5(5, 5, 7, 1, 1);
    case '5': return GLYPH_3X5(7, 4, 7, 1, 6);
    case '6': return GLYPH_3X5(7, 4, 7, 5, 7);
    case '7': return GLYPH_3X5(7, 1, 2, 2, 2);
    case '8': return GLYPH_3X5(7, 5, 7, 5, 7);
    case '9': return GLYPH_3X5(7, 5, 7, 1, 7);
    default: return 0;
  }
}

static void draw_micro_text(GContext *ctx, const char *text, int center_y, GColor color, int bounds_w, int scale, int letter_spacing, int word_spacing) {
  int len = strlen(text);
  if (len == 0) return;

  int glyph_w = 3 * scale;
  int glyph_h = 5 * scale;
  int total_w = 0;

  for (int i = 0; i < len; i++) {
    if (text[i] == ' ') {
      total_w += word_spacing;
    } else {
      total_w += glyph_w;
      if (i < len - 1 && text[i+1] != ' ') {
        total_w += letter_spacing;
      }
    }
  }

  int cur_x = (bounds_w - total_w) / 2;
  int top_y = center_y - (glyph_h / 2);

  graphics_context_set_fill_color(ctx, color);

  for (int i = 0; i < len; i++) {
    char c = text[i];
    if (c == ' ') {
      cur_x += word_spacing;
      continue;
    }
    uint16_t glyph = get_glyph_3x5(c);
    for (int r = 0; r < 5; r++) {
      for (int col = 0; col < 3; col++) {
        int bit = 14 - (r * 3 + col);
        if ((glyph >> bit) & 1) {
          graphics_fill_rect(ctx, GRect(cur_x + (col * scale), top_y + (r * scale), scale, scale), 0, GCornerNone);
        }
      }
    }
    cur_x += glyph_w;
    if (i < len - 1 && text[i+1] != ' ') {
      cur_x += letter_spacing;
    }
  }
}

static void draw_step_beads(GContext *ctx, GRect bounds, const Colorway *palette, int steps) {
  if (!s_show_step_beads) return;
  
  int num_beads = 10;
  int bead_spacing = bounds.size.w > 180 ? 10 : 7;
  int bead_radius = bounds.size.w > 180 ? 2 : 1;
  int total_bead_width = (num_beads - 1) * bead_spacing;
  int start_x = (bounds.size.w - total_bead_width) / 2;
  int bead_y = bounds.size.w > 180 ? 158 : 118;
  
  bool is_active = (s_operating_mode == MODE_ALWAYS_ON) || s_stealth_awake;
  int goal = s_step_goal > 0 ? s_step_goal : 10000;
  
  for (int i = 0; i < num_beads; i++) {
    int threshold = ((i + 1) * goal) / num_beads;
    bool lit = is_active && (steps >= threshold);
    int bx = start_x + (i * bead_spacing);
    
    GColor bead_color = lit ? GColorLightGray : GColorDarkGray;
    graphics_context_set_fill_color(ctx, bead_color);
    graphics_fill_circle(ctx, GPoint(bx, bead_y), lit ? bead_radius : 1);
  }
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  const Colorway *palette = &PALETTES[s_colorway % NUM_COLORWAYS];
  bool is_active = (s_operating_mode == MODE_ALWAYS_ON) || s_stealth_awake;
  
  // 1. Outermost Watch Dial Bezel (Pure Black Edge)
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  
  // 2. Vintage Ruby Cushion Mask (Surrounds Text and LED Display)
  GRect outer_cushion = bounds.size.w > 180 ? GRect(6, 6, 188, 216) : GRect(4, 4, 136, 160);
  int outer_radius = bounds.size.w > 180 ? 14 : 10;
  graphics_context_set_fill_color(ctx, palette->outer_bg);
  graphics_fill_rect(ctx, outer_cushion, outer_radius, GCornersAll);
  
  graphics_context_set_stroke_color(ctx, palette->accent);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_round_rect(ctx, outer_cushion, outer_radius);

  // 3. Dynamic Space-Age Header at Top of Cushion Window (Symmetrical 11px padding)
  const char *header_text = "PULSAR";
  if (is_active) {
    if (s_display_mode == DISPLAY_MODE_SECONDS) {
      header_text = "SECONDS";
    } else if (s_display_mode == DISPLAY_MODE_DATE) {
      header_text = "DATE";
    } else if (s_display_mode == DISPLAY_MODE_STEPS) {
      header_text = "STEPS";
    } else if (s_display_mode == DISPLAY_MODE_BATTERY) {
      header_text = "BATTERY";
    }
  }

  int header_center_y = bounds.size.w > 180 ? 22 : 16;
  int header_scale = bounds.size.w > 180 ? 2 : 1;
  int header_letter_spacing = bounds.size.w > 180 ? 6 : 4;
  int header_word_spacing = bounds.size.w > 180 ? 10 : 6;
  draw_micro_text(ctx, header_text, header_center_y, palette->text_outer, bounds.size.w,
                  header_scale, header_letter_spacing, header_word_spacing);

  // 4. Central Inner Obsidian Display Aperture (Square-Cornered Rectangle with Symmetrical 32px Margins)
  GRect frame_rect = bounds.size.w > 180 ? GRect(12, 38, 176, 152) : GRect(8, 28, 128, 112);
  graphics_context_set_fill_color(ctx, palette->inner_bg);
  graphics_fill_rect(ctx, frame_rect, 0, GCornerNone);

  // 5. Data Extraction
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  int steps = get_step_count();
  
  int digit_span_x = (DIGIT_WIDTH - 1) * DOT_SPACING_X;
  int start_y = TOP_MARGIN;

  if (s_display_mode == DISPLAY_MODE_STEPS && is_active) {
    // 5-Digit Step Count Layout (e.g., 08420)
    int clamped_steps = steps > 99999 ? 99999 : steps;
    int s1 = (clamped_steps / 10000) % 10;
    int s2 = (clamped_steps / 1000) % 10;
    int s3 = (clamped_steps / 100) % 10;
    int s4 = (clamped_steps / 10) % 10;
    int s5 = clamped_steps % 10;

    int slant_scale = (bounds.size.w > 180) ? 3 : 2;
    int max_slant = s_italic_slant ? slant_scale : 0;
    int step_gap = bounds.size.w > 180 ? 4 : 3;
    int total_5_width = (5 * digit_span_x) + (4 * step_gap) + max_slant;
    int start_5_x = (bounds.size.w - total_5_width) / 2;

    int s_digits[5] = {s1, s2, s3, s4, s5};
    for (int i = 0; i < 5; i++) {
      int dx = start_5_x + (i * (digit_span_x + step_gap));
      draw_matrix_digit(ctx, dx, start_y, s_digits[i], palette, is_active, bounds.size.w);
    }
  } else {
    // 4-Digit Layout (Time, Seconds, Date, Battery)
    int d1 = 10, d2 = 10, d3 = 10, d4 = 10;
    bool show_colon = false;
    bool colon_blinking = false;

    if (is_active) {
      if (s_display_mode == DISPLAY_MODE_DATE) {
        // Date Mode: MM DD
        int month = tick_time->tm_mon + 1;
        int day = tick_time->tm_mday;
        d1 = month / 10;
        d2 = month % 10;
        d3 = day / 10;
        d4 = day % 10;
        show_colon = false;
      } else if (s_display_mode == DISPLAY_MODE_SECONDS) {
        // Live Seconds Mode: :SS with d3 & d4
        d1 = 10; // Blank
        d2 = 10; // Blank
        d3 = tick_time->tm_sec / 10;
        d4 = tick_time->tm_sec % 10;
        show_colon = true;
        colon_blinking = false; // Solid colon in seconds mode
      } else if (s_display_mode == DISPLAY_MODE_BATTERY) {
        // Battery Percentage Mode: e.g. " 85%" or "100%"
        int bat = s_battery_level > 100 ? 100 : (s_battery_level < 0 ? 0 : s_battery_level);
        if (bat == 100) {
          d1 = 1;
          d2 = 0;
          d3 = 0;
          d4 = 14; // '%'
        } else {
          d1 = 10; // Blank
          d2 = (bat >= 10) ? (bat / 10) : 10;
          d3 = bat % 10;
          d4 = 14; // '%'
        }
        show_colon = false;
      } else {
        // Time Mode: HH:MM
        int hours = tick_time->tm_hour;
        if (!clock_is_24h_style()) {
          hours = hours % 12;
          if (hours == 0) hours = 12;
          d1 = (hours >= 10) ? (hours / 10) : 10;
        } else {
          d1 = hours / 10;
        }
        d2 = hours % 10;
        int mins = tick_time->tm_min;
        d3 = mins / 10;
        d4 = mins % 10;
        show_colon = true;
        colon_blinking = true;
      }
    }

    int slant_scale = (bounds.size.w > 180) ? 3 : 2;
    int max_slant = s_italic_slant ? slant_scale : 0;
    int total_width = (digit_span_x * 4) + (DIGIT_GAP * 2) + COLON_GAP + max_slant;
    int start_x = (bounds.size.w - total_width) / 2;

    int d1_x = start_x;
    int d2_x = d1_x + digit_span_x + DIGIT_GAP;
    int d3_x = d2_x + digit_span_x + COLON_GAP;
    int d4_x = d3_x + digit_span_x + DIGIT_GAP;
    
    draw_matrix_digit(ctx, d1_x, start_y, d1, palette, is_active, bounds.size.w);
    draw_matrix_digit(ctx, d2_x, start_y, d2, palette, is_active, bounds.size.w);
    draw_matrix_digit(ctx, d3_x, start_y, d3, palette, is_active, bounds.size.w);
    draw_matrix_digit(ctx, d4_x, start_y, d4, palette, is_active, bounds.size.w);
    
    // Colon Dots with matching slant angle
    int d2_right = d2_x + digit_span_x;
    int colon_base_x = d2_right + (COLON_GAP / 2);
    int colon_slant1 = s_italic_slant ? (((DIGIT_HEIGHT - 1 - 2) * slant_scale) / 6) : 0;
    int colon_slant2 = s_italic_slant ? (((DIGIT_HEIGHT - 1 - 4) * slant_scale) / 6) : 0;
    int colon_x1 = colon_base_x + colon_slant1;
    int colon_x2 = colon_base_x + colon_slant2;
    int colon_y1 = start_y + (DOT_SPACING_Y * 2);
    int colon_y2 = start_y + (DOT_SPACING_Y * 4);
    
    bool colon_lit = show_colon && is_active && (!colon_blinking || (tick_time->tm_sec % 2 == 0));
    GColor colon_color = colon_lit ? palette->lit : palette->ghost;
    graphics_context_set_fill_color(ctx, colon_color);
    graphics_fill_circle(ctx, GPoint(colon_x1, colon_y1), colon_lit ? DOT_RADIUS : 1);
    graphics_fill_circle(ctx, GPoint(colon_x2, colon_y2), colon_lit ? DOT_RADIUS : 1);
    
    // Middle separator dot for Date Mode
    if (s_display_mode == DISPLAY_MODE_DATE) {
      graphics_context_set_fill_color(ctx, is_active ? palette->lit : palette->ghost);
      graphics_fill_circle(ctx, GPoint(colon_x2, colon_y2), is_active ? DOT_RADIUS : 1);
    }
    
    // AM/PM Indicator Dot
    if (is_active && !clock_is_24h_style() && s_display_mode == DISPLAY_MODE_TIME) {
      bool is_pm = tick_time->tm_hour >= 12;
      GColor pm_dot_color = is_pm ? palette->lit : palette->ghost;
      graphics_context_set_fill_color(ctx, pm_dot_color);
      graphics_fill_circle(ctx, GPoint(start_x, start_y + (DIGIT_HEIGHT * DOT_SPACING_Y) + 6), INDICATOR_RADIUS);
    }
  }

  // 5. 10-Dot Micro-LED Step Progress Bar (Gray)
  draw_step_beads(ctx, bounds, palette, steps);
  
  // 6. Status Indicators (Left: BT, Right: Battery - inside bottom of window)
  int indicator_y = bounds.size.w > 180 ? 174 : 128;
  if (!s_bluetooth_connected) {
    graphics_context_set_fill_color(ctx, palette->lit);
    graphics_fill_circle(ctx, GPoint(bounds.size.w / 4, indicator_y), INDICATOR_RADIUS);
  }
  
  if (s_battery_level <= 20) {
    graphics_context_set_fill_color(ctx, palette->lit);
    graphics_fill_circle(ctx, GPoint((bounds.size.w * 3) / 4, indicator_y), INDICATOR_RADIUS);
  }

  // 7. Vintage Footer OUTSIDE the border at Bottom (Symmetrical 10px padding)
  if (s_footer_style != FOOTER_STYLE_NONE) {
    const char *footer_text = "TIME COMPUTER";
    if (s_display_mode == DISPLAY_MODE_STEPS && is_active) {
      footer_text = "DAILY STEPS";
    } else if (s_display_mode == DISPLAY_MODE_BATTERY && is_active) {
      footer_text = "POWER LEVEL";
    } else if (s_display_mode == DISPLAY_MODE_DATE && is_active) {
      footer_text = "MONTH / DAY";
    } else if (s_display_mode == DISPLAY_MODE_SECONDS && is_active) {
      footer_text = "LIVE SECONDS";
    } else {
      if (s_footer_style == FOOTER_STYLE_HAMILTON) {
        footer_text = "HAMILTON";
      } else if (s_footer_style == FOOTER_STYLE_PULSAR) {
        footer_text = "PULSAR";
      }
    }
    
    int footer_center_y = bounds.size.w > 180 ? 207 : 152;
    int footer_scale = bounds.size.w > 180 ? 2 : 1;
    int footer_letter_spacing = bounds.size.w > 180 ? 2 : 1;
    int footer_word_spacing = bounds.size.w > 180 ? 8 : 4;
    draw_micro_text(ctx, footer_text, footer_center_y, palette->text_outer, bounds.size.w,
                    footer_scale, footer_letter_spacing, footer_word_spacing);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Hourly chime
  if (tick_time->tm_min == 0 && tick_time->tm_sec == 0 && tick_time->tm_hour != s_last_vibe_hour) {
    s_last_vibe_hour = tick_time->tm_hour;
    if (s_hourly_vibe == HOURLY_VIBE_SINGLE) {
      vibes_short_pulse();
    } else if (s_hourly_vibe == HOURLY_VIBE_DOUBLE) {
      vibes_double_pulse();
    }
  }
  layer_mark_dirty(s_canvas_layer);
}

static int tuple_to_int(Tuple *tuple, int default_val) {
  if (!tuple) return default_val;
  if (tuple->type == TUPLE_INT) {
    if (tuple->length == 1) return tuple->value->int8;
    if (tuple->length == 2) return tuple->value->int16;
    if (tuple->length == 4) return tuple->value->int32;
  } else if (tuple->type == TUPLE_UINT) {
    if (tuple->length == 1) return tuple->value->uint8;
    if (tuple->length == 2) return tuple->value->uint16;
    if (tuple->length == 4) return tuple->value->uint32;
  } else if (tuple->type == TUPLE_CSTRING) {
    return atoi(tuple->value->cstring);
  }
  return default_val;
}

static bool tuple_to_bool(Tuple *tuple, bool default_val) {
  if (!tuple) return default_val;
  if (tuple->type == TUPLE_INT || tuple->type == TUPLE_UINT) {
    return tuple_to_int(tuple, default_val ? 1 : 0) != 0;
  } else if (tuple->type == TUPLE_CSTRING) {
    return (strcmp(tuple->value->cstring, "true") == 0 || strcmp(tuple->value->cstring, "1") == 0);
  }
  return default_val;
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *t_op = dict_find(iterator, MESSAGE_KEY_AppKeyOperatingMode);
  if (t_op) {
    s_operating_mode = tuple_to_int(t_op, s_operating_mode);
    persist_write_int(STORAGE_KEY_OPERATING_MODE, s_operating_mode);
  }
  Tuple *t_col = dict_find(iterator, MESSAGE_KEY_AppKeyColorway);
  if (t_col) {
    s_colorway = tuple_to_int(t_col, s_colorway);
    if (s_colorway < 0 || s_colorway >= NUM_COLORWAYS) s_colorway = 0;
    persist_write_int(STORAGE_KEY_COLORWAY, s_colorway);
  }
  Tuple *t_flick = dict_find(iterator, MESSAGE_KEY_AppKeyFlickAction);
  if (t_flick) {
    s_flick_action = tuple_to_int(t_flick, s_flick_action);
    persist_write_int(STORAGE_KEY_FLICK_ACTION, s_flick_action);
  }
  Tuple *t_vibe = dict_find(iterator, MESSAGE_KEY_AppKeyHourlyVibe);
  if (t_vibe) {
    s_hourly_vibe = tuple_to_int(t_vibe, s_hourly_vibe);
    persist_write_int(STORAGE_KEY_HOURLY_VIBE, s_hourly_vibe);
  }
  Tuple *t_beads = dict_find(iterator, MESSAGE_KEY_AppKeyShowStepBeads);
  if (t_beads) {
    s_show_step_beads = tuple_to_bool(t_beads, s_show_step_beads);
    persist_write_bool(STORAGE_KEY_SHOW_STEP_BEADS, s_show_step_beads);
  }
  Tuple *t_slant = dict_find(iterator, MESSAGE_KEY_AppKeyItalicSlant);
  if (t_slant) {
    s_italic_slant = tuple_to_bool(t_slant, s_italic_slant);
    persist_write_bool(STORAGE_KEY_ITALIC_SLANT, s_italic_slant);
  }
  Tuple *t_footer = dict_find(iterator, MESSAGE_KEY_AppKeyFooterStyle);
  if (t_footer) {
    s_footer_style = tuple_to_int(t_footer, s_footer_style);
    persist_write_int(STORAGE_KEY_FOOTER_STYLE, s_footer_style);
  }
  Tuple *t_goal = dict_find(iterator, MESSAGE_KEY_AppKeyStepGoal);
  if (t_goal) {
    s_step_goal = tuple_to_int(t_goal, s_step_goal);
    if (s_step_goal <= 0) s_step_goal = 10000;
    persist_write_int(STORAGE_KEY_STEP_GOAL, s_step_goal);
  }
  layer_mark_dirty(s_canvas_layer);
}

static void load_settings(void) {
  if (persist_exists(STORAGE_KEY_OPERATING_MODE)) {
    s_operating_mode = persist_read_int(STORAGE_KEY_OPERATING_MODE);
  }
  if (persist_exists(STORAGE_KEY_COLORWAY)) {
    s_colorway = persist_read_int(STORAGE_KEY_COLORWAY);
    if (s_colorway < 0 || s_colorway >= NUM_COLORWAYS) s_colorway = 0;
  }
  if (persist_exists(STORAGE_KEY_FLICK_ACTION)) {
    s_flick_action = persist_read_int(STORAGE_KEY_FLICK_ACTION);
  }
  if (persist_exists(STORAGE_KEY_HOURLY_VIBE)) {
    s_hourly_vibe = persist_read_int(STORAGE_KEY_HOURLY_VIBE);
  }
  if (persist_exists(STORAGE_KEY_SHOW_STEP_BEADS)) {
    s_show_step_beads = persist_read_bool(STORAGE_KEY_SHOW_STEP_BEADS);
  }
  if (persist_exists(STORAGE_KEY_ITALIC_SLANT)) {
    s_italic_slant = persist_read_bool(STORAGE_KEY_ITALIC_SLANT);
  }
  if (persist_exists(STORAGE_KEY_FOOTER_STYLE)) {
    s_footer_style = persist_read_int(STORAGE_KEY_FOOTER_STYLE);
  }
  if (persist_exists(STORAGE_KEY_STEP_GOAL)) {
    s_step_goal = persist_read_int(STORAGE_KEY_STEP_GOAL);
    if (s_step_goal <= 0) s_step_goal = 10000;
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(window_layer, s_canvas_layer);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

static void init(void) {
  load_settings();

  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  
  // AppMessage configuration
  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(256, 256);

  // Subscribe to services
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });
  battery_state_service_subscribe(battery_callback);

#if defined(PBL_HEALTH)
  health_service_events_subscribe(health_handler, NULL);
#endif
  
  // Initial state
  s_bluetooth_connected = connection_service_peek_pebble_app_connection();
  BatteryChargeState charge_state = battery_state_service_peek();
  s_battery_level = charge_state.charge_percent;
}

static void deinit(void) {
#if defined(PBL_HEALTH)
  health_service_events_unsubscribe();
#endif
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  accel_tap_service_unsubscribe();
  tick_timer_service_unsubscribe();
  if (s_mode_timer) {
    app_timer_cancel(s_mode_timer);
  }
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
