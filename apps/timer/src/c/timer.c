#include <pebble.h>
#include "pulsar_palette.h"
#include "pulsar_layout.h"
#include "pulsar_matrix.h"
#include "pulsar_micro_bar.h"
#include "pulsar_brand.h"
#include "pulsar_audio.h"

#define WAKEUP_COOKIE_TIMER 1970
#define NUM_PRESETS 10

typedef struct {
  const char *name;
  int seconds;
} TimerPreset;

static const TimerPreset PRESETS[NUM_PRESETS] = {
  { "0 1 : 0 0   P R E S E T", 60 },
  { "0 3 : 0 0   P R E S E T", 180 },
  { "0 5 : 0 0   P R E S E T", 300 },
  { "1 0 : 0 0   P R E S E T", 600 },
  { "1 5 : 0 0   P R E S E T", 900 },
  { "2 0 : 0 0   P R E S E T", 1200 },
  { "P O M O D O R O   2 5 M", 1500 },
  { "3 0 : 0 0   P R E S E T", 1800 },
  { "4 5 : 0 0   P R E S E T", 2700 },
  { "6 0 : 0 0   P R E S E T", 3600 }
};

#define STORAGE_KEY_COLORWAY       10001
#define STORAGE_KEY_ITALIC_SLANT   10005
#define STORAGE_KEY_AUDIO_ENABLED  10020
#define STORAGE_KEY_VIBE_ENABLED   10021
#define STORAGE_KEY_PRESET_IDX     10040
#define STORAGE_KEY_TARGET_EPOCH   10041
#define STORAGE_KEY_TOTAL_DURATION 10042
#define STORAGE_KEY_REMAINING_SEC  10043
#define STORAGE_KEY_IS_RUNNING     10044
#define STORAGE_KEY_WAKEUP_ID      10045

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

static int s_colorway = COLORWAY_VIBRANT_RUBY;
static bool s_italic_slant = true;
static bool s_audio_enabled = true;
static bool s_vibe_enabled = true;

static int s_preset_index = 2; // Default 5 min
static int s_total_duration_sec = 300;
static int s_remaining_sec = 300;
static bool s_is_running = false;
static bool s_is_paused = false;
static bool s_is_alarm_firing = false;
static time_t s_target_epoch = 0;
static WakeupId s_wakeup_id = -1;
static int s_alarm_flash_count = 0;

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

