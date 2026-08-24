#pragma once
#include <pebble.h>
#include "pulsar_palette.h"
#include "pulsar_layout.h"

void pulsar_draw_header_at_y(GContext *ctx, GRect bounds, int header_y, const char *header_text, const Colorway *palette);
void pulsar_draw_header(GContext *ctx, GRect bounds, const char *header_text, const Colorway *palette);
void pulsar_draw_footer_at_y(GContext *ctx, GRect bounds, int footer_y, const char *footer_text, const Colorway *palette);
void pulsar_draw_footer(GContext *ctx, GRect bounds, const char *footer_text, const Colorway *palette);
void pulsar_draw_alert_banners(GContext *ctx, GRect bounds, const Colorway *palette);
void pulsar_draw_annunciators(GContext *ctx, GRect bounds, const Colorway *palette, 
                             bool bt_disconnected, bool battery_low, bool is_charging);
