#ifndef UI_THEME_H
#define UI_THEME_H

#include <lvgl.h>

// Pip-Boy Green Color Palette
#define PIPBOY_GREEN_BRIGHT    lv_color_make(0x33, 0xFF, 0x33)
#define PIPBOY_GREEN           lv_color_make(0x00, 0xFF, 0x00)
#define PIPBOY_GREEN_MED       lv_color_make(0x00, 0xCC, 0x00)
#define PIPBOY_GREEN_DARK      lv_color_make(0x00, 0x66, 0x00)
#define PIPBOY_GREEN_DARKER    lv_color_make(0x00, 0x33, 0x00)
#define PIPBOY_BG_DARK         lv_color_make(0x00, 0x22, 0x00)
#define PIPBOY_BG_DARKER       lv_color_make(0x00, 0x11, 0x00)
#define PIPBOY_BLACK           lv_color_make(0x00, 0x00, 0x00)

// Transparency levels
#define PIPBOY_OPA_TRANSP      LV_OPA_TRANSP
#define PIPBOY_OPA_10          LV_OPA_10
#define PIPBOY_OPA_30          LV_OPA_30
#define PIPBOY_OPA_50          LV_OPA_50
#define PIPBOY_OPA_70          LV_OPA_70
#define PIPBOY_OPA_COVER       LV_OPA_COVER

// Theme styles
extern lv_style_t style_screen;
extern lv_style_t style_container;
extern lv_style_t style_label_title;
extern lv_style_t style_label_normal;
extern lv_style_t style_label_small;
extern lv_style_t style_button;
extern lv_style_t style_button_pressed;
extern lv_style_t style_slider;
extern lv_style_t style_bar;
extern lv_style_t style_textarea;
extern lv_style_t style_tabview;
extern lv_style_t style_tab_active;
extern lv_style_t style_tab_inactive;

// Theme initialization
void ui_theme_init();

// Apply theme to specific objects
void ui_theme_apply_screen(lv_obj_t *obj);
void ui_theme_apply_container(lv_obj_t *obj);
void ui_theme_apply_label_title(lv_obj_t *obj);
void ui_theme_apply_label_normal(lv_obj_t *obj);
void ui_theme_apply_label_small(lv_obj_t *obj);
void ui_theme_apply_button(lv_obj_t *obj);
void ui_theme_apply_slider(lv_obj_t *obj);
void ui_theme_apply_bar(lv_obj_t *obj);
void ui_theme_apply_textarea(lv_obj_t *obj);

// Scanline effect (optional CRT-like appearance)
void ui_theme_enable_scanlines(bool enable);
bool ui_theme_scanlines_enabled();

// Glow effect for text (Pip-Boy style)
void ui_theme_apply_glow(lv_obj_t *obj);

#endif // UI_THEME_H
