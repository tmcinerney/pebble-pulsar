#include <pebble.h>

/**
 * 1970s Hamilton Pulsar Watchface for Pebble Time / Pebble Time 2
 * 
 * Inspired by the Pulsar P1 / P2 / P3 "Time Computer".
 * Features:
 * - Procedural GaAsP 5x7 Red Dot-Matrix LED digits.
 * - Unlit "ghost" LED dies rendered beneath the synthetic ruby crystal.
 * - Dynamic geometry scaling for Pebble Time 2 (Emery: 200x228) and Pebble Time (Basalt: 144x168).
 * - Accelerometer wrist-flick / tap wake (authentic 1970s power-save mode).
 * - Pulsar P3 Date Command toggle (Month / Day) on flick.
 * - Sub-LED indicators for Bluetooth connectivity and Battery status.
 */

#if defined(PBL_PLATFORM_EMERY)
  #define DOT_RADIUS        3
  #define DOT_SPACING       8
  #define DIGIT_SPACING     46
  #define COLON_OFFSET      7
  #define TOP_MARGIN        78
  #define HEADER_FONT       FONT_KEY_GOTHIC_18_BOLD
  #define FOOTER_FONT       FONT_KEY_GOTHIC_14
  #define INDICATOR_RADIUS  2
#else // Basalt, Diorite, Aplite (144x168)
  #define DOT_RADIUS        2
  #define DOT_SPACING       6
  #define DIGIT_SPACING     34
  #define COLON_OFFSET      5
  #define TOP_MARGIN        58
  #define HEADER_FONT       FONT_KEY_GOTHIC_14_BOLD
  #define FOOTER_FONT       FONT_KEY_GOTHIC_14
  #define INDICATOR_RADIUS  1
#endif

#define DIGIT_WIDTH 5
#define DIGIT_HEIGHT 7
#define WAKE_DURATION_MS 4000

static Window *s_main_window;
static Layer *s_canvas_layer;
static AppTimer *s_wake_timer = NULL;
static bool s_is_awake = true;   // Starts awake on load
static bool s_show_date = false;  // Toggle between Time and P3 Date Mode
static bool s_bluetooth_connected = true;
static int s_battery_level = 100;

// 5x7 GaAsP Dot Matrix Font (0-9, Blank)
static const uint8_t FONT_5X7[11][7] = {
    {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F}, // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}, // 1
    {0x1F, 0x01, 0x01, 0x1F, 0x10, 0x10, 0x1F}, // 2
    {0x1F, 0x01, 0x01, 0x1F, 0x01, 0x01, 0x1F}, // 3
    {0x11, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x01}, // 4
    {0x1F, 0x10, 0x10, 0x1F, 0x01, 0x01, 0x1F}, // 5
    {0x1F, 0x10, 0x10, 0x1F, 0x11, 0x11, 0x1F}, // 6
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}, // 7
    {0x1F, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x1F}, // 8
    {0x1F, 0x11, 0x11, 0x1F, 0x01, 0x01, 0x1F}, // 9
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 10 = Blank
};

static void wake_timer_callback(void *data) {
    s_wake_timer = NULL;
    s_is_awake = false; // Return to authentic stealth / ghost mode
    s_show_date = false;
    layer_mark_dirty(s_canvas_layer);
}

static void trigger_wake(bool toggle_date) {
    s_is_awake = true;
    if (toggle_date) {
        s_show_date = !s_show_date;
    }
    if (s_wake_timer) {
        app_timer_cancel(s_wake_timer);
    }
    s_wake_timer = app_timer_register(WAKE_DURATION_MS, wake_timer_callback, NULL);
    layer_mark_dirty(s_canvas_layer);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
    // Wrist flick or tap wakes up the LEDs and toggles date
    trigger_wake(true);
}

static void bluetooth_callback(bool connected) {
    s_bluetooth_connected = connected;
    layer_mark_dirty(s_canvas_layer);
}

static void battery_callback(BatteryChargeState state) {
    s_battery_level = state.charge_percent;
    layer_mark_dirty(s_canvas_layer);
}

