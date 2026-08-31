#include <pebble.h>
#include "pulsar_palette.h"
#include "pulsar_layout.h"
#include "pulsar_matrix.h"
#include "pulsar_micro_bar.h"
#include "pulsar_brand.h"
#include "pulsar_audio.h"

#define WAKEUP_COOKIE_ALARM 1972
#define NUM_ALARMS 4

enum AlarmRepeat {
  REPEAT_DAILY = 0,
  REPEAT_WEEKDAYS = 1,
  REPEAT_WEEKENDS = 2,
  REPEAT_ONCE = 3
};
#define NUM_REPEATS 4

static const char *REPEAT_NAMES[NUM_REPEATS] = {
  "D A I L Y",
  "W E E K D A Y S",
  "W E E K E N D S",
  "O N C E"
};

typedef struct {
  int hour;
  int minute;
  bool enabled;
  int repeat;
} AlarmSlot;

enum EditMode {
  EDIT_NONE = 0,
  EDIT_HOUR = 1,
  EDIT_MINUTE = 2,
  EDIT_REPEAT = 3
};

#define STORAGE_KEY_COLORWAY        10001
#define STORAGE_KEY_ITALIC_SLANT    10005
#define STORAGE_KEY_SHOW_GHOST     10025
#define STORAGE_KEY_LED_BRIGHTNESS 10026
#define STORAGE_KEY_LED_GLOW       10028
#define STORAGE_KEY_AUDIO_ENABLED   10020
#define STORAGE_KEY_VIBE_ENABLED    10021
#define STORAGE_KEY_SNOOZE_DUR      10050
#define STORAGE_KEY_ALARM_SLOTS     10060
#define STORAGE_KEY_WAKEUP_ID       10061

#ifndef MESSAGE_KEY_AppKeyColorway
#define MESSAGE_KEY_AppKeyColorway       10001
#define MESSAGE_KEY_AppKeyItalicSlant   10005
#define MESSAGE_KEY_AppKeyAudioEnabled  10020
#define MESSAGE_KEY_AppKeyVibeEnabled   10021
#define MESSAGE_KEY_AppKeySnoozeDuration 10050
#endif

static Window *s_main_window;
static Layer *s_canvas_layer;
static AppTimer *s_ring_timer = NULL;
static AppTimer *s_blink_timer = NULL;

static int s_colorway = COLORWAY_VIBRANT_RUBY;
// Ghost dots off by default across the suite: they grey out the gaps between lit dots and
// flatten the glow. Matches the watchface so a fresh install looks consistent.
static bool s_show_ghost = false;
static bool s_bright_leds = false;
static bool s_led_glow = true;
static bool s_italic_slant = true;
static bool s_audio_enabled = true;
static bool s_vibe_enabled = true;
static int s_snooze_duration_min = 9;

static AlarmSlot s_alarms[NUM_ALARMS] = {
  { 7, 0, true, REPEAT_WEEKDAYS },
  { 8, 30, false, REPEAT_WEEKENDS },
  { 12, 0, false, REPEAT_DAILY },
  { 18, 0, false, REPEAT_ONCE }
};

static int s_active_slot_idx = 0;
static int s_edit_mode = EDIT_NONE;
static bool s_is_ringing = false;
static int s_ring_pulse_count = 0;
static bool s_blink_state = true;
static WakeupId s_scheduled_wakeup_id = -1;

static time_t compute_next_epoch_for_alarm(const AlarmSlot *slot, time_t now) {
  if (!slot->enabled) return 0;

  struct tm *t = localtime(&now);
  struct tm alarm_tm = *t;
  alarm_tm.tm_hour = slot->hour;
  alarm_tm.tm_min = slot->minute;
  alarm_tm.tm_sec = 0;

  time_t candidate = mktime(&alarm_tm);
  if (candidate <= now) {
    candidate += 86400; // Add 1 day
  }

  for (int day_offset = 0; day_offset < 8; day_offset++) {
    struct tm *cand_tm = localtime(&candidate);
    int wday = cand_tm->tm_wday; // 0 = Sun, 1 = Mon ... 6 = Sat
    bool valid = false;

    switch (slot->repeat) {
      case REPEAT_DAILY:
      case REPEAT_ONCE:
        valid = true;
        break;
      case REPEAT_WEEKDAYS:
        valid = (wday >= 1 && wday <= 5);
        break;
      case REPEAT_WEEKENDS:
        valid = (wday == 0 || wday == 6);
        break;
    }

    if (valid) {
      return candidate;
    }
    candidate += 86400;
  }
  return 0;
}

