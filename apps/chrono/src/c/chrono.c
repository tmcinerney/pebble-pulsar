#include <pebble.h>
#include "pulsar_palette.h"
#include "pulsar_layout.h"
#include "pulsar_matrix.h"
#include "pulsar_micro_bar.h"
#include "pulsar_brand.h"
#include "pulsar_audio.h"

#define MAX_LAPS 20
#define REFRESH_RATE_MS 50
#define FREEZE_DURATION_MS 3000

#define STORAGE_KEY_COLORWAY            10001
#define STORAGE_KEY_ITALIC_SLANT        10005
#define STORAGE_KEY_AUDIO_ENABLED       10020
#define STORAGE_KEY_VIBE_ENABLED        10021
#define STORAGE_KEY_BACKLIGHT_ALWAYS_ON 10022
#define STORAGE_KEY_RUNNING_STATE       10030
#define STORAGE_KEY_SAVED_ELAPSED       10031
#define STORAGE_KEY_SAVED_START         10032
#define STORAGE_KEY_LAP_COUNT           10033
#define STORAGE_KEY_LAP_BASE            10040

#ifndef MESSAGE_KEY_AppKeyColorway
#define MESSAGE_KEY_AppKeyColorway            10001
#define MESSAGE_KEY_AppKeyItalicSlant        10005
#define MESSAGE_KEY_AppKeyAudioEnabled       10020
#define MESSAGE_KEY_AppKeyVibeEnabled        10021
#define MESSAGE_KEY_AppKeyBacklightAlwaysOn  10022
#endif

static Window *s_main_window;
static Layer *s_canvas_layer;
static AppTimer *s_refresh_timer = NULL;
static AppTimer *s_freeze_timer = NULL;

static int s_colorway = COLORWAY_VIBRANT_RUBY;
static bool s_italic_slant = true;
static bool s_audio_enabled = true;
static bool s_vibe_enabled = true;
static bool s_backlight_always_on = true;

static bool s_is_running = false;
static time_t s_start_sec = 0;
static uint16_t s_start_ms = 0;
static uint32_t s_accumulated_ms = 0;

static uint32_t s_laps[MAX_LAPS];
static int s_lap_count = 0;
static int s_browse_lap_idx = -1; // -1 means live view, 0..s_lap_count-1 is lap view
static bool s_is_frozen = false;
static int s_frozen_lap_idx = -1;

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

static int get_best_lap_idx(void) {
  if (s_lap_count <= 0) return -1;
  int best_idx = 0;
  uint32_t best_delta = s_laps[0];
  for (int i = 1; i < s_lap_count; i++) {
    uint32_t delta = s_laps[i] - s_laps[i - 1];
    if (delta < best_delta) {
      best_delta = delta;
      best_idx = i;
    }
  }
  return best_idx;
}

static void update_backlight(void) {
  if (s_backlight_always_on && s_is_running) {
    light_enable(true);
  } else {
    light_enable(false);
  }
}

static void refresh_timer_callback(void *data) {
  s_refresh_timer = NULL;
  if (s_is_running) {
    layer_mark_dirty(s_canvas_layer);
    s_refresh_timer = app_timer_register(REFRESH_RATE_MS, refresh_timer_callback, NULL);
  }
}

static void freeze_timer_callback(void *data) {
  s_freeze_timer = NULL;
  s_is_frozen = false;
  s_frozen_lap_idx = -1;
  layer_mark_dirty(s_canvas_layer);
}

static void start_stopwatch(void) {
  if (s_is_running) return;
  time_ms(&s_start_sec, &s_start_ms);
  s_is_running = true;
  s_browse_lap_idx = -1;
  s_is_frozen = false;
  if (s_freeze_timer) {
    app_timer_cancel(s_freeze_timer);
    s_freeze_timer = NULL;
  }
  update_backlight();
  pulsar_sound_start(s_audio_enabled, s_vibe_enabled);
  if (!s_refresh_timer) {
    s_refresh_timer = app_timer_register(REFRESH_RATE_MS, refresh_timer_callback, NULL);
  }
  layer_mark_dirty(s_canvas_layer);
}

