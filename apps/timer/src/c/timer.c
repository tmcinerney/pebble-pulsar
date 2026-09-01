#include <pebble.h>
#include "pulsar_palette.h"
#include "pulsar_layout.h"
#include "pulsar_matrix.h"
#include "pulsar_micro_bar.h"
#include "pulsar_brand.h"
#include "pulsar_audio.h"

#define WAKEUP_COOKIE_TIMER 1970

enum EditMode {
  EDIT_NONE = 0,
  EDIT_MINUTES = 1,
  EDIT_SECONDS = 2
};

#define NUM_PRESETS 16

typedef struct {
  const char *name;
  int seconds;
} TimerPreset;

static const TimerPreset PRESETS[NUM_PRESETS] = {
  { "0 0 : 1 0   P R E S E T", 10 },
  { "0 0 : 3 0   P R E S E T", 30 },
  { "0 0 : 4 5   P R E S E T", 45 },
  { "0 1 : 0 0   P R E S E T", 60 },
  { "0 2 : 0 0   P R E S E T", 120 },
  { "0 3 : 0 0   P R E S E T", 180 },
  { "0 5 : 0 0   P R E S E T", 300 },
  { "1 0 : 0 0   P R E S E T", 600 },
  { "1 5 : 0 0   P R E S E T", 900 },
  { "2 0 : 0 0   P R E S E T", 1200 },
  { "P O M O D O R O   2 5 M", 1500 },
  { "3 0 : 0 0   P R E S E T", 1800 },
  { "4 5 : 0 0   P R E S E T", 2700 },
  { "6 0 : 0 0   P R E S E T", 3600 },
  { "9 0 : 0 0   P R E S E T", 5400 },
  { "C U S T O M   T I M E R", -1 }
};

#define STORAGE_KEY_COLORWAY       10001
#define STORAGE_KEY_ITALIC_SLANT   10005
#define STORAGE_KEY_SHOW_GHOST     10025
#define STORAGE_KEY_LED_GLOW       10028
#define STORAGE_KEY_BACKLIGHT_TINT 10029
#define STORAGE_KEY_AUDIO_ENABLED  10020
#define STORAGE_KEY_VIBE_ENABLED   10021
#define STORAGE_KEY_PRESET_IDX     10040
#define STORAGE_KEY_TARGET_EPOCH   10041
#define STORAGE_KEY_TOTAL_DURATION 10042
#define STORAGE_KEY_REMAINING_SEC  10043
#define STORAGE_KEY_IS_RUNNING     10044
#define STORAGE_KEY_WAKEUP_ID      10045
#define STORAGE_KEY_CUSTOM_SEC     10046

#ifndef MESSAGE_KEY_AppKeyColorway
#define MESSAGE_KEY_AppKeyColorway       10001
#define MESSAGE_KEY_AppKeyItalicSlant   10005
#define MESSAGE_KEY_AppKeyAudioEnabled  10020
#define MESSAGE_KEY_AppKeyVibeEnabled   10021
#endif

static Window *s_main_window;
static Layer *s_canvas_layer;
static AppTimer *s_tick_timer = NULL;
static AppTimer *s_alarm_pulse_timer = NULL;
static AppTimer *s_blink_timer = NULL;

static int s_colorway = COLORWAY_VIBRANT_RUBY;
// Ghost dots off by default across the suite: they grey out the gaps between lit dots and
// flatten the glow. Matches the watchface so a fresh install looks consistent.
static bool s_show_ghost = false;
static bool s_led_glow = true;
static bool s_backlight_tint = false;
static bool s_italic_slant = true;
static bool s_audio_enabled = true;
static bool s_vibe_enabled = true;

static int s_preset_index = 6; // Default 5 min preset
static int s_custom_sec = 30;  // Default custom duration
static int s_total_duration_sec = 300;
static int s_remaining_sec = 300;
static bool s_is_running = false;
static bool s_is_paused = false;
static bool s_is_alarm_firing = false;
static time_t s_target_epoch = 0;
static WakeupId s_wakeup_id = -1;
static int s_alarm_flash_count = 0;

