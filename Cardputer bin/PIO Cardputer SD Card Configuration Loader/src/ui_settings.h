#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <lvgl.h>

// Settings UI elements
extern lv_obj_t *ui_slider_brightness;
extern lv_obj_t *ui_label_brightness_value;
extern lv_obj_t *ui_btn_wifi_config;
extern lv_obj_t *ui_btn_about;
extern lv_obj_t *ui_label_wifi_status;

// Initialize settings tab
void ui_settings_init(lv_obj_t *parent);

// Settings callbacks
void ui_settings_brightness_changed(lv_event_t *e);
void ui_settings_wifi_config_clicked(lv_event_t *e);
void ui_settings_about_clicked(lv_event_t *e);

// Update settings display
void ui_settings_update_brightness(int value);
void ui_settings_update_wifi_status(bool connected, const char *ssid);

#endif // UI_SETTINGS_H