static void stop_stopwatch(void) {
  if (!s_is_running) return;
  s_accumulated_ms = get_current_elapsed_ms();
  s_is_running = false;
  s_is_frozen = false;
  if (s_freeze_timer) {
    app_timer_cancel(s_freeze_timer);
    s_freeze_timer = NULL;
  }
  update_backlight();
  if (s_refresh_timer) {
    app_timer_cancel(s_refresh_timer);
    s_refresh_timer = NULL;
  }
  pulsar_sound_stop(s_audio_enabled, s_vibe_enabled);
  layer_mark_dirty(s_canvas_layer);
}

static void record_lap(void) {
  if (!s_is_running) return;
  uint32_t current_ms = get_current_elapsed_ms();
  if (s_lap_count < MAX_LAPS) {
    s_laps[s_lap_count] = current_ms;
    s_frozen_lap_idx = s_lap_count;
    s_lap_count++;
    s_is_frozen = true;
    pulsar_sound_lap(s_audio_enabled, s_vibe_enabled);
    if (s_freeze_timer) {
      app_timer_cancel(s_freeze_timer);
    }
    s_freeze_timer = app_timer_register(FREEZE_DURATION_MS, freeze_timer_callback, NULL);
    layer_mark_dirty(s_canvas_layer);
  }
}

static void reset_stopwatch(void) {
  if (s_is_running) return;
  s_accumulated_ms = 0;
  s_browse_lap_idx = -1;
  s_is_frozen = false;
  s_frozen_lap_idx = -1;
  pulsar_sound_reset(s_audio_enabled, s_vibe_enabled);
  layer_mark_dirty(s_canvas_layer);
}

static void clear_all_laps(void) {
  if (s_is_running) return;
  s_lap_count = 0;
  s_browse_lap_idx = -1;
  s_accumulated_ms = 0;
  s_is_frozen = false;
  s_frozen_lap_idx = -1;
  pulsar_sound_reset(s_audio_enabled, s_vibe_enabled);
  layer_mark_dirty(s_canvas_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_running) {
    stop_stopwatch();
  } else {
    if (s_browse_lap_idx >= 0) {
      s_browse_lap_idx = -1; // exit browse mode back to live total view
      pulsar_sound_start(s_audio_enabled, s_vibe_enabled);
      layer_mark_dirty(s_canvas_layer);
    } else {
      start_stopwatch();
    }
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_running) {
    record_lap();
  } else {
    // Browse recorded laps forward
    if (s_lap_count > 0) {
      if (s_browse_lap_idx == -1) {
        s_browse_lap_idx = 0;
      } else if (s_browse_lap_idx < s_lap_count - 1) {
        s_browse_lap_idx++;
      } else {
        s_browse_lap_idx = -1; // wrap back to total view
      }
      pulsar_sound_lap(s_audio_enabled, s_vibe_enabled);
      layer_mark_dirty(s_canvas_layer);
    }
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_running) {
    if (s_is_frozen) {
      if (s_freeze_timer) {
        app_timer_cancel(s_freeze_timer);
        s_freeze_timer = NULL;
      }
      s_is_frozen = false;
      s_frozen_lap_idx = -1;
      layer_mark_dirty(s_canvas_layer);
    }
  } else {
    if (s_browse_lap_idx > 0) {
      s_browse_lap_idx--;
      pulsar_sound_lap(s_audio_enabled, s_vibe_enabled);
      layer_mark_dirty(s_canvas_layer);
    } else if (s_browse_lap_idx == 0) {
      s_browse_lap_idx = -1;
      pulsar_sound_lap(s_audio_enabled, s_vibe_enabled);
      layer_mark_dirty(s_canvas_layer);
    } else {
      reset_stopwatch();
    }
  }
}