static enum EditMode s_edit_mode = EDIT_NONE;
static int s_edit_minutes = 0;
static int s_edit_seconds = 30;
static bool s_blink_state = true;

static int get_preset_duration(int preset_idx) {
  if (preset_idx < 0 || preset_idx >= NUM_PRESETS) preset_idx = 6;
  if (PRESETS[preset_idx].seconds > 0) {
    return PRESETS[preset_idx].seconds;
  }
  return (s_custom_sec > 0) ? s_custom_sec : 30;
}

static void update_remaining_from_clock(void) {
  if (!s_is_running) return;
  time_t now = time(NULL);
  int diff = (int)(s_target_epoch - now);
  if (diff <= 0) {
    s_remaining_sec = 0;
  } else {
    s_remaining_sec = diff;
  }
}

static void trigger_alarm_firing(void);

static void blink_timer_callback(void *data) {
  s_blink_timer = NULL;
  if (s_edit_mode != EDIT_NONE) {
    s_blink_state = !s_blink_state;
    layer_mark_dirty(s_canvas_layer);
    s_blink_timer = app_timer_register(400, blink_timer_callback, NULL);
  }
}

static void update_blink_loop(void) {
  if (s_edit_mode != EDIT_NONE) {
    if (!s_blink_timer) {
      s_blink_timer = app_timer_register(400, blink_timer_callback, NULL);
    }
  } else {
    if (s_blink_timer) {
      app_timer_cancel(s_blink_timer);
      s_blink_timer = NULL;
    }
    s_blink_state = true;
  }
}

static void timer_tick_callback(void *data) {
  s_tick_timer = NULL;
  if (s_is_running) {
    update_remaining_from_clock();
    if (s_remaining_sec <= 3 && s_remaining_sec > 0) {
      pulsar_sound_countdown_tick(s_remaining_sec, s_audio_enabled, s_vibe_enabled);
    }
    
    if (s_remaining_sec <= 0) {
      trigger_alarm_firing();
      return;
    }
    
    layer_mark_dirty(s_canvas_layer);
    s_tick_timer = app_timer_register(1000, timer_tick_callback, NULL);
  }
}

static void alarm_pulse_callback(void *data) {
  s_alarm_pulse_timer = NULL;
  if (s_is_alarm_firing) {
    s_alarm_flash_count++;
    pulsar_sound_alarm_pulse(s_audio_enabled, s_vibe_enabled);
    layer_mark_dirty(s_canvas_layer);
    // Limit alarm ringing to 60 pulses (30 seconds)
    if (s_alarm_flash_count < 60) {
      s_alarm_pulse_timer = app_timer_register(500, alarm_pulse_callback, NULL);
    } else {
      light_enable(false);
    }
  }
}

static void trigger_alarm_firing(void) {
  s_is_running = false;
  s_is_paused = false;
  s_is_alarm_firing = true;
  s_remaining_sec = 0;
  s_alarm_flash_count = 0;
  light_enable(true);
  
  if (s_tick_timer) {
    app_timer_cancel(s_tick_timer);
    s_tick_timer = NULL;
  }
  
  if (s_wakeup_id >= 0) {
    wakeup_cancel(s_wakeup_id);
    s_wakeup_id = -1;
  }

  alarm_pulse_callback(NULL);
}

static void stop_alarm_firing(void) {
  s_is_alarm_firing = false;
  light_enable(false);
  if (s_alarm_pulse_timer) {
    app_timer_cancel(s_alarm_pulse_timer);
    s_alarm_pulse_timer = NULL;
  }
  s_remaining_sec = s_total_duration_sec;
  layer_mark_dirty(s_canvas_layer);
}

