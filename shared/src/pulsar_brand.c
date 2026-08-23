#include "pulsar_brand.h"

void pulsar_draw_header(GContext *ctx, GRect bounds, const char *header_text, const Colorway *palette) {
  if (!header_text || strlen(header_text) == 0) return;
  
  GFont font_header = bounds.size.w > 180 ? 
                      fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD) : 
                      fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  int header_y = bounds.size.w > 180 ? HEADER_Y : HEADER_Y;
  graphics_context_set_text_color(ctx, palette->text_outer);
  graphics_draw_text(ctx, header_text, font_header,
                     GRect(0, header_y, bounds.size.w, bounds.size.w > 180 ? 24 : 18),
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

void pulsar_draw_footer(GContext *ctx, GRect bounds, const char *footer_text, const Colorway *palette) {
  if (!footer_text || strlen(footer_text) == 0) return;

  GFont font_footer = bounds.size.w > 180 ? 
                      fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD) : 
                      fonts_get_system_font(FONT_KEY_GOTHIC_14);
  int footer_y = bounds.size.w > 180 ? FOOTER_Y : FOOTER_Y;
  graphics_context_set_text_color(ctx, palette->text_outer);
  graphics_draw_text(ctx, footer_text, font_footer,
                     GRect(0, footer_y, bounds.size.w, bounds.size.w > 180 ? 20 : 14),
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

void pulsar_draw_annunciators(GContext *ctx, GRect bounds, const Colorway *palette, 
                             bool bt_disconnected, bool battery_low, bool is_charging) {
  int ind_y = bounds.size.w > 180 ? 20 : 12;
  int ind_margin = bounds.size.w > 180 ? 18 : 12;

  if (bt_disconnected) {
    graphics_context_set_fill_color(ctx, palette->lit);
    graphics_fill_circle(ctx, GPoint(ind_margin, ind_y), INDICATOR_RADIUS);
  }
  if (is_charging || battery_low) {
    graphics_context_set_fill_color(ctx, palette->lit);
    graphics_fill_circle(ctx, GPoint(bounds.size.w - ind_margin, ind_y), INDICATOR_RADIUS);
  }
}
