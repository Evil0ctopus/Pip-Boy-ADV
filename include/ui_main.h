#ifndef UI_MAIN_H
#define UI_MAIN_H

#include <lvgl.h>

// Pip-Boy Green Color Scheme
#define PIPBOY_GREEN lv_color_make(0x33, 0xFF, 0x33)
#define PIPBOY_DARK_GREEN lv_color_make(0x00, 0x66, 0x00)
#define PIPBOY_BG lv_color_make(0x00, 0x22, 0x00)
#define PIPBOY_BLACK lv_color_make(0x00, 0x00, 0x00)

// Global UI objects
extern lv_obj_t *ui_tabview;
extern lv_obj_t *ui_tab_stats;
extern lv_obj_t *ui_tab_radio;
extern lv_obj_t *ui_tab_inventory;
extern lv_obj_t *ui_tab_settings;

// Label objects for Stats tab
extern lv_obj_t *ui_label_time;
extern lv_obj_t *ui_label_date;
extern lv_obj_t *ui_label_weather;
extern lv_obj_t *ui_label_temp;
extern lv_obj_t *ui_label_battery;

// Animation container
extern lv_obj_t *ui_animation_container;

// Initialize main UI
void ui_main_init();

// Update functions
void ui_update_time(const char *time_str);
void ui_update_date(const char *date_str);
void ui_update_weather(const char *weather_str, const char *temp_str);
void ui_update_battery(int percentage);

// Tab switching
void ui_switch_tab(int tab_index);

#endif // UI_MAIN_H
