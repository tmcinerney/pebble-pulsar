#include <pebble.h>
#include "pulsar_palette.h"
#include "pulsar_layout.h"
#include "pulsar_matrix.h"
#include "pulsar_micro_bar.h"
#include "pulsar_brand.h"
#include "pulsar_audio.h"

#define MAX_LAPS 20
#define REFRESH_RATE_MS 50

#define STORAGE_KEY_COLORWAY       10001
#define STORAGE_KEY_ITALIC_SLANT   10005
#define STORAGE_KEY_AUDIO_ENABLED  10020
#define STORAGE_KEY_VIBE_ENABLED   10021
#define STORAGE_KEY_RUNNING_STATE  10030
#define STORAGE_KEY_SAVED_ELAPSED  10031
#define STORAGE_KEY_SAVED_START    10032

#ifndef MESSAGE_KEY_AppKeyColorway
#define MESSAGE_KEY_AppKeyColorway       10001
#define MESSAGE_KEY_AppKeyItalicSlant   10005
#define MESSAGE_KEY_AppKeyAudioEnabled  10020
#define MESSAGE_KEY_AppKeyVibeEnabled   10021
#endif

static Window *s_main_window;
static Layer *s_canvas_layer;
static AppTimer *s_refresh_timer = NULL;
static AppTimer *s_lap_flash_timer = NULL;

static int s_colorway = COLORWAY_VIBRANT_RUBY;
static bool s_italic_slant = true;
static bool s_audio_enabled = true;
static bool s_vibe_enabled = true;

static bool s_is_running = false;
static time_t s_start_sec = 0;
static uint16_t s_start_ms = 0;
static uint32_t s_accumulated_ms = 0;

static uint32_t s_laps[MAX_LAPS];
static int s_lap_count = 0;
static int s_browse_lap_idx = -1; // -1 means live view
static int s_flash_lap_num = 0;

static uint32_t get_current_elapsed_ms(void) {
  if (!s_is_running) {
    return s_accumulated_ms;
  }
  time_t now_sec;
  uint16_t now_ms;
  time_ms(&now_sec, &now_ms);

  int64_t diff_sec = (int64_t)now_sec - (int64_t)s_start_sec;
  int64_t diff_ms = (int64_t)now_ms - (int64_t)s_start_ms;
  int64_t total_diff = (diff_sec * 1000) + diff_ms;
  if (total_diff < 0) total_diff = 0;
  return s_accumulated_ms + (uint32_t)total_diff;
}

static void refresh_timer_callback(void *data) {
  s_refresh_timer = NULL;
  if (s_is_running) {
    layer_mark_dirty(s_canvas_layer);
    s_refresh_timer = app_timer_register(REFRESH_RATE_MS, refresh_timer_callback, NULL);
  }
}

static void lap_flash_callback(void *data) {
  s_lap_flash_timer = NULL;
  s_flash_lap_num = 0;
  layer_mark_dirty(s_canvas_layer);
}

static void start_stopwatch(void) {
  if (s_is_running) return;
  time_ms(&s_start_sec, &s_start_ms);
  s_is_running = true;
  s_browse_lap_idx = -1;
  if (s_audio_enabled || s_vibe_enabled) {
    pulsar_sound_start();
  }
  if (!s_refresh_timer) {
    s_refresh_timer = app_timer_register(REFRESH_RATE_MS, refresh_timer_callback, NULL);
  }
  layer_mark_dirty(s_canvas_layer);
}

static void stop_stopwatch(void) {
  if (!s_is_running) return;
  s_accumulated_ms = get_current_elapsed_ms();
  s_is_running = false;
  if (s_refresh_timer) {
    app_timer_cancel(s_refresh_timer);
    s_refresh_timer = NULL;
  }
  if (s_audio_enabled || s_vibe_enabled) {
    pulsar_sound_stop();
  }
  layer_mark_dirty(s_canvas_layer);
}

