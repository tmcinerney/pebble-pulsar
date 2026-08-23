#include "pulsar_matrix.h"

// Authentic 1970s Hamilton Pulsar Rigid 5x7 GaAsP LED Matrix Font
static const uint8_t FONT_5X7[NUM_GLYPHS][7] = {
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
    {0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03}, // 14 = '%'
    {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}, // 15 = 'H'
    {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}, // 16 = 'R'
    {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00}, // 17 = '-'
    {0x0A, 0x1F, 0x1F, 0x1F, 0x0E, 0x04, 0x00}, // 18 = '♥' (Heart)
    {0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}, // 19 = 'C'
    {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}, // 20 = 'L'
    {0x1F, 0x11, 0x11, 0x1F, 0x10, 0x10, 0x10}, // 21 = 'P'
    {0x1F, 0x10, 0x10, 0x1F, 0x01, 0x01, 0x1F}, // 22 = 'S'
    {0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F}, // 23 = 'O'
    {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11}, // 24 = 'N'
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10}, // 25 = 'F'
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}, // 26 = 'E'
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F}, // 27 = 'U'
    {0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E}, // 28 = 'D'
    {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04}, // 29 = 'Y'
    {0x11, 0x1B, 0x15, 0x11, 0x11, 0x11, 0x11}, // 30 = 'M'
    {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E}, // 31 = 'I'
    {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11}, // 32 = 'W'
    {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}, // 33 = 'K'
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06}  // 34 = '.'
};

int pulsar_char_to_glyph(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  switch (c) {
    case ' ': return GLYPH_BLANK;
    case 'B': case 'b': return GLYPH_B;
    case 'A': case 'a': return GLYPH_A;
    case 'T': case 't': return GLYPH_T;
    case '%': return GLYPH_PERCENT;
    case 'H': case 'h': return GLYPH_H;
    case 'R': case 'r': return GLYPH_R;
    case '-': return GLYPH_DASH;
    case 'C': case 'c': return GLYPH_C;
    case 'L': case 'l': return GLYPH_L;
    case 'P': case 'p': return GLYPH_P;
    case 'S': case 's': return GLYPH_S;
    case 'O': case 'o': return GLYPH_O;
    case 'N': case 'n': return GLYPH_N;
    case 'F': case 'f': return GLYPH_F;
    case 'E': case 'e': return GLYPH_E;
    case 'U': case 'u': return GLYPH_U;
    case 'D': case 'd': return GLYPH_D;
    case 'Y': case 'y': return GLYPH_Y;
    case 'M': case 'm': return GLYPH_M;
    case 'I': case 'i': return GLYPH_I;
    case 'W': case 'w': return GLYPH_W;
    case 'K': case 'k': return GLYPH_K;
    case '.': return GLYPH_DOT;
    default: return GLYPH_BLANK;
  }
}

void pulsar_draw_digit_custom(GContext *ctx, int x_offset, int y_offset, int digit_index, 
                             const Colorway *palette, bool is_active, int bounds_w, 
                             int spacing_x, int spacing_y, int dot_radius, bool italic_slant) {
  if (digit_index < 0 || digit_index >= NUM_GLYPHS) digit_index = GLYPH_BLANK;
  
  int slant_scale = (bounds_w > 180) ? 7 : 5;
  
  for (int r = 0; r < DIGIT_HEIGHT; r++) {
    uint8_t row_bits = FONT_5X7[digit_index][r];
    int slant_x = italic_slant ? (((DIGIT_HEIGHT - 1 - r) * slant_scale) / (DIGIT_HEIGHT - 1)) : 0;
    
    for (int c = 0; c < DIGIT_WIDTH; c++) {
      bool is_lit = is_active && ((row_bits >> (4 - c)) & 0x01);
      int dot_x = x_offset + (c * spacing_x) + slant_x;
      int dot_y = y_offset + (r * spacing_y);
      
      if (is_lit) {
        graphics_context_set_fill_color(ctx, palette->lit);
        graphics_fill_circle(ctx, GPoint(dot_x, dot_y), dot_radius);
      } else {
#if defined(PBL_COLOR)
        // Subtle ghost die beneath dark crystal on 64-color displays
        graphics_context_set_fill_color(ctx, palette->ghost);
        graphics_fill_circle(ctx, GPoint(dot_x, dot_y), 1);
#else
        // Pure black / white on 1-bit monochrome (no dither noise)
#endif
      }
    }
  }
}

void pulsar_draw_digit(GContext *ctx, int x_offset, int y_offset, int digit_index, 
                       const Colorway *palette, bool is_active, int bounds_w, bool italic_slant) {
  pulsar_draw_digit_custom(ctx, x_offset, y_offset, digit_index, palette, is_active, bounds_w, 
                           DOT_SPACING_X, DOT_SPACING_Y, DOT_RADIUS, italic_slant);
}