static void start_timer(void) {
  if (s_is_running) return;
  if (s_remaining_sec <= 0) {
    s_remaining_sec = s_total_duration_sec;
  }
  
  time_t now = time(NULL);
  s_target_epoch = now + s_remaining_sec;
  s_is_running = true;
  s_is_paused = false;
  s_is_alarm_firing = false;
  light_enable_interaction();

  // Schedule background wakeup
  if (s_wakeup_id >= 0) {
    wakeup_cancel(s_wakeup_id);
  }
  s_wakeup_id = wakeup_schedule(s_target_epoch, WAKEUP_COOKIE_TIMER, true);

  pulsar_sound_start(s_audio_enabled, s_vibe_enabled);

  if (s_tick_timer) {
    app_timer_cancel(s_tick_timer);
  }
  s_tick_timer = app_timer_register(1000, timer_tick_callback, NULL);
  layer_mark_dirty(s_canvas_layer);
}

static void pause_timer(void) {
  if (!s_is_running) return;
  update_remaining_from_clock();
  s_is_running = false;
  s_is_paused = true;
  light_enable(false);

  if (s_wakeup_id >= 0) {
    wakeup_cancel(s_wakeup_id);
    s_wakeup_id = -1;
  }

  if (s_tick_timer) {
    app_timer_cancel(s_tick_timer);
    s_tick_timer = NULL;
  }

  pulsar_sound_stop(s_audio_enabled, s_vibe_enabled);
  layer_mark_dirty(s_canvas_layer);
}

static void reset_timer(void) {
  s_is_running = false;
  s_is_paused = false;
  s_is_alarm_firing = false;
  light_enable(false);

  if (s_wakeup_id >= 0) {
    wakeup_cancel(s_wakeup_id);
    s_wakeup_id = -1;
  }
  if (s_tick_timer) {
    app_timer_cancel(s_tick_timer);
    s_tick_timer = NULL;
  }
  if (s_alarm_pulse_timer) {
    app_timer_cancel(s_alarm_pulse_timer);
    s_alarm_pulse_timer = NULL;
  }

  s_total_duration_sec = get_preset_duration(s_preset_index);
  s_remaining_sec = s_total_duration_sec;
  
  pulsar_sound_reset(s_audio_enabled, s_vibe_enabled);
  layer_mark_dirty(s_canvas_layer);
}

static void enter_edit_mode(void) {
  s_edit_mode = EDIT_MINUTES;
  int current_sec = (s_preset_index == NUM_PRESETS - 1) ? s_custom_sec : s_total_duration_sec;
  if (current_sec <= 0) current_sec = 30;
  s_edit_minutes = current_sec / 60;
  s_edit_seconds = current_sec % 60;
  s_blink_state = true;
  pulsar_sound_lap(s_audio_enabled, s_vibe_enabled);
  update_blink_loop();
  layer_mark_dirty(s_canvas_layer);
}

