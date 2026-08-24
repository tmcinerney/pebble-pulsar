#include <pebble.h>
#include "pulsar_palette.h"
#include "pulsar_layout.h"
#include "pulsar_matrix.h"
#include "pulsar_micro_bar.h"
#include "pulsar_brand.h"
#include "pulsar_audio.h"

/**
 * 1970s Hamilton Pulsar Watchface for Pebble Time / Pebble Time 2
 * 
 * Inspired by the Pulsar P1 / P2 / P3 "Time Computer".
 */

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
#define STORAGE_KEY_CHARGING_STYLE   10013
#define STORAGE_KEY_NIGHTLIGHT       10014
#define STORAGE_KEY_CYCLE_SLOT_1     10015
#define STORAGE_KEY_CYCLE_SLOT_2     10016
#define STORAGE_KEY_CYCLE_SLOT_3     10017
#define STORAGE_KEY_CYCLE_SLOT_4     10018
#define STORAGE_KEY_CYCLE_SLOT_5     10019
#define STORAGE_KEY_FLICK_SENSITIVITY 10024

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
#define MESSAGE_KEY_AppKeyChargingStyle  10013
#define MESSAGE_KEY_AppKeyNightlight     10014
#define MESSAGE_KEY_AppKeyCycleSlot1     10015
#define MESSAGE_KEY_AppKeyCycleSlot2     10016
#define MESSAGE_KEY_AppKeyCycleSlot3     10017
#define MESSAGE_KEY_AppKeyCycleSlot4     10018
#define MESSAGE_KEY_AppKeyCycleSlot5     10019
#define MESSAGE_KEY_AppKeyFlickSensitivity 10024
#endif

// Operating Modes
enum OperatingMode {
  MODE_ALWAYS_ON = 0,
  MODE_STEALTH = 1
};

// Flick Sensitivity
enum FlickSensitivity {
  FLICK_SENSITIVITY_BALANCED = 0,
  FLICK_SENSITIVITY_LOW = 1,
  FLICK_SENSITIVITY_HIGH = 2,
  FLICK_SENSITIVITY_TAPS_ONLY = 3,
  FLICK_SENSITIVITY_OFF = 4
};

