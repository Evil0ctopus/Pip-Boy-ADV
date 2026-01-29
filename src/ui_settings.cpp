#include "ui_settings.h"
#include "ui_main.h"
#include <M5Unified.h>

// Settings UI elements
lv_obj_t *ui_slider_brightness = NULL;
lv_obj_t *ui_label_brightness_value = NULL;
lv_obj_t *ui_btn_wifi_config = NULL;
lv_obj_t *ui_btn_about = NULL;
lv_obj_t *ui_label_wifi_status = NULL;

// External reference to current brightness
extern int currentBrightness;

// Brightness slider callback
void ui_settings_brightness_changed(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    int value = lv_slider_get_value(slider);
    
    // Update brightness value label
    if (ui_label_brightness_value != NULL) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", value);
        lv_label_set_text(ui_label_brightness_value, buf);
    }
    
    // Update actual screen brightness
    currentBrightness = value;
    M5.Display.setBrightness(value);
}

// WiFi config button callback
void ui_settings_wifi_config_clicked(lv_event_t *e) {
    // Future: Open WiFi configuration dialog
    // For now, just a placeholder
}

// About button callback
void ui_settings_about_clicked(lv_event_t *e) {
    // Future: Show about/info screen
    // For now, just a placeholder
}

// Initialize settings tab
void ui_settings_init(lv_obj_t *parent) {
    if (parent == NULL) return;
    
    // Title
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "DATA");
    lv_obj_set_style_text_color(title, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
    
    // Brightness label
    lv_obj_t *brightness_label = lv_label_create(parent);
    lv_label_set_text(brightness_label, "Brightness:");
    lv_obj_set_style_text_color(brightness_label, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(brightness_label, &lv_font_montserrat_12, 0);
    lv_obj_align(brightness_label, LV_ALIGN_TOP_LEFT, 10, 35);
    
    // Brightness value label
    ui_label_brightness_value = lv_label_create(parent);
    lv_label_set_text(ui_label_brightness_value, "128");
    lv_obj_set_style_text_color(ui_label_brightness_value, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_brightness_value, &lv_font_montserrat_12, 0);
    lv_obj_align(ui_label_brightness_value, LV_ALIGN_TOP_RIGHT, -10, 35);
    
    // Brightness slider
    ui_slider_brightness = lv_slider_create(parent);
    lv_obj_set_width(ui_slider_brightness, 200);
    lv_obj_align(ui_slider_brightness, LV_ALIGN_TOP_MID, 0, 55);
    lv_slider_set_range(ui_slider_brightness, 0, 255);
    lv_slider_set_value(ui_slider_brightness, currentBrightness, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(ui_slider_brightness, PIPBOY_DARK_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_slider_brightness, PIPBOY_GREEN, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(ui_slider_brightness, PIPBOY_GREEN, LV_PART_KNOB);
    lv_obj_add_event_cb(ui_slider_brightness, ui_settings_brightness_changed, LV_EVENT_VALUE_CHANGED, NULL);
    
    // WiFi status label
    ui_label_wifi_status = lv_label_create(parent);
    lv_label_set_text(ui_label_wifi_status, "WiFi: Disconnected");
    lv_obj_set_style_text_color(ui_label_wifi_status, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_wifi_status, &lv_font_montserrat_12, 0);
    lv_obj_align(ui_label_wifi_status, LV_ALIGN_TOP_LEFT, 10, 80);
    
    // System info
    lv_obj_t *sys_info = lv_label_create(parent);
    lv_label_set_text(sys_info, "System Info:");
    lv_obj_set_style_text_color(sys_info, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(sys_info, &lv_font_montserrat_12, 0);
    lv_obj_align(sys_info, LV_ALIGN_TOP_LEFT, 10, 100);
    
    lv_obj_t *device_info = lv_label_create(parent);
    lv_label_set_text(device_info, "M5Stack Cardputer ADV\nESP32-S3\n240x135 Display");
    lv_obj_set_style_text_color(device_info, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(device_info, &lv_font_montserrat_14, 0);
    lv_obj_align(device_info, LV_ALIGN_TOP_LEFT, 10, 115);
}

// Update brightness display
void ui_settings_update_brightness(int value) {
    if (ui_slider_brightness != NULL) {
        lv_slider_set_value(ui_slider_brightness, value, LV_ANIM_ON);
    }
    if (ui_label_brightness_value != NULL) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", value);
        lv_label_set_text(ui_label_brightness_value, buf);
    }
}

// Update WiFi status display
void ui_settings_update_wifi_status(bool connected, const char *ssid) {
    if (ui_label_wifi_status != NULL) {
        char buf[64];
        if (connected && ssid != NULL) {
            snprintf(buf, sizeof(buf), "WiFi: %s", ssid);
        } else {
            snprintf(buf, sizeof(buf), "WiFi: Disconnected");
        }
        lv_label_set_text(ui_label_wifi_status, buf);
    }
}