void pulsar_draw_colon(GContext *ctx, int colon_base_x, int start_y, 
                       const Colorway *palette, bool is_active, bool colon_lit, 
                       int bounds_w, bool italic_slant) {
  int slant_scale = (bounds_w > 180) ? 7 : 5;
  int colon_slant1 = italic_slant ? (((DIGIT_HEIGHT - 1 - 2) * slant_scale) / (DIGIT_HEIGHT - 1)) : 0;
  int colon_slant2 = italic_slant ? (((DIGIT_HEIGHT - 1 - 4) * slant_scale) / (DIGIT_HEIGHT - 1)) : 0;
  int colon_x1 = colon_base_x + colon_slant1;
  int colon_x2 = colon_base_x + colon_slant2;
  int colon_y1 = start_y + (DOT_SPACING_Y * 2);
  int colon_y2 = start_y + (DOT_SPACING_Y * 4);
  
#if defined(PBL_COLOR)
  GColor colon_color = (is_active && colon_lit) ? palette->lit : palette->ghost;
  graphics_context_set_fill_color(ctx, colon_color);
  graphics_fill_circle(ctx, GPoint(colon_x1, colon_y1), (is_active && colon_lit) ? DOT_RADIUS : 1);
  graphics_fill_circle(ctx, GPoint(colon_x2, colon_y2), (is_active && colon_lit) ? DOT_RADIUS : 1);
#else
  if (is_active && colon_lit) {
    graphics_context_set_fill_color(ctx, palette->lit);
    graphics_fill_circle(ctx, GPoint(colon_x1, colon_y1), DOT_RADIUS);
    graphics_fill_circle(ctx, GPoint(colon_x2, colon_y2), DOT_RADIUS);
  }
#endif
}

void pulsar_draw_4digits(GContext *ctx, GRect bounds, int d1, int d2, int d3, int d4, 
                         bool show_colon, bool colon_lit, const Colorway *palette, 
                         bool is_active, bool italic_slant) {
  int digit_span_x = (DIGIT_WIDTH - 1) * DOT_SPACING_X;
  int start_y = TOP_MARGIN;
  int slant_scale = (bounds.size.w > 180) ? 7 : 5;
  int max_slant = italic_slant ? slant_scale : 0;

  int total_width = (digit_span_x * 4) + (DIGIT_GAP * 2) + COLON_GAP + max_slant;
  int start_x = (bounds.size.w - total_width) / 2;

  int d1_x = start_x;
  int d2_x = d1_x + digit_span_x + DIGIT_GAP;
  int d3_x = d2_x + digit_span_x + COLON_GAP;
  int d4_x = d3_x + digit_span_x + DIGIT_GAP;
  
  pulsar_draw_digit(ctx, d1_x, start_y, d1, palette, is_active, bounds.size.w, italic_slant);
  pulsar_draw_digit(ctx, d2_x, start_y, d2, palette, is_active, bounds.size.w, italic_slant);
  pulsar_draw_digit(ctx, d3_x, start_y, d3, palette, is_active, bounds.size.w, italic_slant);
  pulsar_draw_digit(ctx, d4_x, start_y, d4, palette, is_active, bounds.size.w, italic_slant);
  
  if (show_colon) {
    int d2_right = d2_x + digit_span_x;
    int colon_base_x = d2_right + (COLON_GAP / 2);
    pulsar_draw_colon(ctx, colon_base_x, start_y, palette, is_active, colon_lit, bounds.size.w, italic_slant);
  }
}

void pulsar_draw_5digits(GContext *ctx, GRect bounds, const int digits[5], 
                         const Colorway *palette, bool is_active, bool italic_slant) {
  int step_spacing_x = bounds.size.w > 180 ? 6 : 4;
  int step_spacing_y = bounds.size.w > 180 ? 7 : 5;
  int step_dot_radius = bounds.size.w > 180 ? 2 : 1;
  int step_gap = bounds.size.w > 180 ? 11 : 8;
  int step_digit_span = (DIGIT_WIDTH - 1) * step_spacing_x;
  int slant_scale = (bounds.size.w > 180) ? 7 : 5;
  int max_slant = italic_slant ? slant_scale : 0;

  int total_5_width = (5 * step_digit_span) + (4 * step_gap) + max_slant;
  int start_5_x = (bounds.size.w - total_5_width) / 2;
  int step_start_y = bounds.size.w > 180 ? 72 : 50;

  for (int i = 0; i < 5; i++) {
    int dx = start_5_x + (i * (step_digit_span + step_gap));
    pulsar_draw_digit_custom(ctx, dx, step_start_y, digits[i], palette, is_active, bounds.size.w,
                             step_spacing_x, step_spacing_y, step_dot_radius, italic_slant);
  }
}