// Charging Styles
enum ChargingStyle {
  CHARGING_STYLE_FLOW = 0,
  CHARGING_STYLE_CHASER = 1,
  CHARGING_STYLE_PULSE = 2,
  CHARGING_STYLE_MARQUEE = 3,
  CHARGING_STYLE_SOLID = 4,
  CHARGING_STYLE_OFF = 5
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

// Display Modes
enum DisplayMode {
  DISPLAY_MODE_TIME = 0,
  DISPLAY_MODE_SECONDS = 1,
  DISPLAY_MODE_DATE = 2,
  DISPLAY_MODE_STEPS = 3,
  DISPLAY_MODE_BATTERY = 4,
  DISPLAY_MODE_HEART_RATE = 5
};

// Flick Actions
enum FlickAction {
  FLICK_ACTION_CYCLE = 0,
  FLICK_ACTION_SECONDS = 1,
  FLICK_ACTION_DATE = 2,
  FLICK_ACTION_STEPS = 3,
  FLICK_ACTION_BATTERY = 4,
  FLICK_ACTION_HEART_RATE = 5,
  FLICK_ACTION_OFF = 6
};

// Hourly Vibe
enum HourlyVibe {
  HOURLY_VIBE_OFF = 0,
  HOURLY_VIBE_SINGLE = 1,
  HOURLY_VIBE_DOUBLE = 2
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
static bool s_bt_vibe = false;
static int s_bead_mode = BEAD_MODE_STEPS;
static bool s_italic_slant = true;
static int s_header_style = HEADER_STYLE_PULSAR;
static int s_footer_style = FOOTER_STYLE_TIME_COMPUTER;
static int s_date_format = DATE_FORMAT_MD;
static bool s_leading_zero = false;
static int s_step_goal = 10000;
static int s_charging_style = CHARGING_STYLE_FLOW;
static bool s_nightlight = false;
static int s_cycle_slot1 = 1; // Live Seconds
static int s_cycle_slot2 = 2; // Date
static int s_cycle_slot3 = 3; // Daily Steps
static int s_cycle_slot4 = 4; // Battery Level
static int s_cycle_slot5 = 0; // Heart Rate (Disabled by default, opt-in for HR devices)
static int s_flick_sensitivity = FLICK_SENSITIVITY_BALANCED;

static bool s_bluetooth_connected = true;
static int s_battery_level = 100;
static bool s_battery_charging = false;
static bool s_battery_plugged = false;
static bool s_charging_preview = false;
static AppTimer *s_preview_timer = NULL;
static AppTimer *s_charge_anim_timer = NULL;
static int s_anim_frame = 0;
static int s_last_vibe_hour = -1;

static void update_nightlight(void) {
  bool should_light = s_nightlight && (s_battery_charging || s_battery_plugged);
  light_enable(should_light);
}

static void charge_anim_timer_callback(void *data) {
  s_charge_anim_timer = NULL;
  bool is_animating = (s_battery_charging || s_battery_plugged || s_charging_preview) && 
                      (s_charging_style != CHARGING_STYLE_OFF) && 
                      (s_charging_style != CHARGING_STYLE_SOLID);
  if (is_animating) {
    s_anim_frame++;
    layer_mark_dirty(s_canvas_layer);
    s_charge_anim_timer = app_timer_register(60, charge_anim_timer_callback, NULL);
  }
}

static void update_charging_animation(void) {
  bool is_animating = (s_battery_charging || s_battery_plugged || s_charging_preview) && 
                      (s_charging_style != CHARGING_STYLE_OFF) && 
                      (s_charging_style != CHARGING_STYLE_SOLID);
  if (is_animating) {
    if (!s_charge_anim_timer) {
      s_charge_anim_timer = app_timer_register(60, charge_anim_timer_callback, NULL);
    }
  } else {
    if (s_charge_anim_timer) {
      app_timer_cancel(s_charge_anim_timer);
      s_charge_anim_timer = NULL;
    }
  }
}

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

static int get_heart_rate(void) {
  int hr = 0;
#if defined(PBL_HEALTH)
  HealthMetric metric = HealthMetricHeartRateBPM;
  time_t now = time(NULL);
  HealthServiceAccessibilityMask mask = health_service_metric_aggregate_averaged_accessible(
      metric, now, now, HealthAggregationAvg, HealthServiceTimeScopeOnce);
  if (mask & HealthServiceAccessibilityMaskAvailable) {
    hr = (int)health_service_peek_current_value(metric);
  } else {
    hr = (int)health_service_peek_current_value(metric);
  }
#endif
  return hr;
}

static void preview_timer_callback(void *data) {
  s_preview_timer = NULL;
  s_charging_preview = false;
  update_nightlight();
  update_charging_animation();
  layer_mark_dirty(s_canvas_layer);
}

static void mode_timer_callback(void *data) {
  s_mode_timer = NULL;
  s_stealth_awake = false;
  s_display_mode = DISPLAY_MODE_TIME;
  layer_mark_dirty(s_canvas_layer);
}

static void trigger_display_change(int mode) {
  light_enable_interaction();
  s_display_mode = mode;
  if (s_mode_timer) {
    app_timer_cancel(s_mode_timer);
  }
  s_mode_timer = app_timer_register(WAKE_DURATION_MS, mode_timer_callback, NULL);
  layer_mark_dirty(s_canvas_layer);
}

static void advance_display_mode_dir(int dir) {
  if (s_flick_action == FLICK_ACTION_OFF || s_flick_sensitivity == FLICK_SENSITIVITY_OFF) {
    return;
  }
  if (s_flick_action == FLICK_ACTION_SECONDS) {
    trigger_display_change(DISPLAY_MODE_SECONDS);
    return;
  } else if (s_flick_action == FLICK_ACTION_DATE) {
    trigger_display_change(DISPLAY_MODE_DATE);
    return;
  } else if (s_flick_action == FLICK_ACTION_STEPS) {
    trigger_display_change(DISPLAY_MODE_STEPS);
    return;
  } else if (s_flick_action == FLICK_ACTION_BATTERY) {
    trigger_display_change(DISPLAY_MODE_BATTERY);
    return;
  } else if (s_flick_action == FLICK_ACTION_HEART_RATE) {
#if defined(PBL_HEALTH)
    trigger_display_change(DISPLAY_MODE_HEART_RATE);
#else
    trigger_display_change(DISPLAY_MODE_TIME);
#endif
    return;
  }

  int active_slots[5];
  int active_count = 0;
  int raw_slots[5] = {s_cycle_slot1, s_cycle_slot2, s_cycle_slot3, s_cycle_slot4, s_cycle_slot5};
  for (int i = 0; i < 5; i++) {
    int slot_val = raw_slots[i];
    if (slot_val >= 1 && slot_val <= 5) {
#if !defined(PBL_HEALTH)
      if (slot_val == DISPLAY_MODE_HEART_RATE) {
        continue;
      }
#endif
      bool duplicate = false;
      for (int j = 0; j < active_count; j++) {
        if (active_slots[j] == slot_val) {
          duplicate = true;
          break;
        }
      }
      if (!duplicate) {
        active_slots[active_count++] = slot_val;
      }
    }
  }

  if (active_count == 0) {
    return;
  }

  int current_index = -1;
  for (int i = 0; i < active_count; i++) {
    if (active_slots[i] == s_display_mode) {
      current_index = i;
      break;
    }
  }

  int next_mode;
  if (current_index == -1) {
    next_mode = (dir >= 0) ? active_slots[0] : active_slots[active_count - 1];
  } else {
    int next_index = current_index + dir;
    if (next_index >= active_count || next_index < 0) {
      next_mode = DISPLAY_MODE_TIME;
    } else {
      next_mode = active_slots[next_index];
    }
  }

  trigger_display_change(next_mode);
}

static time_t s_last_gesture_time_s = 0;
static uint16_t s_last_gesture_time_ms = 0;
static bool s_accel_subscribed = false;

static void execute_gesture_action_dir(int dir) {
  if (s_flick_sensitivity == FLICK_SENSITIVITY_OFF || s_flick_action == FLICK_ACTION_OFF) {
    return;
  }
  light_enable_interaction();
  if (s_operating_mode == MODE_STEALTH) {
    if (!s_stealth_awake) {
      s_stealth_awake = true;
      if (s_flick_action == FLICK_ACTION_SECONDS) {
        s_display_mode = DISPLAY_MODE_SECONDS;
      } else if (s_flick_action == FLICK_ACTION_DATE) {
        s_display_mode = DISPLAY_MODE_DATE;
      } else if (s_flick_action == FLICK_ACTION_STEPS) {
        s_display_mode = DISPLAY_MODE_STEPS;
      } else if (s_flick_action == FLICK_ACTION_BATTERY) {
        s_display_mode = DISPLAY_MODE_BATTERY;
      } else if (s_flick_action == FLICK_ACTION_HEART_RATE) {
        s_display_mode = DISPLAY_MODE_HEART_RATE;
      } else {
        s_display_mode = DISPLAY_MODE_TIME;
      }
      if (s_mode_timer) {
        app_timer_cancel(s_mode_timer);
      }
      s_mode_timer = app_timer_register(WAKE_DURATION_MS, mode_timer_callback, NULL);
      layer_mark_dirty(s_canvas_layer);
    } else {
      advance_display_mode_dir(dir);
    }
  } else {
    advance_display_mode_dir(dir);
  }
}

static void handle_gesture_dir(int dir, int debounce_ms) {
  time_t now_s;
  uint16_t now_ms = time_ms(&now_s, NULL);
  int elapsed_ms = (int)((now_s - s_last_gesture_time_s) * 1000 + (now_ms - s_last_gesture_time_ms));
  if (elapsed_ms < debounce_ms && elapsed_ms >= 0) {
    return;
  }
  s_last_gesture_time_s = now_s;
  s_last_gesture_time_ms = now_ms;

  execute_gesture_action_dir(dir);
}

static void accel_data_handler(AccelData *data, uint32_t num_samples) {
  if (s_flick_sensitivity == FLICK_SENSITIVITY_OFF || s_flick_action == FLICK_ACTION_OFF) {
    return;
  }

  // Taps Only: Require sharp Z-axis perpendicular impact with minimal wrist rotation
  if (s_flick_sensitivity == FLICK_SENSITIVITY_TAPS_ONLY) {
    for (uint32_t i = 1; i < num_samples; i++) {
      int dx = abs(data[i].x - data[i - 1].x);
      int dy = abs(data[i].y - data[i - 1].y);
      int dz = abs(data[i].z - data[i - 1].z);
      if (dz > 950 && dx < 350 && dy < 350) {
        APP_LOG(APP_LOG_LEVEL_INFO, "Tap detected: dz=%d, dx=%d, dy=%d", dz, dx, dy);
        handle_gesture_dir(1, 650);
        break;
      }
    }
    return;
  }

  int threshold_xy = 1100;
  int threshold_z = 1200;
  int debounce_ms = 1000;

  if (s_flick_sensitivity == FLICK_SENSITIVITY_LOW) {
    threshold_xy = 1600;
    threshold_z = 1700;
    debounce_ms = 1350;
  } else if (s_flick_sensitivity == FLICK_SENSITIVITY_HIGH) {
    threshold_xy = 700;
    threshold_z = 800;
    debounce_ms = 650;
  }

  for (uint32_t i = 1; i < num_samples; i++) {
    int dx = data[i].x - data[i - 1].x;
    int dy = data[i].y - data[i - 1].y;
    int dz = data[i].z - data[i - 1].z;
    if (abs(dx) > threshold_xy || abs(dy) > threshold_xy || abs(dz) > threshold_z) {
      int dir = (dx < -300) ? -1 : 1;
      APP_LOG(APP_LOG_LEVEL_INFO, "Flick detected: dir=%d, dx=%d, dy=%d, dz=%d (sens=%d)", dir, dx, dy, dz, s_flick_sensitivity);
      handle_gesture_dir(dir, debounce_ms);
      break;
    }
  }
}

static void update_accel_subscription(void) {
  bool gestures_enabled = (s_flick_action != FLICK_ACTION_OFF) &&
                          (s_flick_sensitivity != FLICK_SENSITIVITY_OFF);
  if (gestures_enabled) {
    if (!s_accel_subscribed) {
      accel_data_service_subscribe(5, accel_data_handler);
      accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
      s_accel_subscribed = true;
    }
  } else {
    if (s_accel_subscribed) {
      accel_data_service_unsubscribe();
      s_accel_subscribed = false;
    }
  }
}

#if PBL_API_EXISTS(touch_service_subscribe)
static void touch_handler(const TouchEvent *event, void *context) {
  if (event->type == TouchEvent_Touchdown) {
    handle_gesture_dir(1, 500);
  }
}
#endif

static void bluetooth_callback(bool connected) {
  if (s_bt_vibe && !connected && s_bluetooth_connected) {
    vibes_double_pulse();
#if PBL_API_EXISTS(speaker_play_tone)
    speaker_play_tone(880, 120, 70, SpeakerWaveformSawtooth);
#endif
  }
  s_bluetooth_connected = connected;
  layer_mark_dirty(s_canvas_layer);
}

static void battery_callback(BatteryChargeState state) {
  s_battery_level = state.charge_percent;
  s_battery_charging = state.is_charging;
  s_battery_plugged = state.is_plugged;
  update_nightlight();
  update_charging_animation();
  layer_mark_dirty(s_canvas_layer);
}

#if defined(PBL_HEALTH)
static void health_handler(HealthEventType event, void *context) {
  if (event == HealthEventMovementUpdate || event == HealthEventHeartRateUpdate) {
    layer_mark_dirty(s_canvas_layer);
  }
}
#endif

static void draw_step_beads(GContext *ctx, GRect bounds, const Colorway *palette, int steps, int sec, uint16_t now_ms) {
  bool is_charging_active = (s_battery_charging || s_battery_plugged || s_charging_preview) && (s_charging_style != CHARGING_STYLE_OFF);
  if (s_bead_mode == BEAD_MODE_OFF && !is_charging_active) return;
  
  bool beads_lit[NUM_MICRO_BEADS] = {false};
  bool on_power = (s_battery_charging || s_battery_plugged);
  bool is_active = (s_operating_mode == MODE_ALWAYS_ON) || s_stealth_awake || (on_power && s_nightlight) || s_charging_preview;
  int goal = s_step_goal > 0 ? s_step_goal : 10000;
  
  for (int i = 0; i < NUM_MICRO_BEADS; i++) {
    bool lit = false;
    
    if (is_charging_active) {
      if (s_charging_style == CHARGING_STYLE_CHASER) {
        int total_ms = ((sec % 2) * 1000) + now_ms;
        int step = (total_ms * 18) / 2000;
        if (step > 17) step = 17;
        int active_idx = (step < 10) ? step : (18 - step);
        bool is_head = (i == active_idx);
        bool is_tail = (i == active_idx - 1) || (i == active_idx + 1);
        lit = is_active && (is_head || is_tail);
      } else if (s_charging_style == CHARGING_STYLE_PULSE) {
        int current_beads = (s_battery_level + 9) / 10;
        if (current_beads > NUM_MICRO_BEADS) current_beads = NUM_MICRO_BEADS;
        if (current_beads < 1) current_beads = 1;
        bool pulse_on = (now_ms < 180) || (now_ms >= 300 && now_ms < 480);
        lit = is_active && (i < current_beads) && pulse_on;
      } else if (s_charging_style == CHARGING_STYLE_MARQUEE) {
        bool phase = (now_ms < 500);
        lit = is_active && ((i % 2 == 0) == phase);
      } else if (s_charging_style == CHARGING_STYLE_FLOW) {
        int current_beads = s_battery_level / 10;
        if (current_beads >= NUM_MICRO_BEADS) {
          int wave_idx = (now_ms * NUM_MICRO_BEADS) / 1000;
          lit = is_active && (i != wave_idx);
        } else {
          if (i < current_beads) {
            lit = is_active;
          } else {
            int remaining = NUM_MICRO_BEADS - current_beads;
            int cycle_pos = (remaining > 0) ? ((now_ms * (remaining + 2)) / 1000) : 0;
            lit = is_active && (i == (current_beads + cycle_pos));
          }
        }
      } else if (s_charging_style == CHARGING_STYLE_SOLID) {
        int current_beads = (s_battery_level + 9) / 10;
        if (current_beads > NUM_MICRO_BEADS) current_beads = NUM_MICRO_BEADS;
        lit = is_active && (i < current_beads);
      }
    } else {
      if (s_bead_mode == BEAD_MODE_STEPS) {
        if (steps >= goal) {
          if (steps >= 2 * goal) {
            bool flash = (sec % 2 == 0);
            lit = is_active && flash;
          } else {
            int surplus_steps = steps - goal;
            int lap2_beads = (surplus_steps * NUM_MICRO_BEADS) / goal;
            if (i < lap2_beads) {
              bool lap2_pulse = (sec % 2 == 0);
              lit = is_active && lap2_pulse;
            } else {
              lit = is_active;
            }
          }
        } else {
          int threshold = ((i + 1) * goal) / NUM_MICRO_BEADS;
          lit = is_active && (steps >= threshold);
        }
      } else if (s_bead_mode == BEAD_MODE_BATTERY) {
        int threshold = (i + 1) * 10;
        lit = is_active && (s_battery_level >= threshold);
      }
    }
    beads_lit[i] = lit;
  }
  
  pulsar_draw_micro_beads(ctx, bounds, palette, is_active, beads_lit);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  Colorway active_palette = pulsar_get_palette(s_colorway % NUM_COLORWAYS);
  const Colorway *palette = &active_palette;
  bool on_power = (s_battery_charging || s_battery_plugged);
  bool is_active = (s_operating_mode == MODE_ALWAYS_ON) || s_stealth_awake || (on_power && s_nightlight) || s_charging_preview;
  
  // 1. Background Fill
  graphics_context_set_fill_color(ctx, palette->outer_bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // 2. Brand Header
  if (s_header_style != HEADER_STYLE_NONE) {
    const char *header_text = "P U L S A R";
    if (s_header_style == HEADER_STYLE_HAMILTON) {
      header_text = "H A M I L T O N";
    } else if (s_header_style == HEADER_STYLE_SOLID_STATE) {
      header_text = "S O L I D   S T A T E";
    }
    pulsar_draw_header(ctx, bounds, header_text, palette);
  }

  // 3. Annunciator Dots
  time_t now;
  uint16_t now_ms = 0;
  time_ms(&now, &now_ms);
  struct tm *tick_time = localtime(&now);
  int steps = get_step_count();

  bool bt_disc = !s_bluetooth_connected;
  bool bat_alert = (s_battery_charging || s_battery_plugged) ? (s_battery_charging ? (now_ms < 500) : true) : (s_battery_level <= 20);
  pulsar_draw_annunciators(ctx, bounds, palette, bt_disc, bat_alert, s_battery_charging || s_battery_plugged);

  // 4. Data Extraction & Display Engine
  if (s_display_mode == DISPLAY_MODE_STEPS && is_active) {
    int clamped_steps = steps > 99999 ? 99999 : steps;
    int s_digits[5] = {
      (clamped_steps / 10000) % 10,
      (clamped_steps / 1000) % 10,
      (clamped_steps / 100) % 10,
      (clamped_steps / 10) % 10,
      clamped_steps % 10
    };
    pulsar_draw_5digits(ctx, bounds, s_digits, palette, is_active, s_italic_slant);
  } else if (s_display_mode == DISPLAY_MODE_HEART_RATE && is_active) {
    int hr = get_heart_rate();
    int h_digits[5] = {GLYPH_HEART, GLYPH_BLANK, GLYPH_BLANK, GLYPH_BLANK, GLYPH_BLANK};
    if (hr > 0) {
      if (hr >= 100) {
        h_digits[2] = (hr / 100) % 10;
        h_digits[3] = (hr / 10) % 10;
        h_digits[4] = hr % 10;
      } else {
        h_digits[3] = (hr / 10) % 10;
        h_digits[4] = hr % 10;
      }
    } else {
      h_digits[3] = GLYPH_DASH;
      h_digits[4] = GLYPH_DASH;
    }
    pulsar_draw_5digits(ctx, bounds, h_digits, palette, is_active, s_italic_slant);
  } else if (s_display_mode == DISPLAY_MODE_SECONDS) {
    int secs = tick_time->tm_sec;
    int sec_tens = is_active ? (secs / 10) : GLYPH_BLANK;
    int sec_ones = is_active ? (secs % 10) : GLYPH_BLANK;
    
    int digit_span_x = (DIGIT_WIDTH - 1) * DOT_SPACING_X;
    int slant_scale = (bounds.size.w > 180) ? 7 : 5;
    int max_slant = s_italic_slant ? slant_scale : 0;
    int sec_digit_gap = bounds.size.w > 180 ? 14 : 10;
    int sec_colon_gap = bounds.size.w > 180 ? 22 : 16;
    int sec_total_width = (digit_span_x * 2) + sec_digit_gap + sec_colon_gap + max_slant;
    int sec_start_x = (bounds.size.w - sec_total_width) / 2;
    
    int sec_d3_x = sec_start_x + sec_colon_gap;
    int sec_d4_x = sec_d3_x + digit_span_x + sec_digit_gap;
    
    pulsar_draw_digit(ctx, sec_d3_x, TOP_MARGIN, sec_tens, palette, is_active, bounds.size.w, s_italic_slant);
    pulsar_draw_digit(ctx, sec_d4_x, TOP_MARGIN, sec_ones, palette, is_active, bounds.size.w, s_italic_slant);
    
    int sec_colon_base_x = sec_start_x + (sec_colon_gap / 2);
    pulsar_draw_colon(ctx, sec_colon_base_x, TOP_MARGIN, palette, is_active, true, bounds.size.w, s_italic_slant);
  } else {
    // 4-Digit Layout for Time, Date, Battery
    int d1 = GLYPH_BLANK, d2 = GLYPH_BLANK, d3 = GLYPH_BLANK, d4 = GLYPH_BLANK;
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
          d4 = GLYPH_PERCENT;
        } else {
          d2 = bat / 10;
          d3 = bat % 10;
          d4 = GLYPH_PERCENT;
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
          d1 = s_leading_zero ? 0 : GLYPH_BLANK;
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

    bool colon_lit = show_colon && is_active && (!colon_blinking || (tick_time->tm_sec % 2 == 0));
    pulsar_draw_4digits(ctx, bounds, d1, d2, d3, d4, show_colon, colon_lit, palette, is_active, s_italic_slant);
    
    // Middle separator dot for Date Mode
    if (s_display_mode == DISPLAY_MODE_DATE && is_active) {
      int digit_span_x = (DIGIT_WIDTH - 1) * DOT_SPACING_X;
      int slant_scale = (bounds.size.w > 180) ? 7 : 5;
      int max_slant = s_italic_slant ? slant_scale : 0;
      int total_width = (digit_span_x * 4) + (DIGIT_GAP * 2) + COLON_GAP + max_slant;
      int start_x = (bounds.size.w - total_width) / 2;
      int d2_right = start_x + (digit_span_x * 2) + DIGIT_GAP;
      int colon_base_x = d2_right + (COLON_GAP / 2);
      int colon_slant2 = s_italic_slant ? (((DIGIT_HEIGHT - 1 - 4) * slant_scale) / (DIGIT_HEIGHT - 1)) : 0;
      int colon_x2 = colon_base_x + colon_slant2;
      int colon_y2 = TOP_MARGIN + (DOT_SPACING_Y * 4);
      graphics_context_set_fill_color(ctx, palette->lit);
      graphics_fill_circle(ctx, GPoint(colon_x2, colon_y2), DOT_RADIUS);
    }
    
    // AM/PM Indicator Dot
    if (is_active && !clock_is_24h_style() && s_display_mode == DISPLAY_MODE_TIME) {
      bool is_pm = tick_time->tm_hour >= 12;
      if (is_pm) {
        int digit_span_x = (DIGIT_WIDTH - 1) * DOT_SPACING_X;
        int slant_scale = (bounds.size.w > 180) ? 7 : 5;
        int max_slant = s_italic_slant ? slant_scale : 0;
        int total_width = (digit_span_x * 4) + (DIGIT_GAP * 2) + COLON_GAP + max_slant;
        int start_x = (bounds.size.w - total_width) / 2;
        graphics_context_set_fill_color(ctx, palette->lit);
        graphics_fill_circle(ctx, GPoint(start_x, TOP_MARGIN + (DIGIT_HEIGHT * DOT_SPACING_Y) + 4), INDICATOR_RADIUS);
      }
    }
  }

  // 5. 10-Dot Micro-LED Progress Bar
  draw_step_beads(ctx, bounds, palette, steps, tick_time->tm_sec, now_ms);

  // 6. Vintage Space-Age Footer
  if (s_footer_style != FOOTER_STYLE_NONE) {
    const char *footer_text = "T I M E   C O M P U T E R";
    if (s_charging_preview) {
      if (s_charging_style == CHARGING_STYLE_FLOW) {
        footer_text = "F L O W   P R E V I E W";
      } else if (s_charging_style == CHARGING_STYLE_CHASER) {
        footer_text = "C H A S E R   P R E V I E W";
      } else if (s_charging_style == CHARGING_STYLE_PULSE) {
        footer_text = "P U L S E   P R E V I E W";
      } else if (s_charging_style == CHARGING_STYLE_MARQUEE) {
        footer_text = "M A R Q U E E   P R E V I E W";
      } else if (s_charging_style == CHARGING_STYLE_SOLID) {
        footer_text = "S O L I D   G A U G E";
      } else {
        footer_text = "A N I M   O F F";
      }
    } else if (s_display_mode == DISPLAY_MODE_STEPS && is_active) {
      int goal = s_step_goal > 0 ? s_step_goal : 10000;
      if (steps >= 2 * goal) {
        footer_text = "★   2 X   G O A L   ★";
      } else if (steps >= goal) {
        footer_text = "★   G O A L   M E T   ★";
      } else {
        footer_text = "S T E P S";
      }
    } else if (s_display_mode == DISPLAY_MODE_BATTERY && is_active) {
      footer_text = "B A T T E R Y";
    } else if (s_display_mode == DISPLAY_MODE_DATE && is_active) {
      footer_text = "D A T E";
    } else if (s_display_mode == DISPLAY_MODE_SECONDS && is_active) {
      footer_text = "S E C O N D S";
    } else if (s_display_mode == DISPLAY_MODE_HEART_RATE && is_active) {
      footer_text = "H E A R T   R A T E";
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
    pulsar_draw_footer(ctx, bounds, footer_text, palette);
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (units_changed & HOUR_UNIT) {
    if (s_hourly_vibe != HOURLY_VIBE_OFF && tick_time->tm_hour != s_last_vibe_hour) {
      s_last_vibe_hour = tick_time->tm_hour;
      if (s_hourly_vibe == HOURLY_VIBE_SINGLE) {
        vibes_short_pulse();
#if PBL_API_EXISTS(speaker_play_tone)
        speaker_play_tone(1760, 80, 50, SpeakerWaveformSquare);
#endif
      } else if (s_hourly_vibe == HOURLY_VIBE_DOUBLE) {
        vibes_double_pulse();
#if PBL_API_EXISTS(speaker_play_tone)
        speaker_play_tone(2093, 100, 50, SpeakerWaveformSquare);
#endif
      }
    }
  }

  if (s_nightlight && (s_battery_charging || s_battery_plugged)) {
    light_enable(true);
  }

  bool animating_charge = (s_battery_charging || s_battery_plugged || s_charging_preview) && (s_charging_style != CHARGING_STYLE_OFF) && (s_charging_style != CHARGING_STYLE_SOLID);
  if (s_display_mode == DISPLAY_MODE_SECONDS || animating_charge || (s_operating_mode == MODE_ALWAYS_ON && (units_changed & SECOND_UNIT))) {
    layer_mark_dirty(s_canvas_layer);
  } else if (units_changed & MINUTE_UNIT) {
    layer_mark_dirty(s_canvas_layer);
  }
}

static int tuple_to_int(Tuple *tuple, int default_val) {
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
  for (Tuple *t = dict_read_first(iterator); t != NULL; t = dict_read_next(iterator)) {
    uint32_t key = t->key;
    if (key == MESSAGE_KEY_AppKeyOperatingMode) {
      s_operating_mode = tuple_to_int(t, s_operating_mode);
      persist_write_int(STORAGE_KEY_OPERATING_MODE, s_operating_mode);
    } else if (key == MESSAGE_KEY_AppKeyColorway) {
      s_colorway = tuple_to_int(t, s_colorway);
      if (s_colorway < 0 || s_colorway >= NUM_COLORWAYS) s_colorway = 0;
      persist_write_int(STORAGE_KEY_COLORWAY, s_colorway);
    } else if (key == MESSAGE_KEY_AppKeyFlickAction) {
      s_flick_action = tuple_to_int(t, s_flick_action);
      persist_write_int(STORAGE_KEY_FLICK_ACTION, s_flick_action);
    } else if (key == MESSAGE_KEY_AppKeyHourlyVibe) {
      s_hourly_vibe = tuple_to_int(t, s_hourly_vibe);
      persist_write_int(STORAGE_KEY_HOURLY_VIBE, s_hourly_vibe);
    } else if (key == MESSAGE_KEY_AppKeyBtVibe) {
      s_bt_vibe = tuple_to_bool(t, s_bt_vibe);
      persist_write_bool(STORAGE_KEY_BT_VIBE, s_bt_vibe);
    } else if (key == MESSAGE_KEY_AppKeyBeadMode) {
      s_bead_mode = tuple_to_int(t, s_bead_mode);
      persist_write_int(STORAGE_KEY_BEAD_MODE, s_bead_mode);
    } else if (key == MESSAGE_KEY_AppKeyShowStepBeads) {
      bool show = tuple_to_bool(t, true);
      s_bead_mode = show ? BEAD_MODE_STEPS : BEAD_MODE_OFF;
      persist_write_int(STORAGE_KEY_BEAD_MODE, s_bead_mode);
    } else if (key == MESSAGE_KEY_AppKeyItalicSlant) {
      s_italic_slant = tuple_to_bool(t, s_italic_slant);
      persist_write_bool(STORAGE_KEY_ITALIC_SLANT, s_italic_slant);
    } else if (key == MESSAGE_KEY_AppKeyHeaderStyle) {
      s_header_style = tuple_to_int(t, s_header_style);
      persist_write_int(STORAGE_KEY_HEADER_STYLE, s_header_style);
    } else if (key == MESSAGE_KEY_AppKeyFooterStyle) {
      s_footer_style = tuple_to_int(t, s_footer_style);
      persist_write_int(STORAGE_KEY_FOOTER_STYLE, s_footer_style);
    } else if (key == MESSAGE_KEY_AppKeyDateFormat) {
      s_date_format = tuple_to_int(t, s_date_format);
      persist_write_int(STORAGE_KEY_DATE_FORMAT, s_date_format);
    } else if (key == MESSAGE_KEY_AppKeyLeadingZero) {
      s_leading_zero = tuple_to_bool(t, s_leading_zero);
      persist_write_bool(STORAGE_KEY_LEADING_ZERO, s_leading_zero);
    } else if (key == MESSAGE_KEY_AppKeyStepGoal) {
      s_step_goal = tuple_to_int(t, s_step_goal);
      if (s_step_goal <= 0) s_step_goal = 10000;
      persist_write_int(STORAGE_KEY_STEP_GOAL, s_step_goal);
    } else if (key == MESSAGE_KEY_AppKeyChargingStyle) {
      int new_style = tuple_to_int(t, s_charging_style);
      bool changed = (new_style != s_charging_style);
      s_charging_style = new_style;
      persist_write_int(STORAGE_KEY_CHARGING_STYLE, s_charging_style);
      
      if (changed && (s_charging_style != CHARGING_STYLE_OFF)) {
        s_charging_preview = true;
        s_anim_frame = 0;
        if (s_preview_timer) {
          app_timer_cancel(s_preview_timer);
        }
        s_preview_timer = app_timer_register(12000, preview_timer_callback, NULL);
      }
      update_charging_animation();
    } else if (key == MESSAGE_KEY_AppKeyNightlight) {
      bool new_nightlight = tuple_to_bool(t, s_nightlight);
      bool changed = (new_nightlight != s_nightlight);
      s_nightlight = new_nightlight;
      persist_write_bool(STORAGE_KEY_NIGHTLIGHT, s_nightlight);
      
      if (changed && s_nightlight) {
        light_enable(true);
        if (!s_battery_charging && !s_battery_plugged) {
          if (s_preview_timer) app_timer_cancel(s_preview_timer);
          s_preview_timer = app_timer_register(5000, preview_timer_callback, NULL);
        }
      } else {
        update_nightlight();
      }
    } else if (key == MESSAGE_KEY_AppKeyCycleSlot1) {
      s_cycle_slot1 = tuple_to_int(t, s_cycle_slot1);
      persist_write_int(STORAGE_KEY_CYCLE_SLOT_1, s_cycle_slot1);
    } else if (key == MESSAGE_KEY_AppKeyCycleSlot2) {
      s_cycle_slot2 = tuple_to_int(t, s_cycle_slot2);
      persist_write_int(STORAGE_KEY_CYCLE_SLOT_2, s_cycle_slot2);
    } else if (key == MESSAGE_KEY_AppKeyCycleSlot3) {
      s_cycle_slot3 = tuple_to_int(t, s_cycle_slot3);
      persist_write_int(STORAGE_KEY_CYCLE_SLOT_3, s_cycle_slot3);
    } else if (key == MESSAGE_KEY_AppKeyCycleSlot4) {
      s_cycle_slot4 = tuple_to_int(t, s_cycle_slot4);
      persist_write_int(STORAGE_KEY_CYCLE_SLOT_4, s_cycle_slot4);
    } else if (key == MESSAGE_KEY_AppKeyCycleSlot5) {
      s_cycle_slot5 = tuple_to_int(t, s_cycle_slot5);
      persist_write_int(STORAGE_KEY_CYCLE_SLOT_5, s_cycle_slot5);
    } else if (key == MESSAGE_KEY_AppKeyFlickSensitivity) {
      s_flick_sensitivity = tuple_to_int(t, s_flick_sensitivity);
      persist_write_int(STORAGE_KEY_FLICK_SENSITIVITY, s_flick_sensitivity);
    }
  }
  update_accel_subscription();
  APP_LOG(APP_LOG_LEVEL_INFO, "Pulsar synced: action=%d, sensitivity=%d, mode=%d", s_flick_action, s_flick_sensitivity, s_operating_mode);
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
  if (persist_exists(STORAGE_KEY_CHARGING_STYLE)) {
    s_charging_style = persist_read_int(STORAGE_KEY_CHARGING_STYLE);
  }
  if (persist_exists(STORAGE_KEY_NIGHTLIGHT)) {
    s_nightlight = persist_read_bool(STORAGE_KEY_NIGHTLIGHT);
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
  }
  if (persist_exists(STORAGE_KEY_CYCLE_SLOT_1)) {
    s_cycle_slot1 = persist_read_int(STORAGE_KEY_CYCLE_SLOT_1);
  }
  if (persist_exists(STORAGE_KEY_CYCLE_SLOT_2)) {
    s_cycle_slot2 = persist_read_int(STORAGE_KEY_CYCLE_SLOT_2);
  }
  if (persist_exists(STORAGE_KEY_CYCLE_SLOT_3)) {
    s_cycle_slot3 = persist_read_int(STORAGE_KEY_CYCLE_SLOT_3);
  }
  if (persist_exists(STORAGE_KEY_CYCLE_SLOT_4)) {
    s_cycle_slot4 = persist_read_int(STORAGE_KEY_CYCLE_SLOT_4);
  }
  if (persist_exists(STORAGE_KEY_CYCLE_SLOT_5)) {
    s_cycle_slot5 = persist_read_int(STORAGE_KEY_CYCLE_SLOT_5);
  }
  if (persist_exists(STORAGE_KEY_FLICK_SENSITIVITY)) {
    s_flick_sensitivity = persist_read_int(STORAGE_KEY_FLICK_SENSITIVITY);
  }
  update_accel_subscription();
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
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_open(1024, 128);

  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  update_accel_subscription();

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
  
  s_bluetooth_connected = connection_service_peek_pebble_app_connection();
  BatteryChargeState charge_state = battery_state_service_peek();
  s_battery_level = charge_state.charge_percent;
  s_battery_charging = charge_state.is_charging;
  s_battery_plugged = charge_state.is_plugged;
  update_nightlight();
  update_charging_animation();
}

static void deinit(void) {
  light_enable(false);
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
  if (s_accel_subscribed) {
    accel_data_service_unsubscribe();
    s_accel_subscribed = false;
  }
  tick_timer_service_unsubscribe();
  if (s_mode_timer) {
    app_timer_cancel(s_mode_timer);
    s_mode_timer = NULL;
  }
  if (s_preview_timer) {
    app_timer_cancel(s_preview_timer);
    s_preview_timer = NULL;
  }
  if (s_charge_anim_timer) {
    app_timer_cancel(s_charge_anim_timer);
    s_charge_anim_timer = NULL;
  }
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
