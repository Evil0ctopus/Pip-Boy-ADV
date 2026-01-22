#include "ui_theme.h"

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

static bool scanlines_enabled = false;

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
    lv_style_set_text_font(&style_label_title, &lv_font_montserrat_18);
    
    // Normal label style
    lv_style_init(&style_label_normal);
    lv_style_set_text_color(&style_label_normal, PIPBOY_GREEN);
    lv_style_set_text_font(&style_label_normal, &lv_font_montserrat_12);
    
    // Small label style
    lv_style_init(&style_label_small);
    lv_style_set_text_color(&style_label_small, PIPBOY_GREEN_MED);
    lv_style_set_text_font(&style_label_small, &lv_font_montserrat_10);
    
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
        lv_obj_set_style_shadow_opa(obj, PIPBOY_OPA_30, 0);
    }
}