static void schedule_earliest_alarm(void) {
  if (s_scheduled_wakeup_id >= 0) {
    wakeup_cancel(s_scheduled_wakeup_id);
    s_scheduled_wakeup_id = -1;
  }

  time_t now = time(NULL);
  time_t earliest = 0;

  for (int i = 0; i < NUM_ALARMS; i++) {
    if (s_alarms[i].enabled) {
      time_t t_next = compute_next_epoch_for_alarm(&s_alarms[i], now);
      if (t_next > now) {
        if (earliest == 0 || t_next < earliest) {
          earliest = t_next;
        }
      }
    }
  }

  if (earliest > now) {
    s_scheduled_wakeup_id = wakeup_schedule(earliest, WAKEUP_COOKIE_ALARM, true);
  }
}

static void ring_pulse_callback(void *data) {
  s_ring_timer = NULL;
  if (s_is_ringing) {
    s_ring_pulse_count++;
    pulsar_sound_alarm_pulse(s_audio_enabled, s_vibe_enabled);
    layer_mark_dirty(s_canvas_layer);
    // Limit continuous ringing to 60 pulses (30 seconds)
    if (s_ring_pulse_count < 60) {
      s_ring_timer = app_timer_register(500, ring_pulse_callback, NULL);
    } else {
      light_enable(false);
    }
  }
}

static void start_alarm_ringing(void) {
  s_is_ringing = true;
  s_edit_mode = EDIT_NONE;
  s_ring_pulse_count = 0;
  light_enable(true);
  ring_pulse_callback(NULL);
}

static void dismiss_alarm(void) {
  s_is_ringing = false;
  light_enable(false);
  if (s_ring_timer) {
    app_timer_cancel(s_ring_timer);
    s_ring_timer = NULL;
  }
  
  if (s_alarms[s_active_slot_idx].repeat == REPEAT_ONCE) {
    s_alarms[s_active_slot_idx].enabled = false;
  }
  schedule_earliest_alarm();
  layer_mark_dirty(s_canvas_layer);
}

