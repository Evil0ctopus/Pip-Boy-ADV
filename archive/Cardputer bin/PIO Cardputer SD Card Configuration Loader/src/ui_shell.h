#ifndef UI_SHELL_H
#define UI_SHELL_H

#include <lvgl.h>

// Main UI initialization
void ui_shell_init();

// Update functions (called from main/helper modules)
void ui_shell_update_time(const char *time_str);
void ui_shell_update_date(const char *date_str);
void ui_shell_update_weather(const char *weather, const char *temp);
void ui_shell_update_battery(int percentage, bool charging);
void ui_shell_update_wifi(bool connected, int rssi);
void ui_shell_update_lora(bool active, float rssi);

// Animation control
void ui_shell_play_animation(int animation_type);
void ui_shell_stop_animation();

// Screen management
lv_obj_t* ui_shell_get_screen();

#endif // UI_SHELL_H
