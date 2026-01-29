#include "ui_main.h"
#include "ui_animations.h"
#include "ui_settings.h"
#include "ui_radio.h"
#include <cstdio>

// Global UI objects
lv_obj_t *ui_tabview = NULL;
lv_obj_t *ui_tab_stats = NULL;
lv_obj_t *ui_tab_radio = NULL;
lv_obj_t *ui_tab_inventory = NULL;
lv_obj_t *ui_tab_settings = NULL;

// Stats tab labels
lv_obj_t *ui_label_time = NULL;
lv_obj_t *ui_label_date = NULL;
lv_obj_t *ui_label_weather = NULL;
lv_obj_t *ui_label_temp = NULL;
lv_obj_t *ui_label_battery = NULL;

// Animation container
lv_obj_t *ui_animation_container = NULL;

// Battery bar
static lv_obj_t *ui_battery_bar = NULL;

// Apply Pip-Boy green theme
static void apply_pipboy_theme() {
    // Create a custom style for the Pip-Boy theme
    static lv_style_t style_screen;
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, PIPBOY_BLACK);
    lv_style_set_text_color(&style_screen, PIPBOY_GREEN);
    
    // Get the screen and apply the style
    lv_obj_t *screen = lv_scr_act();
    lv_obj_add_style(screen, &style_screen, 0);
}