static void snooze_alarm(void) {
  s_is_ringing = false;
  light_enable(false);
  if (s_ring_timer) {
    app_timer_cancel(s_ring_timer);
    s_ring_timer = NULL;
  }
  
  time_t now = time(NULL);
  time_t snooze_target = now + (s_snooze_duration_min * 60);
  if (s_scheduled_wakeup_id >= 0) {
    wakeup_cancel(s_scheduled_wakeup_id);
  }
  s_scheduled_wakeup_id = wakeup_schedule(snooze_target, WAKEUP_COOKIE_ALARM, true);
  
  pulsar_sound_lap(s_audio_enabled, s_vibe_enabled);
  layer_mark_dirty(s_canvas_layer);
}

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

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_ringing) {
    snooze_alarm();
    return;
  }

  if (s_edit_mode == EDIT_NONE) {
    // Toggle active alarm ON / OFF
    s_alarms[s_active_slot_idx].enabled = !s_alarms[s_active_slot_idx].enabled;
    if (s_alarms[s_active_slot_idx].enabled) {
      pulsar_sound_start(s_audio_enabled, s_vibe_enabled);
    } else {
      pulsar_sound_stop(s_audio_enabled, s_vibe_enabled);
    }
    schedule_earliest_alarm();
    layer_mark_dirty(s_canvas_layer);
  } else if (s_edit_mode == EDIT_HOUR) {
    s_edit_mode = EDIT_MINUTE;
    pulsar_sound_start(s_audio_enabled, s_vibe_enabled);
    layer_mark_dirty(s_canvas_layer);
  } else if (s_edit_mode == EDIT_MINUTE) {
    s_edit_mode = EDIT_REPEAT;
    pulsar_sound_start(s_audio_enabled, s_vibe_enabled);
    layer_mark_dirty(s_canvas_layer);
  } else if (s_edit_mode == EDIT_REPEAT) {
    s_edit_mode = EDIT_NONE;
    s_alarms[s_active_slot_idx].enabled = true; // Auto-enable edited alarm
    pulsar_sound_lap(s_audio_enabled, s_vibe_enabled);
    update_blink_loop();
    schedule_earliest_alarm();
    layer_mark_dirty(s_canvas_layer);
  }
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_ringing) {
    dismiss_alarm();
    return;
  }

  if (s_edit_mode == EDIT_NONE) {
    s_edit_mode = EDIT_HOUR;
    s_blink_state = true;
    pulsar_sound_lap(s_audio_enabled, s_vibe_enabled);
    update_blink_loop();
    layer_mark_dirty(s_canvas_layer);
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_ringing) {
    snooze_alarm();
    return;
  }

  if (s_edit_mode == EDIT_NONE) {
    // Switch to previous alarm slot
    s_active_slot_idx--;
    if (s_active_slot_idx < 0) s_active_slot_idx = NUM_ALARMS - 1;
    pulsar_sound_start(s_audio_enabled, s_vibe_enabled);
    layer_mark_dirty(s_canvas_layer);
  } else if (s_edit_mode == EDIT_HOUR) {
    s_alarms[s_active_slot_idx].hour = (s_alarms[s_active_slot_idx].hour + 1) % 24;
    layer_mark_dirty(s_canvas_layer);
  } else if (s_edit_mode == EDIT_MINUTE) {
    s_alarms[s_active_slot_idx].minute = (s_alarms[s_active_slot_idx].minute + 1) % 60;
    layer_mark_dirty(s_canvas_layer);
  } else if (s_edit_mode == EDIT_REPEAT) {
    s_alarms[s_active_slot_idx].repeat = (s_alarms[s_active_slot_idx].repeat + 1) % NUM_REPEATS;
    layer_mark_dirty(s_canvas_layer);
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_is_ringing) {
    snooze_alarm();
    return;
  }

  if (s_edit_mode == EDIT_NONE) {
    // Switch to next alarm slot
    s_active_slot_idx++;
    if (s_active_slot_idx >= NUM_ALARMS) s_active_slot_idx = 0;
    pulsar_sound_start(s_audio_enabled, s_vibe_enabled);
    layer_mark_dirty(s_canvas_layer);
  } else if (s_edit_mode == EDIT_HOUR) {
    s_alarms[s_active_slot_idx].hour = (s_alarms[s_active_slot_idx].hour + 23) % 24;
    layer_mark_dirty(s_canvas_layer);
  } else if (s_edit_mode == EDIT_MINUTE) {
    s_alarms[s_active_slot_idx].minute = (s_alarms[s_active_slot_idx].minute + 59) % 60;
    layer_mark_dirty(s_canvas_layer);
  } else if (s_edit_mode == EDIT_REPEAT) {
    s_alarms[s_active_slot_idx].repeat = (s_alarms[s_active_slot_idx].repeat + NUM_REPEATS - 1) % NUM_REPEATS;
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
  start_alarm_ringing();
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  Colorway active_palette = pulsar_get_palette(s_colorway % NUM_COLORWAYS);
  const Colorway *palette = &active_palette;
  const AlarmSlot *slot = &s_alarms[s_active_slot_idx];

  // 1. Background Fill: Maintain dark background so red LED dots stay 100% visible
  graphics_context_set_fill_color(ctx, palette->outer_bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  bool is_flash_frame = s_is_ringing && (s_ring_pulse_count % 2 == 1);

  // 2. Alert Beacon Frame & Banner fills when ringing
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
  static char header_buffer[32];
  if (s_is_ringing) {
    snprintf(header_buffer, sizeof(header_buffer), "*  A L A R M  *");
  } else if (s_edit_mode != EDIT_NONE) {
    snprintf(header_buffer, sizeof(header_buffer), "S E T   A L A R M   %d", s_active_slot_idx + 1);
  } else {
    snprintf(header_buffer, sizeof(header_buffer), "A L A R M   %d / %d", s_active_slot_idx + 1, NUM_ALARMS);
  }
  pulsar_draw_header(ctx, bounds, header_buffer, &banner_palette);

  // 4. Digits calculation
  int display_hour = slot->hour;
  if (!clock_is_24h_style()) {
    display_hour = display_hour % 12;
    if (display_hour == 0) display_hour = 12;
  }

  int d1 = display_hour / 10;
  int d2 = display_hour % 10;
  int d3 = slot->minute / 10;
  int d4 = slot->minute % 10;

  // Handle blinking when in edit mode
  if (s_edit_mode == EDIT_HOUR && !s_blink_state) {
    d1 = GLYPH_BLANK;
    d2 = GLYPH_BLANK;
  } else if (s_edit_mode == EDIT_MINUTE && !s_blink_state) {
    d3 = GLYPH_BLANK;
    d4 = GLYPH_BLANK;
  }

  bool is_active = slot->enabled || (s_edit_mode != EDIT_NONE) || s_is_ringing;
  pulsar_draw_4digits(ctx, bounds, d1, d2, d3, d4, true, is_active, palette, is_active, s_italic_slant);

  // 5. Micro-LED Bar: Slots 1-4 indicator or Days
  bool beads[NUM_MICRO_BEADS] = {false};
  if (s_is_ringing) {
    for (int i = 0; i < NUM_MICRO_BEADS; i++) beads[i] = (s_ring_pulse_count % 2 == 0);
  } else {
    // Light bead corresponding to active slot and enabled alarms
    for (int i = 0; i < NUM_ALARMS; i++) {
      if (s_alarms[i].enabled) {
        beads[i * 2 + 1] = true;
      }
    }
    beads[s_active_slot_idx * 2 + 1] = true;
  }
  pulsar_draw_micro_beads(ctx, bounds, palette, is_active, beads);

  // 6. Vintage Space-Age Footer
  static char footer_buffer[32];
  if (s_is_ringing) {
    snprintf(footer_buffer, sizeof(footer_buffer), "S N O O Z E   ( %d M )", s_snooze_duration_min);
  } else if (s_edit_mode == EDIT_HOUR) {
    snprintf(footer_buffer, sizeof(footer_buffer), "E D I T   H O U R");
  } else if (s_edit_mode == EDIT_MINUTE) {
    snprintf(footer_buffer, sizeof(footer_buffer), "E D I T   M I N");
  } else if (s_edit_mode == EDIT_REPEAT) {
    snprintf(footer_buffer, sizeof(footer_buffer), "%s", REPEAT_NAMES[slot->repeat]);
  } else {
    snprintf(footer_buffer, sizeof(footer_buffer), "%s   %s", 
             REPEAT_NAMES[slot->repeat], 
             slot->enabled ? "[ O N ]" : "[ O F F ]");
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
    } else if (key == MESSAGE_KEY_AppKeyShowGhost) {
      s_show_ghost = pulsar_tuple_to_bool(t, s_show_ghost);
      persist_write_bool(STORAGE_KEY_SHOW_GHOST, s_show_ghost);
      pulsar_set_ghost_enabled(s_show_ghost);
    } else if (key == MESSAGE_KEY_AppKeyLedBrightness) {
      s_bright_leds = pulsar_tuple_to_bool(t, s_bright_leds);
      persist_write_bool(STORAGE_KEY_LED_BRIGHTNESS, s_bright_leds);
      pulsar_set_bright_leds(s_bright_leds);
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
    } else if (key == MESSAGE_KEY_AppKeySnoozeDuration) {
      s_snooze_duration_min = pulsar_tuple_to_int(t, s_snooze_duration_min);
      persist_write_int(STORAGE_KEY_SNOOZE_DUR, s_snooze_duration_min);
    }
  }
  layer_mark_dirty(s_canvas_layer);
}

