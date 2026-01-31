#ifndef UI_THEME_H
#define UI_THEME_H

#include <lvgl.h>
#include "version.h"  // Auto-generated version file

// Device Information
#define DEVICE_MODEL "M5Stack Cardputer ADV"

// Fallout 4 Pip-Boy 3000 Mark IV Bright Green Phosphor CRT Colors
// From: https://github.com/nishad2m8/Pip-Boy-Weather-Clock
// Authentic bright lime green terminal display with CRT styling
#define PIPBOY_PHOSPHOR_BRIGHT lv_color_make(0x33, 0xFF, 0x66)  // Bright lime green glow #33FF66
#define PIPBOY_PHOSPHOR        lv_color_make(0x00, 0xFF, 0x33)  // Main phosphor green #00FF33
#define PIPBOY_PHOSPHOR_MED    lv_color_make(0x00, 0xCC, 0x22)  // Medium intensity #00CC22
#define PIPBOY_PHOSPHOR_DARK   lv_color_make(0x00, 0x88, 0x11)  // Dark phosphor #008811
#define PIPBOY_PHOSPHOR_SHADOW lv_color_make(0x00, 0x44, 0x00)  // Shadow/outline #004400
#define PIPBOY_CRT_BG          lv_color_make(0x00, 0x22, 0x00)  // CRT background glow #002200
#define PIPBOY_CRT_BLACK       lv_color_make(0x00, 0x11, 0x00)  // Near-black with green tint #001100
#define PIPBOY_BLACK           lv_color_make(0x00, 0x00, 0x00)  // Pure black #000000

// Legacy color aliases for compatibility
#define PIPBOY_GREEN_BRIGHT    PIPBOY_PHOSPHOR_BRIGHT
#define PIPBOY_GREEN           PIPBOY_PHOSPHOR
#define PIPBOY_GREEN_MED       PIPBOY_PHOSPHOR_MED
#define PIPBOY_GREEN_DARK      PIPBOY_PHOSPHOR_DARK
#define PIPBOY_GREEN_DARKER    PIPBOY_PHOSPHOR_SHADOW
#define PIPBOY_BG_DARK         PIPBOY_CRT_BG
#define PIPBOY_BG_DARKER       PIPBOY_CRT_BLACK

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

// CRT effect styles
extern lv_style_t style_crt_scanlines;
extern lv_style_t style_crt_vignette;

// Theme initialization
void ui_theme_init();
void ui_theme_apply_crt_effects(lv_obj_t *screen);
void ui_theme_create_boot_animation(lv_obj_t *parent);

// Sound effects system
void ui_theme_init_audio();
void ui_theme_play_boot_chime();
void ui_theme_play_tab_change();
void ui_theme_play_button_press();
void ui_theme_set_sound_enabled(bool enabled);
bool ui_theme_is_sound_enabled();

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
