#include "ui_statusbar.h"
#include "ui_theme.h"
#include <cstdio>

// Status bar elements
lv_obj_t *ui_statusbar = NULL;
lv_obj_t *ui_statusbar_time = NULL;
lv_obj_t *ui_statusbar_battery = NULL;
lv_obj_t *ui_statusbar_wifi = NULL;
lv_obj_t *ui_statusbar_lora = NULL;
lv_obj_t *ui_statusbar_battery_icon = NULL;

static bool lora_blink_state = false;
static lv_timer_t *lora_blink_timer = NULL;

// Blink callback for LoRa indicator
static void lora_blink_callback(lv_timer_t *timer) {
    if (ui_statusbar_lora) {
        lora_blink_state = !lora_blink_state;
        lv_obj_set_style_opa(ui_statusbar_lora, 
            lora_blink_state ? PIPBOY_OPA_COVER : PIPBOY_OPA_30, 0);
    }
}

void ui_statusbar_init(lv_obj_t *parent) {
    if (parent == NULL) return;
    
    // Create status bar container with CRT styling
    ui_statusbar = lv_obj_create(parent);
    lv_obj_set_size(ui_statusbar, 240, STATUSBAR_HEIGHT);
    lv_obj_align(ui_statusbar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(ui_statusbar, PIPBOY_CRT_BLACK, 0);
    lv_obj_set_style_bg_opa(ui_statusbar, PIPBOY_OPA_COVER, 0);
    lv_obj_set_style_border_color(ui_statusbar, PIPBOY_PHOSPHOR_SHADOW, 0);
    lv_obj_set_style_border_width(ui_statusbar, 1, 0);
    lv_obj_set_style_border_side(ui_statusbar, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_pad_all(ui_statusbar, 2, 0);
    lv_obj_set_style_radius(ui_statusbar, 0, 0);
    lv_obj_clear_flag(ui_statusbar, LV_OBJ_FLAG_SCROLLABLE);
    
    // Time label (left side) - larger monospace font for readability
    ui_statusbar_time = lv_label_create(ui_statusbar);
    lv_label_set_text(ui_statusbar_time, "00:00");
    lv_obj_set_style_text_color(ui_statusbar_time, PIPBOY_PHOSPHOR_BRIGHT, 0);
    lv_obj_set_style_text_font(ui_statusbar_time, &lv_font_unscii_16, 0);
    lv_obj_align(ui_statusbar_time, LV_ALIGN_LEFT_MID, 2, 0);
    
    // Battery level (right side) - larger font
    ui_statusbar_battery = lv_label_create(ui_statusbar);
    lv_label_set_text(ui_statusbar_battery, "100%");
    lv_obj_set_style_text_color(ui_statusbar_battery, PIPBOY_PHOSPHOR_BRIGHT, 0);
    lv_obj_set_style_text_font(ui_statusbar_battery, &lv_font_unscii_16, 0);
    lv_obj_align(ui_statusbar_battery, LV_ALIGN_RIGHT_MID, -2, 0);
    
    // WiFi indicator - hidden (use small font)
    ui_statusbar_wifi = lv_label_create(ui_statusbar);
    lv_label_set_text(ui_statusbar_wifi, "");
    lv_obj_set_style_text_color(ui_statusbar_wifi, PIPBOY_PHOSPHOR_DARK, 0);
    lv_obj_set_style_text_font(ui_statusbar_wifi, &lv_font_unscii_8, 0);
    lv_obj_align(ui_statusbar_wifi, LV_ALIGN_RIGHT_MID, -55, 0);
    lv_obj_add_flag(ui_statusbar_wifi, LV_OBJ_FLAG_HIDDEN);
    
    // LoRa indicator - hidden
    ui_statusbar_lora = lv_label_create(ui_statusbar);
    lv_label_set_text(ui_statusbar_lora, "");
    lv_obj_set_style_text_color(ui_statusbar_lora, PIPBOY_PHOSPHOR_DARK, 0);
    lv_obj_set_style_text_font(ui_statusbar_lora, &lv_font_unscii_8, 0);
    lv_obj_align(ui_statusbar_lora, LV_ALIGN_RIGHT_MID, -65, 0);
    lv_obj_add_flag(ui_statusbar_lora, LV_OBJ_FLAG_HIDDEN);
}

void ui_statusbar_update_time(const char *time_str) {
    if (ui_statusbar_time) {
        lv_label_set_text(ui_statusbar_time, time_str);
    }
}

void ui_statusbar_update_battery(int level, bool charging) {
    if (!ui_statusbar_battery) return;
    
    char buf[16];
    if (charging) {
        snprintf(buf, sizeof(buf), "+%d%%", level);
    } else {
        snprintf(buf, sizeof(buf), "%d%%", level);
    }
    
    lv_label_set_text(ui_statusbar_battery, buf);
    
    // Always use bright green phosphor color - no red/yellow warnings
    lv_obj_set_style_text_color(ui_statusbar_battery, PIPBOY_PHOSPHOR_BRIGHT, 0);
}

void ui_statusbar_update_wifi(bool connected, int rssi) {
    if (!ui_statusbar_wifi) return;
    
    if (connected) {
        lv_label_set_text(ui_statusbar_wifi, "W");
        
        // Color based on signal strength
        lv_color_t color;
        if (rssi > -50) {
            color = PIPBOY_GREEN_BRIGHT;
        } else if (rssi > -70) {
            color = PIPBOY_GREEN;
        } else {
            color = PIPBOY_GREEN_DARK;
        }
        
        lv_obj_set_style_text_color(ui_statusbar_wifi, color, 0);
    } else {
        lv_label_set_text(ui_statusbar_wifi, "W");
        lv_obj_set_style_text_color(ui_statusbar_wifi, PIPBOY_GREEN_DARKER, 0);
    }
}

void ui_statusbar_update_lora(bool active, float rssi) {
    if (!ui_statusbar_lora) return;
    
    if (active) {
        lv_label_set_text(ui_statusbar_lora, "L");
        
        // Color based on RSSI
        lv_color_t color;
        if (rssi > -80) {
            color = PIPBOY_GREEN_BRIGHT;
        } else if (rssi > -100) {
            color = PIPBOY_GREEN;
        } else {
            color = PIPBOY_GREEN_DARK;
        }
        
        lv_obj_set_style_text_color(ui_statusbar_lora, color, 0);
        lv_obj_set_style_opa(ui_statusbar_lora, PIPBOY_OPA_COVER, 0);
    } else {
        lv_label_set_text(ui_statusbar_lora, "L");
        lv_obj_set_style_text_color(ui_statusbar_lora, PIPBOY_GREEN_DARKER, 0);
        lv_obj_set_style_opa(ui_statusbar_lora, PIPBOY_OPA_50, 0);
    }
}

void ui_statusbar_blink_lora(bool enable) {
    if (enable) {
        if (!lora_blink_timer) {
            lora_blink_timer = lv_timer_create(lora_blink_callback, 500, NULL);
        }
    } else {
        if (lora_blink_timer) {
            lv_timer_del(lora_blink_timer);
            lora_blink_timer = NULL;
        }
        
        // Reset to solid
        if (ui_statusbar_lora) {
            lv_obj_set_style_opa(ui_statusbar_lora, PIPBOY_OPA_COVER, 0);
        }
    }
}