static void record_lap(void) {
  uint32_t current_ms = get_current_elapsed_ms();
  if (s_lap_count < MAX_LAPS) {
    s_laps[s_lap_count] = current_ms;
    s_lap_count++;
    s_flash_lap_num = s_lap_count;
    if (s_audio_enabled || s_vibe_enabled) {
      pulsar_sound_lap();
    }
    if (s_lap_flash_timer) {
      app_timer_cancel(s_lap_flash_timer);
    }
    s_lap_flash_timer = app_timer_register(2500, lap_flash_callback, NULL);
    layer_mark_dirty(s_canvas_layer);
  }
}

static void reset_stopwatch(void) {
  if (s_is_running) return;
  if (s_accumulated_ms == 0 && s_lap_count > 0) {
    // Second press clears lap list
    s_lap_count = 0;
    s_browse_lap_idx = -1;
  }
  s_accumulated_ms = 0;
  s_browse_lap_idx = -1;
  s_flash_lap_num = 0;
  if (s_audio_enabled || s_vibe_enabled) {
    pulsar_sound_reset();
  }
  layer_mark_dirty(s_canvas_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_running) {
    stop_stopwatch();
  } else {
    start_stopwatch();
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_running) {
    record_lap();
  } else {
    // Browse recorded laps
    if (s_lap_count > 0) {
      s_browse_lap_idx++;
      if (s_browse_lap_idx >= s_lap_count) {
        s_browse_lap_idx = -1; // back to live reset screen
      }
      layer_mark_dirty(s_canvas_layer);
    }
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_running) {
    // In running mode, down cycles lap info
  } else {
    reset_stopwatch();
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  Colorway active_palette = pulsar_get_palette(s_colorway % NUM_COLORWAYS);
  const Colorway *palette = &active_palette;

  // 1. Background Fill
  graphics_context_set_fill_color(ctx, palette->outer_bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // 2. Header
  pulsar_draw_header(ctx, bounds, "C H R O N O", palette);

  // 3. Time calculation
  uint32_t elapsed_ms = get_current_elapsed_ms();
  if (s_browse_lap_idx >= 0 && s_browse_lap_idx < s_lap_count) {
    elapsed_ms = s_laps[s_browse_lap_idx];
  }

  uint32_t total_sec = elapsed_ms / 1000;
  uint32_t mins = (total_sec / 60) % 100;
  uint32_t secs = total_sec % 60;
  uint32_t centis = (elapsed_ms % 1000) / 10;

  int d1 = mins / 10;
  int d2 = mins % 10;
  int d3 = secs / 10;
  int d4 = secs % 10;

  // 4. Main MM:SS Digits
  bool colon_lit = s_is_running ? ((elapsed_ms % 1000) < 500) : true;
  pulsar_draw_4digits(ctx, bounds, d1, d2, d3, d4, true, colon_lit, palette, true, s_italic_slant);

  // 5. Centiseconds Sub-Display (Right aligned / beneath)
  int sub_c1 = centis / 10;
  int sub_c2 = centis % 10;
  int sub_spacing_x = bounds.size.w > 180 ? 4 : 3;
  int sub_spacing_y = bounds.size.w > 180 ? 5 : 4;
  int sub_dot_radius = 1;
  int sub_gap = bounds.size.w > 180 ? 8 : 6;
  int sub_span = (DIGIT_WIDTH - 1) * sub_spacing_x;
  int sub_y = bounds.size.w > 180 ? 122 : 88;
  int sub_total_w = (sub_span * 2) + sub_gap + 8;
  int sub_start_x = (bounds.size.w - sub_total_w) / 2;

  // Decimal dot
  graphics_context_set_fill_color(ctx, palette->lit);
  graphics_fill_circle(ctx, GPoint(sub_start_x, sub_y + (DIGIT_HEIGHT * sub_spacing_y) - 2), sub_dot_radius);

  // Sub digits
  int c1_x = sub_start_x + 8;
  int c2_x = c1_x + sub_span + sub_gap;
  pulsar_draw_digit_custom(ctx, c1_x, sub_y, sub_c1, palette, true, bounds.size.w, 
                           sub_spacing_x, sub_spacing_y, sub_dot_radius, s_italic_slant);
  pulsar_draw_digit_custom(ctx, c2_x, sub_y, sub_c2, palette, true, bounds.size.w, 
                           sub_spacing_x, sub_spacing_y, sub_dot_radius, s_italic_slant);

  // 6. Tachymeter Micro-LED Chaser
  pulsar_draw_tachymeter_beads(ctx, bounds, palette, true, elapsed_ms, s_is_running);

  // 7. Footer Status
  static char footer_buffer[32];
  if (s_flash_lap_num > 0) {
    snprintf(footer_buffer, sizeof(footer_buffer), "L A P   %02d   S E T", s_flash_lap_num);
  } else if (s_browse_lap_idx >= 0) {
    snprintf(footer_buffer, sizeof(footer_buffer), "L A P   %02d / %02d", s_browse_lap_idx + 1, s_lap_count);
  } else if (s_is_running) {
    snprintf(footer_buffer, sizeof(footer_buffer), "L A P   %02d", s_lap_count + 1);
  } else if (elapsed_ms == 0) {
    snprintf(footer_buffer, sizeof(footer_buffer), "R E A D Y");
  } else {
    snprintf(footer_buffer, sizeof(footer_buffer), "P A U S E D");
  }
  pulsar_draw_footer(ctx, bounds, footer_buffer, palette);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  for (Tuple *t = dict_read_first(iterator); t != NULL; t = dict_read_next(iterator)) {
    uint32_t key = t->key;
    if (key == MESSAGE_KEY_AppKeyColorway) {
      s_colorway = (int)t->value->int32;
      persist_write_int(STORAGE_KEY_COLORWAY, s_colorway);
    } else if (key == MESSAGE_KEY_AppKeyItalicSlant) {
      s_italic_slant = (t->value->int32 != 0);
      persist_write_bool(STORAGE_KEY_ITALIC_SLANT, s_italic_slant);
    } else if (key == MESSAGE_KEY_AppKeyAudioEnabled) {
      s_audio_enabled = (t->value->int32 != 0);
      persist_write_bool(STORAGE_KEY_AUDIO_ENABLED, s_audio_enabled);
    } else if (key == MESSAGE_KEY_AppKeyVibeEnabled) {
      s_vibe_enabled = (t->value->int32 != 0);
      persist_write_bool(STORAGE_KEY_VIBE_ENABLED, s_vibe_enabled);
    }
  }
  layer_mark_dirty(s_canvas_layer);
}

static void load_state(void) {
  if (persist_exists(STORAGE_KEY_COLORWAY)) {
    s_colorway = persist_read_int(STORAGE_KEY_COLORWAY);
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

  if (persist_exists(STORAGE_KEY_RUNNING_STATE)) {
    bool was_running = persist_read_bool(STORAGE_KEY_RUNNING_STATE);
    s_accumulated_ms = persist_read_int(STORAGE_KEY_SAVED_ELAPSED);
    if (was_running && persist_exists(STORAGE_KEY_SAVED_START)) {
      time_t saved_start = persist_read_int(STORAGE_KEY_SAVED_START);
      time_t now = time(NULL);
      if (now >= saved_start) {
        s_accumulated_ms += (uint32_t)(now - saved_start) * 1000;
      }
      s_is_running = true;
      time_ms(&s_start_sec, &s_start_ms);
    }
  }
}

static void save_state(void) {
  persist_write_bool(STORAGE_KEY_RUNNING_STATE, s_is_running);
  persist_write_int(STORAGE_KEY_SAVED_ELAPSED, get_current_elapsed_ms());
  if (s_is_running) {
    persist_write_int(STORAGE_KEY_SAVED_START, time(NULL));
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
    s_refresh_timer = app_timer_register(REFRESH_RATE_MS, refresh_timer_callback, NULL);
  }
}

static void deinit(void) {
  save_state();
  if (s_refresh_timer) {
    app_timer_cancel(s_refresh_timer);
    s_refresh_timer = NULL;
  }
  if (s_lap_flash_timer) {
    app_timer_cancel(s_lap_flash_timer);
    s_lap_flash_timer = NULL;
  }
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