static void draw_matrix_digit(GContext *ctx, int x_offset, int y_offset, int digit_index) {
    if (digit_index < 0 || digit_index > 10) digit_index = 10;
    
    GColor lit_color = GColorRed;
    GColor unlit_color = GColorBulgarianRose; // Unlit GaAsP ghost die
    
    for (int r = 0; r < DIGIT_HEIGHT; r++) {
        uint8_t row_bits = FONT_5X7[digit_index][r];
        for (int c = 0; c < DIGIT_WIDTH; c++) {
            bool is_lit = (row_bits >> (4 - c)) & 0x01;
            int dot_x = x_offset + (c * DOT_SPACING);
            int dot_y = y_offset + (r * DOT_SPACING);
            
            if (is_lit && s_is_awake) {
                graphics_context_set_fill_color(ctx, lit_color);
                graphics_fill_circle(ctx, GPoint(dot_x, dot_y), DOT_RADIUS);
            } else {
                // Subtle unlit ghost LED die visible under the ruby crystal
                graphics_context_set_fill_color(ctx, unlit_color);
                graphics_fill_circle(ctx, GPoint(dot_x, dot_y), (DOT_RADIUS > 2) ? (DOT_RADIUS - 1) : 1);
            }
        }
    }
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    
    // 1. Synthetic Ruby Crystal / Black Dial Background
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
    
    // 2. Cushion Bezel Inner Chamfer (Emery highlight)
#if defined(PBL_PLATFORM_EMERY)
    graphics_context_set_stroke_color(ctx, GColorBulgarianRose);
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_round_rect(ctx, grect_inset(bounds, GEdgeInsets(4)), 8);
#endif

    // 3. Vintage "P U L S A R" Space-Age Header
    graphics_context_set_text_color(ctx, GColorDarkCandyAppleRed);
    graphics_draw_text(ctx, "P U L S A R",
                       fonts_get_system_font(HEADER_FONT),
                       GRect(0, bounds.size.h / 14, bounds.size.w, 24),
                       GTextOverflowModeWordWrap,
                       GTextAlignmentCenter,
                       NULL);

    // 4. Time and Date logic
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    int d1, d2, d3, d4;
    if (s_show_date) {
        // P3 Mode: Month and Day
        int month = tick_time->tm_mon + 1;
        int day = tick_time->tm_mday;
        d1 = month / 10;
        d2 = month % 10;
        d3 = day / 10;
        d4 = day % 10;
    } else {
        // Time Mode: Hours and Minutes
        int hours = tick_time->tm_hour;
        if (!clock_is_24h_style()) {
            hours = hours % 12;
            if (hours == 0) hours = 12;
        }
        int mins = tick_time->tm_min;
        d1 = (hours >= 10) ? (hours / 10) : 10; // Blank leading zero if single digit
        d2 = hours % 10;
        d3 = mins / 10;
        d4 = mins % 10;
    }
    
    // 5. Centered Horizontal Origin Calculation
    int total_width = (DIGIT_SPACING * 3) + (DIGIT_WIDTH * DOT_SPACING) + (COLON_OFFSET * 2);
    int start_x = (bounds.size.w - total_width) / 2;
    int start_y = TOP_MARGIN;
    
    // Digits 1 & 2 (Hours or Month)
    draw_matrix_digit(ctx, start_x, start_y, d1);
    draw_matrix_digit(ctx, start_x + DIGIT_SPACING, start_y, d2);
    
    // Colon Dots (Active in Time mode, unlit in Date mode)
    int colon_x = start_x + (DIGIT_SPACING * 2) - COLON_OFFSET;
    int colon_y1 = start_y + (DOT_SPACING * 2);
    int colon_y2 = start_y + (DOT_SPACING * 4);
    GColor colon_color = (s_is_awake && !s_show_date) ? GColorRed : GColorBulgarianRose;
    graphics_context_set_fill_color(ctx, colon_color);
    graphics_fill_circle(ctx, GPoint(colon_x, colon_y1), DOT_RADIUS);
    graphics_fill_circle(ctx, GPoint(colon_x, colon_y2), DOT_RADIUS);
    
    // Digits 3 & 4 (Minutes or Day)
    int right_x = start_x + (DIGIT_SPACING * 2) + (COLON_OFFSET * 2);
    draw_matrix_digit(ctx, right_x, start_y, d3);
    draw_matrix_digit(ctx, right_x + DIGIT_SPACING, start_y, d4);
    
    // 6. AM/PM Indicator Dot (GaAsP LED dot on bottom-left)
    if (!clock_is_24h_style() && !s_show_date) {
        bool is_pm = tick_time->tm_hour >= 12;
        GColor pm_dot_color = (s_is_awake && is_pm) ? GColorRed : GColorBulgarianRose;
        graphics_context_set_fill_color(ctx, pm_dot_color);
        graphics_fill_circle(ctx, GPoint(start_x, start_y + (DIGIT_HEIGHT * DOT_SPACING) + 6), INDICATOR_RADIUS);
    }
    
    // 7. Status Dots (Bottom Indicators)
    // Left dot: Bluetooth Disconnected warning (Red if disconnected, dark if OK)
    if (!s_bluetooth_connected) {
        graphics_context_set_fill_color(ctx, GColorRed);
        graphics_fill_circle(ctx, GPoint(bounds.size.w / 4, bounds.size.h - 48), INDICATOR_RADIUS);
    }
    
    // Right dot: Low Battery warning (Red if < 20%)
    if (s_battery_level <= 20) {
        graphics_context_set_fill_color(ctx, GColorRed);
        graphics_fill_circle(ctx, GPoint((bounds.size.w * 3) / 4, bounds.size.h - 48), INDICATOR_RADIUS);
    }

    // 8. Vintage "TIME COMPUTER" Sub-text
    graphics_context_set_text_color(ctx, GColorBulgarianRose);
    graphics_draw_text(ctx, "TIME COMPUTER",
                       fonts_get_system_font(FOOTER_FONT),
                       GRect(0, bounds.size.h - 32, bounds.size.w, 20),
                       GTextOverflowModeWordWrap,
                       GTextAlignmentCenter,
                       NULL);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    layer_mark_dirty(s_canvas_layer);
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
    s_main_window = window_create();
    window_set_background_color(s_main_window, GColorBlack);
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(s_main_window, true);
    
    // Subscribe to services
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    accel_tap_service_subscribe(tap_handler);
    connection_service_subscribe((ConnectionHandlers) {
        .pebble_app_connection_handler = bluetooth_callback
    });
    battery_state_service_subscribe(battery_callback);
    
    // Initial state
    s_bluetooth_connected = connection_service_peek_pebble_app_connection();
    BatteryChargeState charge_state = battery_state_service_peek();
    s_battery_level = charge_state.charge_percent;
    
    // Auto-dim after initial wake
    s_wake_timer = app_timer_register(WAKE_DURATION_MS, wake_timer_callback, NULL);
}

static void deinit(void) {
    battery_state_service_unsubscribe();
    connection_service_unsubscribe();
    accel_tap_service_unsubscribe();
    tick_timer_service_unsubscribe();
    if (s_wake_timer) {
        app_timer_cancel(s_wake_timer);
    }
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
