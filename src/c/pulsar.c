#include <pebble.h>

/**
 * 1970s Hamilton Pulsar Watchface for Pebble Time / Pebble Time 2
 * 
 * Inspired by the Pulsar P1 / P2 / P3 "Time Computer".
 * Features:
 * - Procedural GaAsP 5x7 Dot-Matrix LED digits.
 * - Unlit "ghost" LED dies rendered beneath the synthetic ruby crystal.
 * - Dynamic geometry scaling for Pebble Time 2 (Emery: 200x228) and Pebble Time / Pebble 2 (144x168).
 * - Multi-Mode Display Engine: Time, Live Seconds (:SS), Date (MM DD / DD MM), Steps (08420), Battery (100%).
 * - Clay Settings: Operating Mode, Themes (including Inverted Paper), Slant, Header/Footer, Bead Mode, Date Format, Vibrations.
 * - 10-Dot Micro-LED Progress Bar (Steps vs Battery Meter vs Off).
 * - Hourly chime & Bluetooth disconnect alert.
 * - Persistent storage & AppMessage settings listener.
 */

#if defined(PBL_PLATFORM_EMERY)
  #define DOT_RADIUS        2
  #define DOT_SPACING_X     7
  #define DOT_SPACING_Y     8
  #define DIGIT_GAP         14
  #define COLON_GAP         22
  #define TOP_MARGIN        70
  #define INDICATOR_RADIUS  2
#else // Basalt, Diorite, Aplite (144x168)
  #define DOT_RADIUS        1
  #define DOT_SPACING_X     5
  #define DOT_SPACING_Y     6
  #define DIGIT_GAP         10
  #define COLON_GAP         16
  #define TOP_MARGIN        48
  #define INDICATOR_RADIUS  1
#endif

#define DIGIT_WIDTH 5
#define DIGIT_HEIGHT 7
#define WAKE_DURATION_MS 4000

// Storage & Message Keys
#define STORAGE_KEY_OPERATING_MODE   10000
#define STORAGE_KEY_COLORWAY         10001
#define STORAGE_KEY_FLICK_ACTION     10002
#define STORAGE_KEY_HOURLY_VIBE      10003
#define STORAGE_KEY_SHOW_STEP_BEADS  10004
#define STORAGE_KEY_ITALIC_SLANT     10005
#define STORAGE_KEY_FOOTER_STYLE     10006
#define STORAGE_KEY_STEP_GOAL        10007
#define STORAGE_KEY_HEADER_STYLE     10008
#define STORAGE_KEY_DATE_FORMAT      10009
#define STORAGE_KEY_LEADING_ZERO     10010
#define STORAGE_KEY_BT_VIBE          10011
#define STORAGE_KEY_BEAD_MODE        10012

#ifndef MESSAGE_KEY_AppKeyOperatingMode
#define MESSAGE_KEY_AppKeyOperatingMode   10000
#define MESSAGE_KEY_AppKeyColorway       10001
#define MESSAGE_KEY_AppKeyFlickAction     10002
#define MESSAGE_KEY_AppKeyHourlyVibe      10003
#define MESSAGE_KEY_AppKeyShowStepBeads  10004
#define MESSAGE_KEY_AppKeyItalicSlant    10005
#define MESSAGE_KEY_AppKeyFooterStyle    10006
#define MESSAGE_KEY_AppKeyStepGoal       10007
#define MESSAGE_KEY_AppKeyHeaderStyle    10008
#define MESSAGE_KEY_AppKeyDateFormat     10009
#define MESSAGE_KEY_AppKeyLeadingZero    10010
#define MESSAGE_KEY_AppKeyBtVibe         10011
#define MESSAGE_KEY_AppKeyBeadMode       10012
#endif

// Operating Modes
enum OperatingMode {
  MODE_ALWAYS_ON = 0,
  MODE_STEALTH = 1
};

// Header Styles
enum HeaderStyle {
  HEADER_STYLE_PULSAR = 0,
  HEADER_STYLE_HAMILTON = 1,
  HEADER_STYLE_SOLID_STATE = 2,
  HEADER_STYLE_NONE = 3
};