static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!s_is_running) {
    clear_all_laps();
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, 600, down_long_click_handler, NULL);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  Colorway active_palette = pulsar_get_palette(s_colorway % NUM_COLORWAYS);
  const Colorway *palette = &active_palette;

  // 1. Background Fill
  graphics_context_set_fill_color(ctx, palette->outer_bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // 2. Determine display time & mode
  uint32_t display_ms = get_current_elapsed_ms();
  bool is_browsing = false;

  if (s_is_frozen && s_frozen_lap_idx >= 0 && s_frozen_lap_idx < s_lap_count) {
    display_ms = s_laps[s_frozen_lap_idx];
  } else if (s_browse_lap_idx >= 0 && s_browse_lap_idx < s_lap_count) {
    display_ms = s_laps[s_browse_lap_idx];
    is_browsing = true;
  }

  uint32_t total_sec = display_ms / 1000;
  bool is_hours_mode = (total_sec >= 3600);

  // 3. Header
  int header_y = bounds.size.w > 180 ? 18 : 12;
  if (is_hours_mode) {
    pulsar_draw_header_at_y(ctx, bounds, header_y, "C H R O N O   [ H R ]", palette);
  } else {
    pulsar_draw_header_at_y(ctx, bounds, header_y, "C H R O N O", palette);
  }

  // 4. Main Digits & Sub-Digits Decomposition
  int d1, d2, d3, d4;
  int sub_c1, sub_c2;
  bool is_sub_colon = false;

  if (!is_hours_mode) {
    uint32_t mins = (total_sec / 60) % 100;
    uint32_t secs = total_sec % 60;
    uint32_t centis = (display_ms % 1000) / 10;
    d1 = mins / 10;
    d2 = mins % 10;
    d3 = secs / 10;
    d4 = secs % 10;
    sub_c1 = centis / 10;
    sub_c2 = centis % 10;
    is_sub_colon = false;
  } else {
    uint32_t hrs = total_sec / 3600;
    if (hrs > 99) hrs = 99;
    uint32_t mins = (total_sec % 3600) / 60;
    uint32_t secs = total_sec % 60;
    d1 = hrs / 10;
    d2 = hrs % 10;
    d3 = mins / 10;
    d4 = mins % 10;
    sub_c1 = secs / 10;
    sub_c2 = secs % 10;
    is_sub_colon = true;
  }

  // Main digits
  int main_y = bounds.size.w > 180 ? 52 : 36;
  bool colon_lit = (s_is_running && !s_is_frozen) ? ((get_current_elapsed_ms() % 1000) < 500) : true;
  pulsar_draw_4digits_at_y(ctx, bounds, main_y, d1, d2, d3, d4, true, colon_lit, palette, true, s_italic_slant);

  // Sub digits (.CC or :SS)
  int sub_spacing_x = bounds.size.w > 180 ? 5 : 4;
  int sub_spacing_y = bounds.size.w > 180 ? 6 : 5;
  int sub_dot_radius = bounds.size.w > 180 ? 2 : 1;
  int sub_gap = bounds.size.w > 180 ? 8 : 6;
  int dot_gap = bounds.size.w > 180 ? 8 : 6;
  int sub_span = (DIGIT_WIDTH - 1) * sub_spacing_x;
  int sub_y = bounds.size.w > 180 ? 120 : 84;
  int sub_slant = s_italic_slant ? sub_spacing_x : 0;
  int sub_total_w = dot_gap + (sub_span * 2) + sub_gap + sub_slant;
  int sub_start_x = (bounds.size.w - sub_total_w) / 2;

  // Separator
  int dot_x = sub_start_x;
  graphics_context_set_fill_color(ctx, palette->lit);
  if (!is_sub_colon) {
    int dot_y = sub_y + (DIGIT_HEIGHT - 1) * sub_spacing_y;
    graphics_fill_circle(ctx, GPoint(dot_x, dot_y), sub_dot_radius);
  } else {
    int colon_y1 = sub_y + (2 * sub_spacing_y);
    int colon_y2 = sub_y + (4 * sub_spacing_y);
    graphics_fill_circle(ctx, GPoint(dot_x + (s_italic_slant ? 1 : 0), colon_y1), sub_dot_radius);
    graphics_fill_circle(ctx, GPoint(dot_x, colon_y2), sub_dot_radius);
  }

  // Sub digits (pure glowing LED dots)
  int c1_x = sub_start_x + dot_gap;
  int c2_x = c1_x + sub_span + sub_gap;
  pulsar_draw_digit_custom_ghost(ctx, c1_x, sub_y, sub_c1, palette, true, bounds.size.w, 
                                sub_spacing_x, sub_spacing_y, sub_dot_radius, s_italic_slant, false);
  pulsar_draw_digit_custom_ghost(ctx, c2_x, sub_y, sub_c2, palette, true, bounds.size.w, 
                                sub_spacing_x, sub_spacing_y, sub_dot_radius, s_italic_slant, false);

  // 5. Tachymeter Micro-LED Chaser OR Lap Beads
  int bead_y = bounds.size.w > 180 ? 174 : 126;
  if (is_browsing && s_lap_count > 0) {
    bool beads[NUM_MICRO_BEADS] = {false};
    beads[s_browse_lap_idx % NUM_MICRO_BEADS] = true;
    pulsar_draw_micro_beads_at_y(ctx, bounds, bead_y, palette, true, beads);
  } else {
    pulsar_draw_tachymeter_beads_at_y(ctx, bounds, bead_y, palette, true, display_ms, s_is_running && !s_is_frozen);
  }

  // 6. Footer Status
  static char footer_buffer[32];
  int best_lap_idx = get_best_lap_idx();

  if (s_is_frozen && s_frozen_lap_idx >= 0) {
    snprintf(footer_buffer, sizeof(footer_buffer), "L A P   %02d   S E T", s_frozen_lap_idx + 1);
  } else if (is_browsing && s_browse_lap_idx >= 0) {
    if (s_browse_lap_idx == best_lap_idx && s_lap_count > 1) {
      snprintf(footer_buffer, sizeof(footer_buffer), "L A P   %02d   [ B E S T ]", s_browse_lap_idx + 1);
    } else {
      snprintf(footer_buffer, sizeof(footer_buffer), "L A P   %02d / %02d", s_browse_lap_idx + 1, s_lap_count);
    }
  } else if (s_is_running) {
    snprintf(footer_buffer, sizeof(footer_buffer), "L A P   %02d", s_lap_count + 1);
  } else if (display_ms == 0) {
    snprintf(footer_buffer, sizeof(footer_buffer), "R E A D Y");
  } else {
    if (s_lap_count > 0) {
      snprintf(footer_buffer, sizeof(footer_buffer), "P A U S E D   ( %d L )", s_lap_count);
    } else {
      snprintf(footer_buffer, sizeof(footer_buffer), "P A U S E D");
    }
  }
  int footer_y = bounds.size.w > 180 ? 194 : 144;
  pulsar_draw_footer_at_y(ctx, bounds, footer_y, footer_buffer, palette);
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
    } else if (key == MESSAGE_KEY_AppKeyBacklightAlwaysOn) {
      s_backlight_always_on = pulsar_tuple_to_bool(t, s_backlight_always_on);
      persist_write_bool(STORAGE_KEY_BACKLIGHT_ALWAYS_ON, s_backlight_always_on);
      update_backlight();
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
  if (persist_exists(STORAGE_KEY_BACKLIGHT_ALWAYS_ON)) {
    s_backlight_always_on = persist_read_bool(STORAGE_KEY_BACKLIGHT_ALWAYS_ON);
  }
  if (persist_exists(STORAGE_KEY_LAP_COUNT)) {
    s_lap_count = persist_read_int(STORAGE_KEY_LAP_COUNT);
    if (s_lap_count > MAX_LAPS) s_lap_count = MAX_LAPS;
    for (int i = 0; i < s_lap_count; i++) {
      if (persist_exists(STORAGE_KEY_LAP_BASE + i)) {
        s_laps[i] = (uint32_t)persist_read_int(STORAGE_KEY_LAP_BASE + i);
      }
    }
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
  persist_write_int(STORAGE_KEY_LAP_COUNT, s_lap_count);
  for (int i = 0; i < s_lap_count; i++) {
    persist_write_int(STORAGE_KEY_LAP_BASE + i, (int)s_laps[i]);
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
    update_backlight();
    s_refresh_timer = app_timer_register(REFRESH_RATE_MS, refresh_timer_callback, NULL);
  }
}

static void deinit(void) {
  light_enable(false);
  save_state();
  if (s_refresh_timer) {
    app_timer_cancel(s_refresh_timer);
    s_refresh_timer = NULL;
  }
  if (s_freeze_timer) {
    app_timer_cancel(s_freeze_timer);
    s_freeze_timer = NULL;
  }
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