// Create Stats tab content
static void create_stats_tab() {
    // Title label
    lv_obj_t *title = lv_label_create(ui_tab_stats);
    lv_label_set_text(title, "STAT");
    lv_obj_set_style_text_color(title, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
    
    // Time label
    ui_label_time = lv_label_create(ui_tab_stats);
    lv_label_set_text(ui_label_time, "12:00:00");
    lv_obj_set_style_text_color(ui_label_time, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_time, &lv_font_montserrat_16, 0);
    lv_obj_align(ui_label_time, LV_ALIGN_TOP_MID, 0, 30);
    
    // Date label
    ui_label_date = lv_label_create(ui_tab_stats);
    lv_label_set_text(ui_label_date, "2026-01-21");
    lv_obj_set_style_text_color(ui_label_date, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_date, &lv_font_montserrat_14, 0);
    lv_obj_align(ui_label_date, LV_ALIGN_TOP_MID, 0, 52);
    
    // Weather label
    ui_label_weather = lv_label_create(ui_tab_stats);
    lv_label_set_text(ui_label_weather, "Weather: --");
    lv_obj_set_style_text_color(ui_label_weather, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_weather, &lv_font_montserrat_12, 0);
    lv_obj_align(ui_label_weather, LV_ALIGN_TOP_LEFT, 10, 75);
    
    // Temperature label
    ui_label_temp = lv_label_create(ui_tab_stats);
    lv_label_set_text(ui_label_temp, "Temp: --°C");
    lv_obj_set_style_text_color(ui_label_temp, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_temp, &lv_font_montserrat_12, 0);
    lv_obj_align(ui_label_temp, LV_ALIGN_TOP_LEFT, 10, 92);
    
    // Battery label
    ui_label_battery = lv_label_create(ui_tab_stats);
    lv_label_set_text(ui_label_battery, "Battery:");
    lv_obj_set_style_text_color(ui_label_battery, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_battery, &lv_font_montserrat_12, 0);
    lv_obj_align(ui_label_battery, LV_ALIGN_BOTTOM_LEFT, 10, -25);
    
    // Battery bar
    ui_battery_bar = lv_bar_create(ui_tab_stats);
    lv_obj_set_size(ui_battery_bar, 150, 15);
    lv_obj_align(ui_battery_bar, LV_ALIGN_BOTTOM_LEFT, 70, -25);
    lv_bar_set_range(ui_battery_bar, 0, 100);
    lv_bar_set_value(ui_battery_bar, 50, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(ui_battery_bar, PIPBOY_DARK_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui_battery_bar, PIPBOY_GREEN, LV_PART_INDICATOR);
    
    // Animation container
    ui_animation_container = lv_obj_create(ui_tab_stats);
    lv_obj_set_size(ui_animation_container, 80, 80);
    lv_obj_align(ui_animation_container, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_bg_color(ui_animation_container, PIPBOY_BG, 0);
    lv_obj_set_style_border_color(ui_animation_container, PIPBOY_GREEN, 0);
    lv_obj_set_style_border_width(ui_animation_container, 2, 0);
    
    // Initialize animations
    ui_animations_init(ui_animation_container);
}

// Create Radio tab content
static void create_radio_tab() {
    ui_radio_init(ui_tab_radio);
}

// Create Inventory tab content
static void create_inventory_tab() {
    lv_obj_t *title = lv_label_create(ui_tab_inventory);
    lv_label_set_text(title, "ITEMS");
    lv_obj_set_style_text_color(title, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
    
    lv_obj_t *subtitle = lv_label_create(ui_tab_inventory);
    lv_label_set_text(subtitle, "[Future: SD Card File Browser]");
    lv_obj_set_style_text_color(subtitle, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
    lv_obj_align(subtitle, LV_ALIGN_CENTER, 0, 0);
    
    lv_obj_t *info = lv_label_create(ui_tab_inventory);
    lv_label_set_text(info, "Expandable for:\n- File Manager\n- Data Viewer\n- Config Editor");
    lv_obj_set_style_text_color(info, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(info, &lv_font_montserrat_14, 0);
    lv_obj_align(info, LV_ALIGN_BOTTOM_MID, 0, -10);
}

// Initialize main UI
void ui_main_init() {
    // Apply Pip-Boy theme
    apply_pipboy_theme();
    
    // Create tabview
    ui_tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 30);
    lv_obj_set_style_bg_color(ui_tabview, PIPBOY_BLACK, 0);
    lv_obj_set_style_text_color(ui_tabview, PIPBOY_GREEN, 0);
    
    // Style the tab buttons
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(ui_tabview);
    lv_obj_set_style_bg_color(tab_btns, PIPBOY_DARK_GREEN, 0);
    lv_obj_set_style_text_color(tab_btns, PIPBOY_GREEN, 0);
    lv_obj_set_style_bg_color(tab_btns, PIPBOY_GREEN, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(tab_btns, PIPBOY_BLACK, LV_PART_ITEMS | LV_STATE_CHECKED);
    
    // Add tabs
    ui_tab_stats = lv_tabview_add_tab(ui_tabview, "STAT");
    ui_tab_radio = lv_tabview_add_tab(ui_tabview, "RADIO");
    ui_tab_inventory = lv_tabview_add_tab(ui_tabview, "ITEMS");
    ui_tab_settings = lv_tabview_add_tab(ui_tabview, "DATA");
    
    // Set background color for each tab
    lv_obj_set_style_bg_color(ui_tab_stats, PIPBOY_BG, 0);
    lv_obj_set_style_bg_color(ui_tab_radio, PIPBOY_BG, 0);
    lv_obj_set_style_bg_color(ui_tab_inventory, PIPBOY_BG, 0);
    lv_obj_set_style_bg_color(ui_tab_settings, PIPBOY_BG, 0);
    
    // Create tab contents
    create_stats_tab();
    create_radio_tab();
    create_inventory_tab();
    ui_settings_init(ui_tab_settings);
    
    // Start with STATUS tab (index 0)
    lv_tabview_set_act(ui_tabview, 0, LV_ANIM_OFF);
}

// Update time display
void ui_update_time(const char *time_str) {
    if (ui_label_time != NULL) {
        lv_label_set_text(ui_label_time, time_str);
    }
}

// Update date display
void ui_update_date(const char *date_str) {
    if (ui_label_date != NULL) {
        lv_label_set_text(ui_label_date, date_str);
    }
}

// Update weather display
void ui_update_weather(const char *weather_str, const char *temp_str) {
    if (ui_label_weather != NULL) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Weather: %s", weather_str);
        lv_label_set_text(ui_label_weather, buf);
    }
    if (ui_label_temp != NULL) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Temp: %s", temp_str);
        lv_label_set_text(ui_label_temp, buf);
    }
}

// Update battery display
void ui_update_battery(int percentage) {
    if (ui_battery_bar != NULL) {
        lv_bar_set_value(ui_battery_bar, percentage, LV_ANIM_ON);
    }
}

// Switch to specific tab
void ui_switch_tab(int tab_index) {
    if (ui_tabview != NULL) {
        lv_tabview_set_act(ui_tabview, tab_index, LV_ANIM_ON);
    }
}