// Footer Styles
enum FooterStyle {
  FOOTER_STYLE_TIME_COMPUTER = 0,
  FOOTER_STYLE_SOLID_STATE = 1,
  FOOTER_STYLE_HAMILTON = 2,
  FOOTER_STYLE_PULSAR = 3,
  FOOTER_STYLE_SWISS = 4,
  FOOTER_STYLE_NONE = 5
};

// Bead Modes
enum BeadMode {
  BEAD_MODE_STEPS = 0,
  BEAD_MODE_BATTERY = 1,
  BEAD_MODE_OFF = 2
};

// Date Formats
enum DateFormat {
  DATE_FORMAT_MD = 0, // Month Day
  DATE_FORMAT_DM = 1  // Day Month
};

// Colorways
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

static Colorway get_current_palette(int colorway_index) {
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
static bool s_bt_vibe = false;
static int s_bead_mode = BEAD_MODE_STEPS;
static bool s_italic_slant = true;
static int s_header_style = HEADER_STYLE_PULSAR;
static int s_footer_style = FOOTER_STYLE_TIME_COMPUTER;
static int s_date_format = DATE_FORMAT_MD;
static bool s_leading_zero = false;
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
    if (!s_stealth_awake) {
      // First tap/flick: Wake up and illuminate TIME (or dedicated action)
      s_stealth_awake = true;
      if (s_flick_action == FLICK_ACTION_SECONDS) {
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
    } else {
      // Already awake: subsequent taps cycle through the modes
      if (s_flick_action == FLICK_ACTION_CYCLE) {
        s_display_mode = (s_display_mode + 1) % 5;
      }
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

#if PBL_API_EXISTS(touch_service_subscribe)
static void touch_handler(const TouchEvent *event, void *context) {
  if (event->type == TouchEvent_Touchdown) {
    tap_handler(ACCEL_AXIS_Z, 1);
  }
}
#endif

static void bluetooth_callback(bool connected) {
  if (s_bt_vibe && !connected && s_bluetooth_connected) {
    vibes_double_pulse();
  }
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

static void draw_matrix_digit_custom(GContext *ctx, int x_offset, int y_offset, int digit_index, 
                                     const Colorway *palette, bool is_active, int bounds_w,
                                     int spacing_x, int spacing_y, int dot_radius) {
  if (digit_index < 0 || digit_index > 14) digit_index = 10;
  
  int slant_scale = (bounds_w > 180) ? 7 : 5;
  
  for (int r = 0; r < DIGIT_HEIGHT; r++) {
    uint8_t row_bits = FONT_5X7[digit_index][r];
    int slant_x = s_italic_slant ? (((DIGIT_HEIGHT - 1 - r) * slant_scale) / (DIGIT_HEIGHT - 1)) : 0;
    
    for (int c = 0; c < DIGIT_WIDTH; c++) {
      bool is_lit = is_active && ((row_bits >> (4 - c)) & 0x01);
      int dot_x = x_offset + (c * spacing_x) + slant_x;
      int dot_y = y_offset + (r * spacing_y);
      
      if (is_lit) {
        graphics_context_set_fill_color(ctx, palette->lit);
        graphics_fill_circle(ctx, GPoint(dot_x, dot_y), dot_radius);
      } else {
#if defined(PBL_COLOR)
        if (s_colorway != COLORWAY_INVERTED_PAPER) {
          // Subtle ghost die beneath dark crystal on 64-color displays
          graphics_context_set_fill_color(ctx, palette->ghost);
          graphics_fill_circle(ctx, GPoint(dot_x, dot_y), 1);
        }
#else
        // Pure black / white on 1-bit monochrome (no dither noise)
#endif
      }
    }
  }
}

static void draw_matrix_digit(GContext *ctx, int x_offset, int y_offset, int digit_index, const Colorway *palette, bool is_active, int bounds_w) {
  draw_matrix_digit_custom(ctx, x_offset, y_offset, digit_index, palette, is_active, bounds_w, DOT_SPACING_X, DOT_SPACING_Y, DOT_RADIUS);
}

static void draw_step_beads(GContext *ctx, GRect bounds, const Colorway *palette, int steps) {
  if (s_bead_mode == BEAD_MODE_OFF) return;
  
  int num_beads = 10;
  int bead_spacing = bounds.size.w > 180 ? 10 : 7;
  int bead_radius = bounds.size.w > 180 ? 2 : 1;
  int total_bead_width = (num_beads - 1) * bead_spacing;
  int start_x = (bounds.size.w - total_bead_width) / 2;
  int bead_y = bounds.size.w > 180 ? 148 : 108;
  
  bool is_active = (s_operating_mode == MODE_ALWAYS_ON) || s_stealth_awake;
  int goal = s_step_goal > 0 ? s_step_goal : 10000;
  
  for (int i = 0; i < num_beads; i++) {
    bool lit = false;
    if (s_bead_mode == BEAD_MODE_STEPS) {
      int threshold = ((i + 1) * goal) / num_beads;
      lit = is_active && (steps >= threshold);
    } else if (s_bead_mode == BEAD_MODE_BATTERY) {
      int threshold = (i + 1) * 10;
      lit = is_active && (s_battery_level >= threshold);
    }
    int bx = start_x + (i * bead_spacing);
    
#if defined(PBL_COLOR)
    if (s_colorway == COLORWAY_INVERTED_PAPER) {
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

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  Colorway active_palette = get_current_palette(s_colorway % NUM_COLORWAYS);
  const Colorway *palette = &active_palette;
  bool is_active = (s_operating_mode == MODE_ALWAYS_ON) || s_stealth_awake;
  
  // 1. Background Fill across entire screen (Zero border clutter)
  graphics_context_set_fill_color(ctx, palette->outer_bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // 2. Vintage Space-Age Brand Header at Top
  if (s_header_style != HEADER_STYLE_NONE) {
    const char *header_text = "P U L S A R";
    if (s_header_style == HEADER_STYLE_HAMILTON) {
      header_text = "H A M I L T O N";
    } else if (s_header_style == HEADER_STYLE_SOLID_STATE) {
      header_text = "S O L I D   S T A T E";
    }
    GFont font_header = bounds.size.w > 180 ? 
                        fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD) : 
                        fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
    int header_y = bounds.size.w > 180 ? 14 : 8;
    graphics_context_set_text_color(ctx, palette->text_outer);
    graphics_draw_text(ctx, header_text, font_header,
                       GRect(0, header_y, bounds.size.w, bounds.size.w > 180 ? 24 : 18),
                       GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }

  // 3. Status Annunciator Dots (Top Left: BT Disconnect, Top Right: Battery Low)
  int ind_y = bounds.size.w > 180 ? 20 : 12;
  int ind_margin = bounds.size.w > 180 ? 18 : 12;
  if (!s_bluetooth_connected) {
    graphics_context_set_fill_color(ctx, palette->lit);
    graphics_fill_circle(ctx, GPoint(ind_margin, ind_y), INDICATOR_RADIUS);
  }
  if (s_battery_level <= 20) {
    graphics_context_set_fill_color(ctx, palette->lit);
    graphics_fill_circle(ctx, GPoint(bounds.size.w - ind_margin, ind_y), INDICATOR_RADIUS);
  }

  // 4. Data Extraction & Display Engine
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  int steps = get_step_count();
  
  int digit_span_x = (DIGIT_WIDTH - 1) * DOT_SPACING_X;
  int start_y = bounds.size.w > 180 ? 70 : 48;
  int slant_scale = (bounds.size.w > 180) ? 7 : 5;
  int max_slant = s_italic_slant ? slant_scale : 0;

  if (s_display_mode == DISPLAY_MODE_STEPS && is_active) {
    // 5-Digit Step Count Layout (e.g., 0 8 4 2 0)
    int clamped_steps = steps > 99999 ? 99999 : steps;
    int s1 = (clamped_steps / 10000) % 10;
    int s2 = (clamped_steps / 1000) % 10;
    int s3 = (clamped_steps / 100) % 10;
    int s4 = (clamped_steps / 10) % 10;
    int s5 = clamped_steps % 10;

    int step_spacing_x = bounds.size.w > 180 ? 6 : 4;
    int step_spacing_y = bounds.size.w > 180 ? 7 : 5;
    int step_dot_radius = bounds.size.w > 180 ? 2 : 1;
    int step_gap = bounds.size.w > 180 ? 11 : 8;
    int step_digit_span = (DIGIT_WIDTH - 1) * step_spacing_x;

    int total_5_width = (5 * step_digit_span) + (4 * step_gap) + max_slant;
    int start_5_x = (bounds.size.w - total_5_width) / 2;
    int step_start_y = bounds.size.w > 180 ? 72 : 50;

    int s_digits[5] = {s1, s2, s3, s4, s5};
    for (int i = 0; i < 5; i++) {
      int dx = start_5_x + (i * (step_digit_span + step_gap));
      draw_matrix_digit_custom(ctx, dx, step_start_y, s_digits[i], palette, is_active, bounds.size.w,
                               step_spacing_x, step_spacing_y, step_dot_radius);
    }
  } else if (s_display_mode == DISPLAY_MODE_SECONDS) {
    // Centered Live Seconds Layout (:SS)
    int secs = tick_time->tm_sec;
    int sec_tens = is_active ? (secs / 10) : 10;
    int sec_ones = is_active ? (secs % 10) : 10;
    
    int sec_digit_gap = bounds.size.w > 180 ? 14 : 10;
    int sec_colon_gap = bounds.size.w > 180 ? 22 : 16;
    int sec_total_width = (digit_span_x * 2) + sec_digit_gap + sec_colon_gap + max_slant;
    int sec_start_x = (bounds.size.w - sec_total_width) / 2;
    
    int sec_d3_x = sec_start_x + sec_colon_gap;
    int sec_d4_x = sec_d3_x + digit_span_x + sec_digit_gap;
    
    draw_matrix_digit(ctx, sec_d3_x, start_y, sec_tens, palette, is_active, bounds.size.w);
    draw_matrix_digit(ctx, sec_d4_x, start_y, sec_ones, palette, is_active, bounds.size.w);
    
    // Centered Colon before seconds digits
    int sec_colon_base_x = sec_start_x + (sec_colon_gap / 2);
    int colon_slant1 = s_italic_slant ? (((DIGIT_HEIGHT - 1 - 2) * slant_scale) / (DIGIT_HEIGHT - 1)) : 0;
    int colon_slant2 = s_italic_slant ? (((DIGIT_HEIGHT - 1 - 4) * slant_scale) / (DIGIT_HEIGHT - 1)) : 0;
    int sec_colon_x1 = sec_colon_base_x + colon_slant1;
    int sec_colon_x2 = sec_colon_base_x + colon_slant2;
    int colon_y1 = start_y + (DOT_SPACING_Y * 2);
    int colon_y2 = start_y + (DOT_SPACING_Y * 4);
    
#if defined(PBL_COLOR)
    graphics_context_set_fill_color(ctx, is_active ? palette->lit : palette->ghost);
    graphics_fill_circle(ctx, GPoint(sec_colon_x1, colon_y1), is_active ? DOT_RADIUS : 1);
    graphics_fill_circle(ctx, GPoint(sec_colon_x2, colon_y2), is_active ? DOT_RADIUS : 1);
#else
    if (is_active) {
      graphics_context_set_fill_color(ctx, palette->lit);
      graphics_fill_circle(ctx, GPoint(sec_colon_x1, colon_y1), DOT_RADIUS);
      graphics_fill_circle(ctx, GPoint(sec_colon_x2, colon_y2), DOT_RADIUS);
    }
#endif
  } else {
    // 4-Digit Layout for Time, Date, Battery
    int d1 = 10, d2 = 10, d3 = 10, d4 = 10;
    bool show_colon = false;
    bool colon_blinking = false;

    if (s_display_mode == DISPLAY_MODE_DATE) {
      if (is_active) {
        int month = tick_time->tm_mon + 1;
        int day = tick_time->tm_mday;
        if (s_date_format == DATE_FORMAT_DM) {
          d1 = day / 10;
          d2 = day % 10;
          d3 = month / 10;
          d4 = month % 10;
        } else {
          d1 = month / 10;
          d2 = month % 10;
          d3 = day / 10;
          d4 = day % 10;
        }
        show_colon = false;
      }
    } else if (s_display_mode == DISPLAY_MODE_BATTERY) {
      if (is_active) {
        int bat = s_battery_level;
        if (bat >= 100) {
          d1 = 1;
          d2 = 0;
          d3 = 0;
          d4 = 14; // %
        } else {
          d2 = bat / 10;
          d3 = bat % 10;
          d4 = 14; // %
        }
      }
    } else {
      // DISPLAY_MODE_TIME
      if (is_active) {
        int hours = tick_time->tm_hour;
        if (!clock_is_24h_style()) {
          hours = hours % 12;
          if (hours == 0) hours = 12;
        }
        if (hours < 10) {
          d1 = s_leading_zero ? 0 : 10; // blank or 0
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
    int colon_slant1 = s_italic_slant ? (((DIGIT_HEIGHT - 1 - 2) * slant_scale) / (DIGIT_HEIGHT - 1)) : 0;
    int colon_slant2 = s_italic_slant ? (((DIGIT_HEIGHT - 1 - 4) * slant_scale) / (DIGIT_HEIGHT - 1)) : 0;
    int colon_x1 = colon_base_x + colon_slant1;
    int colon_x2 = colon_base_x + colon_slant2;
    int colon_y1 = start_y + (DOT_SPACING_Y * 2);
    int colon_y2 = start_y + (DOT_SPACING_Y * 4);
    
    bool colon_lit = show_colon && is_active && (!colon_blinking || (tick_time->tm_sec % 2 == 0));
#if defined(PBL_COLOR)
    GColor colon_color = colon_lit ? palette->lit : palette->ghost;
    graphics_context_set_fill_color(ctx, colon_color);
    graphics_fill_circle(ctx, GPoint(colon_x1, colon_y1), colon_lit ? DOT_RADIUS : 1);
    graphics_fill_circle(ctx, GPoint(colon_x2, colon_y2), colon_lit ? DOT_RADIUS : 1);
#else
    if (colon_lit) {
      graphics_context_set_fill_color(ctx, palette->lit);
      graphics_fill_circle(ctx, GPoint(colon_x1, colon_y1), DOT_RADIUS);
      graphics_fill_circle(ctx, GPoint(colon_x2, colon_y2), DOT_RADIUS);
    }
#endif
    
    // Middle separator dot for Date Mode
    if (s_display_mode == DISPLAY_MODE_DATE && is_active) {
      graphics_context_set_fill_color(ctx, palette->lit);
      graphics_fill_circle(ctx, GPoint(colon_x2, colon_y2), DOT_RADIUS);
    }
    
    // AM/PM Indicator Dot
    if (is_active && !clock_is_24h_style() && s_display_mode == DISPLAY_MODE_TIME) {
      bool is_pm = tick_time->tm_hour >= 12;
      if (is_pm) {
        graphics_context_set_fill_color(ctx, palette->lit);
        graphics_fill_circle(ctx, GPoint(start_x, start_y + (DIGIT_HEIGHT * DOT_SPACING_Y) + 4), INDICATOR_RADIUS);
      }
    }
  }

  // 5. 10-Dot Micro-LED Progress Bar
  draw_step_beads(ctx, bounds, palette, steps);

  // 6. Vintage Space-Age Footer
  if (s_footer_style != FOOTER_STYLE_NONE) {
    const char *footer_text = "T I M E   C O M P U T E R";
    if (s_display_mode == DISPLAY_MODE_STEPS && is_active) {
      footer_text = "S T E P S";
    } else if (s_display_mode == DISPLAY_MODE_BATTERY && is_active) {
      footer_text = "B A T T E R Y";
    } else if (s_display_mode == DISPLAY_MODE_DATE && is_active) {
      footer_text = "D A T E";
    } else if (s_display_mode == DISPLAY_MODE_SECONDS && is_active) {
      footer_text = "S E C O N D S";
    } else {
      if (s_footer_style == FOOTER_STYLE_SOLID_STATE) {
        footer_text = "S O L I D   S T A T E";
      } else if (s_footer_style == FOOTER_STYLE_HAMILTON) {
        footer_text = "H A M I L T O N";
      } else if (s_footer_style == FOOTER_STYLE_PULSAR) {
        footer_text = "P U L S A R";
      } else if (s_footer_style == FOOTER_STYLE_SWISS) {
        footer_text = "S W I S S   M A D E";
      }
    }
    
    GFont font_footer = bounds.size.w > 180 ? 
                        fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD) : 
                        fonts_get_system_font(FONT_KEY_GOTHIC_14);
    int footer_y = bounds.size.w > 180 ? 192 : 142;
    graphics_context_set_text_color(ctx, palette->text_outer);
    graphics_draw_text(ctx, footer_text, font_footer,
                       GRect(0, footer_y, bounds.size.w, bounds.size.w > 180 ? 20 : 14),
                       GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Check hourly vibration
  if (units_changed & HOUR_UNIT) {
    if (s_hourly_vibe != HOURLY_VIBE_OFF && tick_time->tm_hour != s_last_vibe_hour) {
      s_last_vibe_hour = tick_time->tm_hour;
      if (s_hourly_vibe == HOURLY_VIBE_SINGLE) {
        vibes_short_pulse();
      } else if (s_hourly_vibe == HOURLY_VIBE_DOUBLE) {
        vibes_double_pulse();
      }
    }
  }

  // Redraw every second if Live Seconds is active, otherwise every minute
  if (s_display_mode == DISPLAY_MODE_SECONDS || (s_operating_mode == MODE_ALWAYS_ON && (units_changed & SECOND_UNIT))) {
    layer_mark_dirty(s_canvas_layer);
  } else if (units_changed & MINUTE_UNIT) {
    layer_mark_dirty(s_canvas_layer);
  }
}

static int tuple_to_int(Tuple *tuple, int default_val) {
  if (!tuple) return default_val;
  switch (tuple->type) {
    case TUPLE_INT:
    case TUPLE_UINT:
      if (tuple->length == 1) return (int)tuple->value->int8;
      if (tuple->length == 2) return (int)tuple->value->int16;
      if (tuple->length == 4) return (int)tuple->value->int32;
      return (int)tuple->value->int32;
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

static bool tuple_to_bool(Tuple *tuple, bool default_val) {
  if (!tuple) return default_val;
  switch (tuple->type) {
    case TUPLE_INT:
    case TUPLE_UINT:
      return tuple_to_int(tuple, default_val ? 1 : 0) != 0;
    case TUPLE_CSTRING:
      if (tuple->length > 0) {
        return (strcmp(tuple->value->cstring, "true") == 0 ||
                strcmp(tuple->value->cstring, "1") == 0);
      }
      break;
    default:
      break;
  }
  return default_val;
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "AppMessage dropped: %d", (int)reason);
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "AppMessage outbox failed: %d", (int)reason);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "AppMessage received!");
  for (Tuple *t = dict_read_first(iterator); t != NULL; t = dict_read_next(iterator)) {
    uint32_t key = t->key;
    if (key == MESSAGE_KEY_AppKeyOperatingMode) {
      s_operating_mode = tuple_to_int(t, s_operating_mode);
      persist_write_int(STORAGE_KEY_OPERATING_MODE, s_operating_mode);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyOperatingMode: %d", s_operating_mode);
    } else if (key == MESSAGE_KEY_AppKeyColorway) {
      s_colorway = tuple_to_int(t, s_colorway);
      if (s_colorway < 0 || s_colorway >= NUM_COLORWAYS) s_colorway = 0;
      persist_write_int(STORAGE_KEY_COLORWAY, s_colorway);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyColorway: %d", s_colorway);
    } else if (key == MESSAGE_KEY_AppKeyFlickAction) {
      s_flick_action = tuple_to_int(t, s_flick_action);
      persist_write_int(STORAGE_KEY_FLICK_ACTION, s_flick_action);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyFlickAction: %d", s_flick_action);
    } else if (key == MESSAGE_KEY_AppKeyHourlyVibe) {
      s_hourly_vibe = tuple_to_int(t, s_hourly_vibe);
      persist_write_int(STORAGE_KEY_HOURLY_VIBE, s_hourly_vibe);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyHourlyVibe: %d", s_hourly_vibe);
    } else if (key == MESSAGE_KEY_AppKeyBtVibe) {
      s_bt_vibe = tuple_to_bool(t, s_bt_vibe);
      persist_write_bool(STORAGE_KEY_BT_VIBE, s_bt_vibe);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyBtVibe: %d", (int)s_bt_vibe);
    } else if (key == MESSAGE_KEY_AppKeyBeadMode) {
      s_bead_mode = tuple_to_int(t, s_bead_mode);
      persist_write_int(STORAGE_KEY_BEAD_MODE, s_bead_mode);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyBeadMode: %d", s_bead_mode);
    } else if (key == MESSAGE_KEY_AppKeyShowStepBeads) {
      bool show = tuple_to_bool(t, true);
      s_bead_mode = show ? BEAD_MODE_STEPS : BEAD_MODE_OFF;
      persist_write_int(STORAGE_KEY_BEAD_MODE, s_bead_mode);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyShowStepBeads: %d", (int)show);
    } else if (key == MESSAGE_KEY_AppKeyItalicSlant) {
      s_italic_slant = tuple_to_bool(t, s_italic_slant);
      persist_write_bool(STORAGE_KEY_ITALIC_SLANT, s_italic_slant);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyItalicSlant: %d", (int)s_italic_slant);
    } else if (key == MESSAGE_KEY_AppKeyHeaderStyle) {
      s_header_style = tuple_to_int(t, s_header_style);
      persist_write_int(STORAGE_KEY_HEADER_STYLE, s_header_style);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyHeaderStyle: %d", s_header_style);
    } else if (key == MESSAGE_KEY_AppKeyFooterStyle) {
      s_footer_style = tuple_to_int(t, s_footer_style);
      persist_write_int(STORAGE_KEY_FOOTER_STYLE, s_footer_style);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyFooterStyle: %d", s_footer_style);
    } else if (key == MESSAGE_KEY_AppKeyDateFormat) {
      s_date_format = tuple_to_int(t, s_date_format);
      persist_write_int(STORAGE_KEY_DATE_FORMAT, s_date_format);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyDateFormat: %d", s_date_format);
    } else if (key == MESSAGE_KEY_AppKeyLeadingZero) {
      s_leading_zero = tuple_to_bool(t, s_leading_zero);
      persist_write_bool(STORAGE_KEY_LEADING_ZERO, s_leading_zero);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyLeadingZero: %d", (int)s_leading_zero);
    } else if (key == MESSAGE_KEY_AppKeyStepGoal) {
      s_step_goal = tuple_to_int(t, s_step_goal);
      if (s_step_goal <= 0) s_step_goal = 10000;
      persist_write_int(STORAGE_KEY_STEP_GOAL, s_step_goal);
      APP_LOG(APP_LOG_LEVEL_INFO, "AppKeyStepGoal: %d", s_step_goal);
    }
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
  if (persist_exists(STORAGE_KEY_BT_VIBE)) {
    s_bt_vibe = persist_read_bool(STORAGE_KEY_BT_VIBE);
  }
  if (persist_exists(STORAGE_KEY_BEAD_MODE)) {
    s_bead_mode = persist_read_int(STORAGE_KEY_BEAD_MODE);
  } else if (persist_exists(STORAGE_KEY_SHOW_STEP_BEADS)) {
    bool show = persist_read_bool(STORAGE_KEY_SHOW_STEP_BEADS);
    s_bead_mode = show ? BEAD_MODE_STEPS : BEAD_MODE_OFF;
  }
  if (persist_exists(STORAGE_KEY_ITALIC_SLANT)) {
    s_italic_slant = persist_read_bool(STORAGE_KEY_ITALIC_SLANT);
  }
  if (persist_exists(STORAGE_KEY_HEADER_STYLE)) {
    s_header_style = persist_read_int(STORAGE_KEY_HEADER_STYLE);
  }
  if (persist_exists(STORAGE_KEY_FOOTER_STYLE)) {
    s_footer_style = persist_read_int(STORAGE_KEY_FOOTER_STYLE);
  }
  if (persist_exists(STORAGE_KEY_DATE_FORMAT)) {
    s_date_format = persist_read_int(STORAGE_KEY_DATE_FORMAT);
  }
  if (persist_exists(STORAGE_KEY_LEADING_ZERO)) {
    s_leading_zero = persist_read_bool(STORAGE_KEY_LEADING_ZERO);
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
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_open(1024, 128);

  // Subscribe to services
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);
#if PBL_API_EXISTS(touch_service_subscribe)
  if (touch_service_is_enabled()) {
    touch_service_subscribe(touch_handler, NULL);
  }
#endif
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
#if PBL_API_EXISTS(touch_service_subscribe)
  if (touch_service_is_enabled()) {
    touch_service_unsubscribe();
  }
#endif
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  accel_tap_service_unsubscribe();
  tick_timer_service_unsubscribe();
  if (s_mode_timer) {
    app_timer_cancel(s_mode_timer);
    s_mode_timer = NULL;
  }
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
