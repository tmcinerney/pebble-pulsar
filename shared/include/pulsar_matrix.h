#pragma once
#include <pebble.h>
#include "pulsar_palette.h"
#include "pulsar_layout.h"

// Glyph indices in FONT_5X7
#define GLYPH_BLANK     10
#define GLYPH_B         11
#define GLYPH_A         12
#define GLYPH_T         13
#define GLYPH_PERCENT   14
#define GLYPH_H         15
#define GLYPH_R         16
#define GLYPH_DASH      17
#define GLYPH_HEART     18
#define GLYPH_C         19
#define GLYPH_L         20
#define GLYPH_P         21
#define GLYPH_S         22
#define GLYPH_O         23
#define GLYPH_N         24
#define GLYPH_F         25
#define GLYPH_E         26
#define GLYPH_U         27
#define GLYPH_D         28
#define GLYPH_Y         29
#define GLYPH_M         30
#define GLYPH_I         31
#define GLYPH_W         32
#define GLYPH_K         33
#define GLYPH_DOT       34
#define NUM_GLYPHS      35

int pulsar_char_to_glyph(char c);

void pulsar_draw_digit_custom(GContext *ctx, int x_offset, int y_offset, int digit_index, 
                             const Colorway *palette, bool is_active, int bounds_w, 
                             int spacing_x, int spacing_y, int dot_radius, bool italic_slant);

void pulsar_draw_digit(GContext *ctx, int x_offset, int y_offset, int digit_index, 
                       const Colorway *palette, bool is_active, int bounds_w, bool italic_slant);

void pulsar_draw_colon(GContext *ctx, int colon_base_x, int start_y, 
                       const Colorway *palette, bool is_active, bool colon_lit, 
                       int bounds_w, bool italic_slant);

void pulsar_draw_4digits(GContext *ctx, GRect bounds, int d1, int d2, int d3, int d4, 
                         bool show_colon, bool colon_lit, const Colorway *palette, 
                         bool is_active, bool italic_slant);

void pulsar_draw_5digits(GContext *ctx, GRect bounds, const int digits[5], 
                         const Colorway *palette, bool is_active, bool italic_slant);
