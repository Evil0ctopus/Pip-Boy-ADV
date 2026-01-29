#include "ui_theme.h"
#include "AudioHelper.h"
#include <Arduino.h>
#include <M5Unified.h>

// Global theme styles
lv_style_t style_screen;
lv_style_t style_container;
lv_style_t style_label_title;
lv_style_t style_label_normal;
lv_style_t style_label_small;
lv_style_t style_button;
lv_style_t style_button_pressed;
lv_style_t style_slider;
lv_style_t style_bar;
lv_style_t style_textarea;
lv_style_t style_tabview;
lv_style_t style_tab_active;
lv_style_t style_tab_inactive;
lv_style_t style_crt_scanlines;
lv_style_t style_crt_vignette;

static bool scanlines_enabled = true;
static bool sound_enabled = true;
static lv_obj_t *scanline_overlay = NULL;
static lv_obj_t *boot_screen = NULL;

void ui_theme_init() {
    // Screen style
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, PIPBOY_BLACK);
    lv_style_set_bg_opa(&style_screen, PIPBOY_OPA_COVER);
    lv_style_set_text_color(&style_screen, PIPBOY_GREEN_BRIGHT);
    lv_style_set_border_width(&style_screen, 0);
    lv_style_set_pad_all(&style_screen, 0);
    
    // Container style
    lv_style_init(&style_container);
    lv_style_set_bg_color(&style_container, PIPBOY_BG_DARK);
    lv_style_set_bg_opa(&style_container, PIPBOY_OPA_COVER);
    lv_style_set_border_color(&style_container, PIPBOY_GREEN_DARK);
    lv_style_set_border_width(&style_container, 1);
    lv_style_set_border_opa(&style_container, PIPBOY_OPA_50);
    lv_style_set_pad_all(&style_container, 4);
    lv_style_set_radius(&style_container, 2);
    
    // Title label style
    lv_style_init(&style_label_title);
    lv_style_set_text_color(&style_label_title, PIPBOY_GREEN_BRIGHT);
    lv_style_set_text_font(&style_label_title, &lv_font_montserrat_16);
    
    // Normal label style
    lv_style_init(&style_label_normal);
    lv_style_set_text_color(&style_label_normal, PIPBOY_GREEN);
    lv_style_set_text_font(&style_label_normal, &lv_font_montserrat_12);
    
    // Small label style
    lv_style_init(&style_label_small);
    lv_style_set_text_color(&style_label_small, PIPBOY_GREEN_MED);
    lv_style_set_text_font(&style_label_small, &lv_font_montserrat_14);
    
    // Button style
    lv_style_init(&style_button);
    lv_style_set_bg_color(&style_button, PIPBOY_GREEN_DARK);
    lv_style_set_bg_opa(&style_button, PIPBOY_OPA_70);
    lv_style_set_border_color(&style_button, PIPBOY_GREEN);
    lv_style_set_border_width(&style_button, 1);
    lv_style_set_text_color(&style_button, PIPBOY_GREEN_BRIGHT);
    lv_style_set_radius(&style_button, 4);
    lv_style_set_pad_all(&style_button, 6);
    
    // Button pressed style
    lv_style_init(&style_button_pressed);
    lv_style_set_bg_color(&style_button_pressed, PIPBOY_GREEN);
    lv_style_set_bg_opa(&style_button_pressed, PIPBOY_OPA_COVER);
    lv_style_set_text_color(&style_button_pressed, PIPBOY_BLACK);
    
    // Slider style
    lv_style_init(&style_slider);
    lv_style_set_bg_color(&style_slider, PIPBOY_GREEN_DARKER);
    lv_style_set_bg_opa(&style_slider, PIPBOY_OPA_COVER);
    lv_style_set_border_color(&style_slider, PIPBOY_GREEN_DARK);
    lv_style_set_border_width(&style_slider, 1);
    
    // Bar style
    lv_style_init(&style_bar);
    lv_style_set_bg_color(&style_bar, PIPBOY_GREEN_DARKER);
    lv_style_set_bg_opa(&style_bar, PIPBOY_OPA_COVER);
    lv_style_set_border_color(&style_bar, PIPBOY_GREEN_DARK);
    lv_style_set_border_width(&style_bar, 1);
    lv_style_set_radius(&style_bar, 2);
    
    // Textarea style
    lv_style_init(&style_textarea);
    lv_style_set_bg_color(&style_textarea, PIPBOY_BG_DARKER);
    lv_style_set_bg_opa(&style_textarea, PIPBOY_OPA_COVER);
    lv_style_set_border_color(&style_textarea, PIPBOY_GREEN_DARK);
    lv_style_set_border_width(&style_textarea, 1);
    lv_style_set_text_color(&style_textarea, PIPBOY_GREEN);
    lv_style_set_pad_all(&style_textarea, 4);
    lv_style_set_radius(&style_textarea, 2);
    
    // Tabview style
    lv_style_init(&style_tabview);
    lv_style_set_bg_color(&style_tabview, PIPBOY_BLACK);
    lv_style_set_bg_opa(&style_tabview, PIPBOY_OPA_COVER);
    lv_style_set_border_width(&style_tabview, 0);
    lv_style_set_text_color(&style_tabview, PIPBOY_GREEN);
    
    // Active tab style
    lv_style_init(&style_tab_active);
    lv_style_set_bg_color(&style_tab_active, PIPBOY_GREEN);
    lv_style_set_bg_opa(&style_tab_active, PIPBOY_OPA_COVER);
    lv_style_set_text_color(&style_tab_active, PIPBOY_BLACK);
    lv_style_set_border_width(&style_tab_active, 0);
    
    // Inactive tab style
    lv_style_init(&style_tab_inactive);
    lv_style_set_bg_color(&style_tab_inactive, PIPBOY_GREEN_DARK);
    lv_style_set_bg_opa(&style_tab_inactive, PIPBOY_OPA_50);
    lv_style_set_text_color(&style_tab_inactive, PIPBOY_GREEN_BRIGHT);
    lv_style_set_border_width(&style_tab_inactive, 0);
    
    // CRT Scanlines style
    lv_style_init(&style_crt_scanlines);
    lv_style_set_bg_opa(&style_crt_scanlines, LV_OPA_10);
    lv_style_set_bg_color(&style_crt_scanlines, lv_color_black());
    
    // CRT Vignette style (darker edges)
    lv_style_init(&style_crt_vignette);
    lv_style_set_bg_opa(&style_crt_vignette, LV_OPA_20);
    lv_style_set_bg_color(&style_crt_vignette, lv_color_black());
    
    Serial.println("UI Theme: Initialized (Pip-Boy Green CRT)");
}