static void load_state(void) {
  if (persist_exists(STORAGE_KEY_SHOW_GHOST)) {
    s_show_ghost = persist_read_bool(STORAGE_KEY_SHOW_GHOST);
  }
  pulsar_set_ghost_enabled(s_show_ghost);
  if (persist_exists(STORAGE_KEY_LED_BRIGHTNESS)) {
    // Older installs stored a 0-2 level; any non-zero maps onto the bright position.
    s_bright_leds = persist_read_int(STORAGE_KEY_LED_BRIGHTNESS) != 0;
  }
  pulsar_set_bright_leds(s_bright_leds);
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
  if (persist_exists(STORAGE_KEY_SNOOZE_DUR)) {
    s_snooze_duration_min = persist_read_int(STORAGE_KEY_SNOOZE_DUR);
    if (s_snooze_duration_min <= 0) s_snooze_duration_min = 9;
  }
  if (persist_exists(STORAGE_KEY_ALARM_SLOTS)) {
    persist_read_data(STORAGE_KEY_ALARM_SLOTS, s_alarms, sizeof(s_alarms));
  }
}

static void save_state(void) {
  persist_write_data(STORAGE_KEY_ALARM_SLOTS, s_alarms, sizeof(s_alarms));
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

  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    WakeupId id = 0;
    int32_t reason = 0;
    wakeup_get_launch_event(&id, &reason);
    start_alarm_ringing();
  } else {
    schedule_earliest_alarm();
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
}

static void deinit(void) {
  light_enable(false);
  save_state();
  if (s_ring_timer) {
    app_timer_cancel(s_ring_timer);
    s_ring_timer = NULL;
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