static void timer_tick_callback(void *data) {
  s_tick_timer = NULL;
  if (s_is_running) {
    update_remaining_from_clock();
    if (s_remaining_sec <= 3 && s_remaining_sec > 0) {
      if (s_audio_enabled || s_vibe_enabled) {
        pulsar_sound_countdown_tick(s_remaining_sec);
      }
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
    if (s_audio_enabled || s_vibe_enabled) {
      pulsar_sound_alarm_pulse();
    }
    layer_mark_dirty(s_canvas_layer);
    // Limit alarm ringing to 60 pulses (30 seconds)
    if (s_alarm_flash_count < 60) {
      s_alarm_pulse_timer = app_timer_register(500, alarm_pulse_callback, NULL);
    }
  }
}

static void trigger_alarm_firing(void) {
  s_is_running = false;
  s_is_paused = false;
  s_is_alarm_firing = true;
  s_remaining_sec = 0;
  s_alarm_flash_count = 0;
  
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
  light_enable(true);

  // Schedule background wakeup
  if (s_wakeup_id >= 0) {
    wakeup_cancel(s_wakeup_id);
  }
  s_wakeup_id = wakeup_schedule(s_target_epoch, WAKEUP_COOKIE_TIMER, true);

  if (s_audio_enabled || s_vibe_enabled) {
    pulsar_sound_start();
  }

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

  if (s_audio_enabled || s_vibe_enabled) {
    pulsar_sound_stop();
  }
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

  s_total_duration_sec = PRESETS[s_preset_index].seconds;
  s_remaining_sec = s_total_duration_sec;
  
  if (s_audio_enabled || s_vibe_enabled) {
    pulsar_sound_reset();
  }
  layer_mark_dirty(s_canvas_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_alarm_firing) {
    stop_alarm_firing();
  } else if (s_is_running) {
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

  if (s_is_running) {
    // Quick Add +1 Minute
    s_remaining_sec += 60;
    s_total_duration_sec += 60;
    s_target_epoch += 60;
    if (s_wakeup_id >= 0) {
      wakeup_cancel(s_wakeup_id);
      s_wakeup_id = wakeup_schedule(s_target_epoch, WAKEUP_COOKIE_TIMER, true);
    }
    if (s_audio_enabled || s_vibe_enabled) {
      pulsar_sound_lap();
    }
    layer_mark_dirty(s_canvas_layer);
  } else if (!s_is_paused) {
    // Cycle preset up
    s_preset_index--;
    if (s_preset_index < 0) s_preset_index = NUM_PRESETS - 1;
    s_total_duration_sec = PRESETS[s_preset_index].seconds;
    s_remaining_sec = s_total_duration_sec;
    if (s_audio_enabled || s_vibe_enabled) {
      pulsar_sound_start();
    }
    layer_mark_dirty(s_canvas_layer);
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_alarm_firing) {
    stop_alarm_firing();
    return;
  }

  if (s_is_running) {
    // Quick Subtract -1 Minute (if > 60s)
    if (s_remaining_sec > 60) {
      s_remaining_sec -= 60;
      s_target_epoch -= 60;
      if (s_wakeup_id >= 0) {
        wakeup_cancel(s_wakeup_id);
        s_wakeup_id = wakeup_schedule(s_target_epoch, WAKEUP_COOKIE_TIMER, true);
      }
      if (s_audio_enabled || s_vibe_enabled) {
        pulsar_sound_stop();
      }
      layer_mark_dirty(s_canvas_layer);
    }
  } else if (s_is_paused) {
    reset_timer();
  } else {
    // Cycle preset down
    s_preset_index++;
    if (s_preset_index >= NUM_PRESETS) s_preset_index = 0;
    s_total_duration_sec = PRESETS[s_preset_index].seconds;
    s_remaining_sec = s_total_duration_sec;
    if (s_audio_enabled || s_vibe_enabled) {
      pulsar_sound_start();
    }
    layer_mark_dirty(s_canvas_layer);
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
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

  // 1. Background Fill (Flashing in red when alarm firing)
  if (s_is_alarm_firing && (s_alarm_flash_count % 2 == 1)) {
    graphics_context_set_fill_color(ctx, palette->lit);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  } else {
    graphics_context_set_fill_color(ctx, palette->outer_bg);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  }

  // 2. Header
  const char *header_text = s_is_alarm_firing ? "* T I M E ' S  U P *" : "T I M E R";
  pulsar_draw_header(ctx, bounds, header_text, palette);

  // 3. Time calculation
  int total_sec = s_remaining_sec;
  int mins = total_sec / 60;
  int secs = total_sec % 60;

  int d1 = mins / 10;
  int d2 = mins % 10;
  int d3 = secs / 10;
  int d4 = secs % 10;

  // 4. Main 4-Digit Display
  bool colon_lit = s_is_running ? (secs % 2 == 0) : true;
  pulsar_draw_4digits(ctx, bounds, d1, d2, d3, d4, true, colon_lit, palette, true, s_italic_slant);

  // 5. 10-Dot Micro-LED Progress Bar
  pulsar_draw_progress_beads(ctx, bounds, palette, true, s_remaining_sec, s_total_duration_sec);

  // 6. Vintage Space-Age Footer
  static char footer_buffer[32];
  if (s_is_alarm_firing) {
    snprintf(footer_buffer, sizeof(footer_buffer), "★  A L E R T  ★");
  } else if (s_is_running) {
    snprintf(footer_buffer, sizeof(footer_buffer), "C O U N T D O W N");
  } else if (s_is_paused) {
    snprintf(footer_buffer, sizeof(footer_buffer), "P A U S E D");
  } else {
    snprintf(footer_buffer, sizeof(footer_buffer), "%s", PRESETS[s_preset_index].name);
  }
  pulsar_draw_footer(ctx, bounds, footer_buffer, palette);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  for (Tuple *t = dict_read_first(iterator); t != NULL; t = dict_read_next(iterator)) {
    uint32_t key = t->key;
    if (key == MESSAGE_KEY_AppKeyColorway) {
      s_colorway = pulsar_tuple_to_int(t, s_colorway);
      if (s_colorway < 0 || s_colorway >= NUM_COLORWAYS) s_colorway = 0;
      persist_write_int(STORAGE_KEY_COLORWAY, s_colorway);
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
  if (persist_exists(STORAGE_KEY_PRESET_IDX)) {
    s_preset_index = persist_read_int(STORAGE_KEY_PRESET_IDX);
    if (s_preset_index < 0 || s_preset_index >= NUM_PRESETS) s_preset_index = 2;
  }
  if (persist_exists(STORAGE_KEY_TOTAL_DURATION)) {
    s_total_duration_sec = persist_read_int(STORAGE_KEY_TOTAL_DURATION);
  } else {
    s_total_duration_sec = PRESETS[s_preset_index].seconds;
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
}

static void save_state(void) {
  persist_write_int(STORAGE_KEY_PRESET_IDX, s_preset_index);
  persist_write_int(STORAGE_KEY_TOTAL_DURATION, s_total_duration_sec);
  persist_write_int(STORAGE_KEY_REMAINING_SEC, s_remaining_sec);
  persist_write_bool(STORAGE_KEY_IS_RUNNING, s_is_running);
  if (s_is_running) {
    persist_write_int(STORAGE_KEY_TARGET_EPOCH, s_target_epoch);
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

  if (s_is_running) {
    s_tick_timer = app_timer_register(1000, timer_tick_callback, NULL);
  }
}

static void deinit(void) {
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
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