void ui_theme_apply_screen(lv_obj_t *obj) {
    if (obj) lv_obj_add_style(obj, &style_screen, 0);
}

void ui_theme_apply_container(lv_obj_t *obj) {
    if (obj) lv_obj_add_style(obj, &style_container, 0);
}

void ui_theme_apply_label_title(lv_obj_t *obj) {
    if (obj) lv_obj_add_style(obj, &style_label_title, 0);
}

void ui_theme_apply_label_normal(lv_obj_t *obj) {
    if (obj) lv_obj_add_style(obj, &style_label_normal, 0);
}

void ui_theme_apply_label_small(lv_obj_t *obj) {
    if (obj) lv_obj_add_style(obj, &style_label_small, 0);
}

void ui_theme_apply_button(lv_obj_t *obj) {
    if (obj) {
        lv_obj_add_style(obj, &style_button, 0);
        lv_obj_add_style(obj, &style_button_pressed, LV_STATE_PRESSED);
    }
}

void ui_theme_apply_slider(lv_obj_t *obj) {
    if (obj) {
        lv_obj_add_style(obj, &style_slider, LV_PART_MAIN);
        lv_obj_set_style_bg_color(obj, PIPBOY_GREEN, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(obj, PIPBOY_GREEN_BRIGHT, LV_PART_KNOB);
    }
}

void ui_theme_apply_bar(lv_obj_t *obj) {
    if (obj) {
        lv_obj_add_style(obj, &style_bar, LV_PART_MAIN);
        lv_obj_set_style_bg_color(obj, PIPBOY_GREEN, LV_PART_INDICATOR);
    }
}

void ui_theme_apply_textarea(lv_obj_t *obj) {
    if (obj) lv_obj_add_style(obj, &style_textarea, 0);
}

void ui_theme_enable_scanlines(bool enable) {
    scanlines_enabled = enable;
    // Scanline effect implementation would require custom drawing
    // This is a placeholder for future enhancement
}

bool ui_theme_scanlines_enabled() {
    return scanlines_enabled;
}

void ui_theme_apply_glow(lv_obj_t *obj) {
    if (obj) {
        // Create a subtle glow effect by adding shadow
        lv_obj_set_style_shadow_color(obj, PIPBOY_GREEN, 0);
        lv_obj_set_style_shadow_width(obj, 4, 0);
        lv_obj_set_style_shadow_spread(obj, 2, 0);
        lv_obj_set_style_shadow_opa(obj, LV_OPA_30, 0);
    }
}

// ============================================================================
// Sound Effects System
// ============================================================================

void ui_theme_init_audio() {
    // Audio is managed by AudioHelper
    Serial.println("UI Theme: Audio system ready");
}

void ui_theme_play_boot_chime() {
    if (!sound_enabled) return;
    // Boot chime: 800Hz for 150ms, then 1200Hz for 150ms
    M5.Speaker.tone(800, 150);
    delay(150);
    M5.Speaker.tone(1200, 150);
}

void ui_theme_play_tab_change() {
    if (!sound_enabled) return;
    // Tab change: Short 1000Hz beep
    M5.Speaker.tone(1000, 50);
}

void ui_theme_play_button_press() {
    if (!sound_enabled) return;
    // Button press: Short 1500Hz beep
    M5.Speaker.tone(1500, 30);
}

void ui_theme_set_sound_enabled(bool enabled) {
    sound_enabled = enabled;
    Serial.printf("UI Theme: Sound %s\n", enabled ? "enabled" : "disabled");
}

bool ui_theme_is_sound_enabled() {
    return sound_enabled;
}

// ============================================================================
// CRT Effects - Scanlines and Vignette
// ============================================================================

void ui_theme_apply_crt_effects(lv_obj_t *screen) {
    if (!screen) return;
    
    // Create vignette overlay (darker edges for CRT effect)
    lv_obj_t *vignette = lv_obj_create(screen);
    lv_obj_set_size(vignette, 240, 135);
    lv_obj_set_pos(vignette, 0, 0);
    lv_obj_add_style(vignette, &style_crt_vignette, 0);
    lv_obj_clear_flag(vignette, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(vignette, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_move_foreground(vignette);
}

// ============================================================================
// Boot Animation - Enhanced with flicker and initialization text
// ============================================================================

static lv_obj_t *init_label = NULL;
static lv_timer_t *flicker_timer = NULL;
static uint8_t flicker_count = 0;

static void boot_flicker_cb(lv_timer_t *timer) {
    if (!boot_screen) {
        lv_timer_del(timer);
        flicker_timer = NULL;
        return;
    }
    
    // Flicker effect - randomly change opacity
    flicker_count++;
    if (flicker_count < 6) {
        lv_opa_t opa = (flicker_count % 2 == 0) ? LV_OPA_COVER : LV_OPA_70;
        lv_obj_set_style_bg_opa(boot_screen, opa, 0);
    } else {
        // Stop flicker, show initialization text
        lv_obj_set_style_bg_opa(boot_screen, LV_OPA_COVER, 0);
        if (init_label && flicker_count == 6) {
            lv_label_set_text(init_label, "Vault-Tec Systems Initializing...");
        }
    }
}

static void boot_anim_complete_cb(lv_anim_t *a) {
    if (boot_screen) {
        if (flicker_timer) {
            lv_timer_del(flicker_timer);
            flicker_timer = NULL;
        }
        lv_obj_del(boot_screen);
        boot_screen = NULL;
        init_label = NULL;
        flicker_count = 0;
        Serial.println("Boot animation complete");
    }
}

void ui_theme_create_boot_animation(lv_obj_t *parent) {
    if (!parent) return;
    
    // Create boot screen overlay
    boot_screen = lv_obj_create(parent);
    lv_obj_set_size(boot_screen, 240, 135);
    lv_obj_set_pos(boot_screen, 0, 0);
    lv_obj_set_style_bg_color(boot_screen, PIPBOY_BLACK, 0);
    lv_obj_set_style_bg_opa(boot_screen, LV_OPA_COVER, 0);
    lv_obj_clear_flag(boot_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_move_foreground(boot_screen);
    
    // Boot title with glow
    lv_obj_t *title = lv_label_create(boot_screen);
    lv_label_set_text(title, "PIP-BOY");
    lv_obj_set_style_text_color(title, PIPBOY_GREEN_BRIGHT, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -25);
    ui_theme_apply_glow(title);
    
    // Subtitle
    lv_obj_t *subtitle = lv_label_create(boot_screen);
    lv_label_set_text(subtitle, "ADVANCED");
    lv_obj_set_style_text_color(subtitle, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_14, 0);
    lv_obj_align(subtitle, LV_ALIGN_CENTER, 0, 0);
    
    // Version
    lv_obj_t *version = lv_label_create(boot_screen);
    lv_label_set_text(version, "v" FIRMWARE_VERSION);
    lv_obj_set_style_text_color(version, PIPBOY_GREEN_MED, 0);
    lv_obj_set_style_text_font(version, &lv_font_montserrat_12, 0);
    lv_obj_align(version, LV_ALIGN_CENTER, 0, 20);
    
    // Initialization text (initially empty)
    init_label = lv_label_create(boot_screen);
    lv_label_set_text(init_label, "");
    lv_obj_set_style_text_color(init_label, PIPBOY_GREEN_MED, 0);
    lv_obj_set_style_text_font(init_label, &lv_font_montserrat_12, 0);
    lv_obj_align(init_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    // Start flicker effect (100ms intervals, 6 times)
    flicker_count = 0;
    flicker_timer = lv_timer_create(boot_flicker_cb, 100, NULL);
    
    // Play boot chime
    ui_theme_play_boot_chime();
    
    // Fade out animation (1.5 second delay, then 1.5 second fade)
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, boot_screen);
    lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_time(&a, 1500);
    lv_anim_set_delay(&a, 1500);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_bg_opa);
    lv_anim_set_ready_cb(&a, boot_anim_complete_cb);
    lv_anim_start(&a);
    
    Serial.println("Boot animation started (enhanced)");
}

