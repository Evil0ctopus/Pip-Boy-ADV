#include "ui_shell.h"
#include "ui_theme.h"
#include "ui_statusbar.h"
#include "ui_tabs.h"
#include "ui_animations.h"

static lv_obj_t *ui_screen_main = NULL;

void ui_shell_init() {
    // Initialize theme first
    ui_theme_init();
    
    // Create main screen
    ui_screen_main = lv_obj_create(NULL);
    ui_theme_apply_screen(ui_screen_main);
    
    // Initialize status bar at top
    ui_statusbar_init(ui_screen_main);
    
    // Initialize tabview and tabs (below status bar)
    ui_tabs_init(ui_screen_main);
    
    // Load and display the screen
    lv_scr_load(ui_screen_main);
}

void ui_shell_update_time(const char *time_str) {
    ui_statusbar_update_time(time_str);
    ui_tabs_update_time(time_str);
}

void ui_shell_update_date(const char *date_str) {
    ui_tabs_update_date(date_str);
}

void ui_shell_update_weather(const char *weather, const char *temp) {
    ui_tabs_update_weather(weather, temp);
}

void ui_shell_update_battery(int percentage, bool charging) {
    ui_statusbar_update_battery(percentage, charging);
    ui_tabs_update_battery(percentage);
}

void ui_shell_update_wifi(bool connected, int rssi) {
    ui_statusbar_update_wifi(connected, rssi);
}

void ui_shell_update_lora(bool active, float rssi) {
    ui_statusbar_update_lora(active, rssi);
}

void ui_shell_play_animation(int animation_type) {
    ui_animation_play(static_cast<AnimationType>(animation_type));
}

void ui_shell_stop_animation() {
    ui_animation_stop();
}

lv_obj_t* ui_shell_get_screen() {
    return ui_screen_main;
}