static void finish_edit_mode(void) {
  int total = (s_edit_minutes * 60) + s_edit_seconds;
  if (total <= 0) total = 10; // Minimum 10 seconds
  s_custom_sec = total;
  s_preset_index = NUM_PRESETS - 1; // Select CUSTOM PRESET
  s_total_duration_sec = total;
  s_remaining_sec = total;
  s_edit_mode = EDIT_NONE;
  update_blink_loop();
  
  persist_write_int(STORAGE_KEY_CUSTOM_SEC, s_custom_sec);
  persist_write_int(STORAGE_KEY_PRESET_IDX, s_preset_index);
  persist_write_int(STORAGE_KEY_TOTAL_DURATION, s_total_duration_sec);
  persist_write_int(STORAGE_KEY_REMAINING_SEC, s_remaining_sec);
  
  pulsar_sound_lap(s_audio_enabled, s_vibe_enabled);
  layer_mark_dirty(s_canvas_layer);
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_alarm_firing) {
    stop_alarm_firing();
    return;
  }
  if (s_is_running) return;

  if (s_edit_mode == EDIT_NONE) {
    enter_edit_mode();
  } else {
    finish_edit_mode();
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_alarm_firing) {
    stop_alarm_firing();
    return;
  }

  if (s_edit_mode == EDIT_MINUTES) {
    s_edit_mode = EDIT_SECONDS;
    s_blink_state = true;
    pulsar_sound_start(s_audio_enabled, s_vibe_enabled);
    layer_mark_dirty(s_canvas_layer);
    return;
  } else if (s_edit_mode == EDIT_SECONDS) {
    finish_edit_mode();
    return;
  }

  if (s_is_running) {
    pause_timer();
  } else {
    start_timer();
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_alarm_firing) {
    stop_alarm_firing();
    return;
  }

  if (s_edit_mode == EDIT_MINUTES) {
    if (s_edit_minutes < 99) s_edit_minutes++;
    layer_mark_dirty(s_canvas_layer);
    return;
  } else if (s_edit_mode == EDIT_SECONDS) {
    if (s_edit_seconds < 59) s_edit_seconds++;
    layer_mark_dirty(s_canvas_layer);
    return;
  }

  if (s_is_running) {
    // Quick Add +15s (if < 60s) or +60s
    int add_sec = (s_remaining_sec < 60) ? 15 : 60;
    s_remaining_sec += add_sec;
    s_total_duration_sec += add_sec;
    s_target_epoch += add_sec;
    if (s_wakeup_id >= 0) {
      wakeup_cancel(s_wakeup_id);
      s_wakeup_id = wakeup_schedule(s_target_epoch, WAKEUP_COOKIE_TIMER, true);
    }
    pulsar_sound_lap(s_audio_enabled, s_vibe_enabled);
    layer_mark_dirty(s_canvas_layer);
  } else if (!s_is_paused) {
    // AIDEV-NOTE: PRESETS[] is ordered by ascending duration, so UP must step FORWARD through it to reach
    // a longer timer. These were inverted: pressing DOWN on a 1:00 preset moved to 2:00.
    s_preset_index++;
    if (s_preset_index >= NUM_PRESETS) s_preset_index = 0;
    s_total_duration_sec = get_preset_duration(s_preset_index);
    s_remaining_sec = s_total_duration_sec;
    pulsar_sound_start(s_audio_enabled, s_vibe_enabled);
    layer_mark_dirty(s_canvas_layer);
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_alarm_firing) {
    stop_alarm_firing();
    return;
  }

  // AIDEV-NOTE: Clamp at zero rather than wrap. A duration is a magnitude, not a cyclic quantity, so
  // wrapping 0 -> 99 made DOWN read as a large increment -- the timer opens at 0 minutes, so that was the
  // first thing a press did. (The alarm still wraps, correctly: a time of day IS cyclic.)
  if (s_edit_mode == EDIT_MINUTES) {
    if (s_edit_minutes > 0) s_edit_minutes--;
    layer_mark_dirty(s_canvas_layer);
    return;
  } else if (s_edit_mode == EDIT_SECONDS) {
    if (s_edit_seconds > 0) s_edit_seconds--;
    layer_mark_dirty(s_canvas_layer);
    return;
  }

  if (s_is_running) {
    // Quick Subtract -15s or -60s
    int sub_sec = (s_remaining_sec <= 60) ? 15 : 60;
    if (s_remaining_sec > sub_sec) {
      s_remaining_sec -= sub_sec;
      s_target_epoch -= sub_sec;
      if (s_wakeup_id >= 0) {
        wakeup_cancel(s_wakeup_id);
        s_wakeup_id = wakeup_schedule(s_target_epoch, WAKEUP_COOKIE_TIMER, true);
      }
      pulsar_sound_stop(s_audio_enabled, s_vibe_enabled);
      layer_mark_dirty(s_canvas_layer);
    }
  } else if (s_is_paused) {
    reset_timer();
  } else {
    s_preset_index--;
    if (s_preset_index < 0) s_preset_index = NUM_PRESETS - 1;
    s_total_duration_sec = get_preset_duration(s_preset_index);
    s_remaining_sec = s_total_duration_sec;
    pulsar_sound_start(s_audio_enabled, s_vibe_enabled);
    layer_mark_dirty(s_canvas_layer);
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 500, select_long_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void wakeup_handler(WakeupId id, int32_t reason) {
  trigger_alarm_firing();
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  Colorway active_palette = pulsar_get_palette(s_colorway % NUM_COLORWAYS);
  const Colorway *palette = &active_palette;

  // 1. Background Fill: Maintain dark background so red LED dots stay 100% visible
  graphics_context_set_fill_color(ctx, palette->outer_bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  bool is_flash_frame = s_is_alarm_firing && (s_alarm_flash_count % 2 == 1);

  // 2. Alert Beacon Frame & Banner fills when alarm firing
  if (is_flash_frame) {
    pulsar_draw_alert_banners(ctx, bounds, palette);
  }

  // Determine high-contrast text color for header and footer banners
  Colorway banner_palette = *palette;
  if (is_flash_frame) {
    if (palette->outer_bg.argb == GColorWhite.argb) {
      banner_palette.text_outer = GColorWhite;
    } else {
      banner_palette.text_outer = (palette->lit.argb == GColorWhite.argb || palette->lit.argb == GColorYellow.argb) ? GColorBlack : GColorWhite;
    }
  }

  // 3. Header
  const char *header_text = s_is_alarm_firing ? "* T I M E ' S  U P *" : "T I M E R";
  if (s_edit_mode == EDIT_MINUTES) {
    header_text = "S E T   M I N U T E S";
  } else if (s_edit_mode == EDIT_SECONDS) {
    header_text = "S E T   S E C O N D S";
  }
  pulsar_draw_header(ctx, bounds, header_text, &banner_palette);

  // 4. Time calculation & digits
  int d1, d2, d3, d4;
  if (s_edit_mode != EDIT_NONE) {
    d1 = s_edit_minutes / 10;
    d2 = s_edit_minutes % 10;
    d3 = s_edit_seconds / 10;
    d4 = s_edit_seconds % 10;

    if (s_edit_mode == EDIT_MINUTES && !s_blink_state) {
      d1 = GLYPH_BLANK;
      d2 = GLYPH_BLANK;
    } else if (s_edit_mode == EDIT_SECONDS && !s_blink_state) {
      d3 = GLYPH_BLANK;
      d4 = GLYPH_BLANK;
    }
  } else {
    int total_sec = s_remaining_sec;
    int mins = total_sec / 60;
    int secs = total_sec % 60;
    d1 = mins / 10;
    d2 = mins % 10;
    d3 = secs / 10;
    d4 = secs % 10;
  }

  // 5. Main 4-Digit Display
  bool colon_lit = (s_edit_mode != EDIT_NONE) ? true : (s_is_running ? (s_remaining_sec % 2 == 0) : true);
  pulsar_draw_4digits(ctx, bounds, d1, d2, d3, d4, true, colon_lit, palette, true, s_italic_slant);

  // 6. 10-Dot Micro-LED Progress Bar
  if (s_edit_mode != EDIT_NONE) {
    bool beads[NUM_MICRO_BEADS];
    for (int i = 0; i < NUM_MICRO_BEADS; i++) beads[i] = (i % 2 == 0);
    pulsar_draw_micro_beads(ctx, bounds, palette, true, beads);
  } else {
    pulsar_draw_progress_beads(ctx, bounds, palette, true, s_remaining_sec, s_total_duration_sec);
  }

  // 7. Vintage Space-Age Footer
  static char footer_buffer[32];
  if (s_is_alarm_firing) {
    snprintf(footer_buffer, sizeof(footer_buffer), "★  A L E R T  ★");
  } else if (s_edit_mode == EDIT_MINUTES) {
    snprintf(footer_buffer, sizeof(footer_buffer), "S E L E C T :  S E C");
  } else if (s_edit_mode == EDIT_SECONDS) {
    snprintf(footer_buffer, sizeof(footer_buffer), "S E L E C T :  S A V E");
  } else if (s_is_running) {
    snprintf(footer_buffer, sizeof(footer_buffer), "C O U N T D O W N");
  } else if (s_is_paused) {
    snprintf(footer_buffer, sizeof(footer_buffer), "P A U S E D");
  } else {
    if (s_preset_index == NUM_PRESETS - 1) {
      int c_m = s_custom_sec / 60;
      int c_s = s_custom_sec % 60;
      snprintf(footer_buffer, sizeof(footer_buffer), "C U S T O M  %02d:%02d", c_m, c_s);
    } else {
      snprintf(footer_buffer, sizeof(footer_buffer), "%s", PRESETS[s_preset_index].name);
    }
  }
  pulsar_draw_footer(ctx, bounds, footer_buffer, &banner_palette);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  for (Tuple *t = dict_read_first(iterator); t != NULL; t = dict_read_next(iterator)) {
    uint32_t key = t->key;
    if (key == MESSAGE_KEY_AppKeyColorway) {
      s_colorway = pulsar_tuple_to_int(t, s_colorway);
      if (s_colorway < 0 || s_colorway >= NUM_COLORWAYS) s_colorway = 0;
      persist_write_int(STORAGE_KEY_COLORWAY, s_colorway);
      pulsar_apply_backlight_tint(s_colorway % NUM_COLORWAYS, s_backlight_tint);
    } else if (key == MESSAGE_KEY_AppKeyShowGhost) {
      s_show_ghost = pulsar_tuple_to_bool(t, s_show_ghost);
      persist_write_bool(STORAGE_KEY_SHOW_GHOST, s_show_ghost);
      pulsar_set_ghost_enabled(s_show_ghost);
    } else if (key == MESSAGE_KEY_AppKeyBacklightTint) {
      s_backlight_tint = pulsar_tuple_to_bool(t, s_backlight_tint);
      persist_write_bool(STORAGE_KEY_BACKLIGHT_TINT, s_backlight_tint);
      pulsar_apply_backlight_tint(s_colorway % NUM_COLORWAYS, s_backlight_tint);
    } else if (key == MESSAGE_KEY_AppKeyLedGlow) {
      s_led_glow = pulsar_tuple_to_bool(t, s_led_glow);
      persist_write_bool(STORAGE_KEY_LED_GLOW, s_led_glow);
      pulsar_set_glow_enabled(s_led_glow);
    } else if (key == MESSAGE_KEY_AppKeyItalicSlant) {
      s_italic_slant = pulsar_tuple_to_bool(t, s_italic_slant);
      persist_write_bool(STORAGE_KEY_ITALIC_SLANT, s_italic_slant);
    } else if (key == MESSAGE_KEY_AppKeyAudioEnabled) {
      s_audio_enabled = pulsar_tuple_to_bool(t, s_audio_enabled);
      persist_write_bool(STORAGE_KEY_AUDIO_ENABLED, s_audio_enabled);
    } else if (key == MESSAGE_KEY_AppKeyVibeEnabled) {
      s_vibe_enabled = pulsar_tuple_to_bool(t, s_vibe_enabled);
      persist_write_bool(STORAGE_KEY_VIBE_ENABLED, s_vibe_enabled);
    }
  }
  layer_mark_dirty(s_canvas_layer);
}

static void load_state(void) {
  if (persist_exists(STORAGE_KEY_BACKLIGHT_TINT)) {
    s_backlight_tint = persist_read_bool(STORAGE_KEY_BACKLIGHT_TINT);
  }
  if (persist_exists(STORAGE_KEY_SHOW_GHOST)) {
    s_show_ghost = persist_read_bool(STORAGE_KEY_SHOW_GHOST);
  }
  pulsar_set_ghost_enabled(s_show_ghost);
  if (persist_exists(STORAGE_KEY_LED_GLOW)) {
    s_led_glow = persist_read_bool(STORAGE_KEY_LED_GLOW);
  }
  pulsar_set_glow_enabled(s_led_glow);
  if (persist_exists(STORAGE_KEY_COLORWAY)) {
    s_colorway = persist_read_int(STORAGE_KEY_COLORWAY);
    if (s_colorway < 0 || s_colorway >= NUM_COLORWAYS) s_colorway = 0;
  }
  if (persist_exists(STORAGE_KEY_ITALIC_SLANT)) {
    s_italic_slant = persist_read_bool(STORAGE_KEY_ITALIC_SLANT);
  }
  if (persist_exists(STORAGE_KEY_AUDIO_ENABLED)) {
    s_audio_enabled = persist_read_bool(STORAGE_KEY_AUDIO_ENABLED);
  }
  if (persist_exists(STORAGE_KEY_VIBE_ENABLED)) {
    s_vibe_enabled = persist_read_bool(STORAGE_KEY_VIBE_ENABLED);
  }
  if (persist_exists(STORAGE_KEY_CUSTOM_SEC)) {
    s_custom_sec = persist_read_int(STORAGE_KEY_CUSTOM_SEC);
    if (s_custom_sec <= 0) s_custom_sec = 30;
  }
  if (persist_exists(STORAGE_KEY_PRESET_IDX)) {
    s_preset_index = persist_read_int(STORAGE_KEY_PRESET_IDX);
    if (s_preset_index < 0 || s_preset_index >= NUM_PRESETS) s_preset_index = 6;
  }
  if (persist_exists(STORAGE_KEY_TOTAL_DURATION)) {
    s_total_duration_sec = persist_read_int(STORAGE_KEY_TOTAL_DURATION);
  } else {
    s_total_duration_sec = get_preset_duration(s_preset_index);
  }
  if (persist_exists(STORAGE_KEY_REMAINING_SEC)) {
    s_remaining_sec = persist_read_int(STORAGE_KEY_REMAINING_SEC);
  } else {
    s_remaining_sec = s_total_duration_sec;
  }

  if (persist_exists(STORAGE_KEY_IS_RUNNING)) {
    bool was_running = persist_read_bool(STORAGE_KEY_IS_RUNNING);
    if (was_running && persist_exists(STORAGE_KEY_TARGET_EPOCH)) {
      s_target_epoch = persist_read_int(STORAGE_KEY_TARGET_EPOCH);
      time_t now = time(NULL);
      if (now >= s_target_epoch) {
        trigger_alarm_firing();
      } else {
        s_remaining_sec = (int)(s_target_epoch - now);
        s_is_running = true;
      }
    }
  }
  pulsar_apply_backlight_tint(s_colorway % NUM_COLORWAYS, s_backlight_tint);
}

static void save_state(void) {
  persist_write_int(STORAGE_KEY_CUSTOM_SEC, s_custom_sec);
  persist_write_int(STORAGE_KEY_PRESET_IDX, s_preset_index);
  persist_write_int(STORAGE_KEY_TOTAL_DURATION, s_total_duration_sec);
  persist_write_int(STORAGE_KEY_REMAINING_SEC, s_remaining_sec);
  persist_write_bool(STORAGE_KEY_IS_RUNNING, s_is_running);
  if (s_is_running) {
    persist_write_int(STORAGE_KEY_TARGET_EPOCH, s_target_epoch);
  }
}

// AIDEV-NOTE: light_set_color() is documented to reset when a notification preempts the app, but on
// FW 4.36.2 the tint survives into the notification and system UI. Drop to the wearer's system colour
// whenever we lose focus and re-apply on regain, so our colour never escapes our own screen.
static void focus_handler(bool in_focus) {
  if (!s_backlight_tint) return;
  pulsar_apply_backlight_tint(s_colorway % NUM_COLORWAYS, in_focus);
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
  load_state();
  
  wakeup_service_subscribe(wakeup_handler);

  // Check if launched by wakeup event
  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    WakeupId id = 0;
    int32_t reason = 0;
    wakeup_get_launch_event(&id, &reason);
    trigger_alarm_firing();
  }

  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);

  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(256, 64);

  app_focus_service_subscribe_handlers((AppFocusHandlers){ .did_focus = focus_handler });

  if (s_is_running) {
    s_tick_timer = app_timer_register(1000, timer_tick_callback, NULL);
  }
}

static void deinit(void) {
  pulsar_apply_backlight_tint(s_colorway % NUM_COLORWAYS, false);
  app_focus_service_unsubscribe();
  light_enable(false);
  save_state();
  if (s_tick_timer) {
    app_timer_cancel(s_tick_timer);
    s_tick_timer = NULL;
  }
  if (s_alarm_pulse_timer) {
    app_timer_cancel(s_alarm_pulse_timer);
    s_alarm_pulse_timer = NULL;
  }
  if (s_blink_timer) {
    app_timer_cancel(s_blink_timer);
    s_blink_timer = NULL;
  }
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
