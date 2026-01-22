#include "ui_tabs.h"
#include "ui_theme.h"
#include "ui_radio.h"
#include "ui_settings.h"
#include "ui_animations.h"
#include "ui_statusbar.h"

// Tab objects
lv_obj_t *ui_tabview_main = NULL;
lv_obj_t *ui_tab_stats = NULL;
lv_obj_t *ui_tab_radio = NULL;
lv_obj_t *ui_tab_inventory = NULL;
lv_obj_t *ui_tab_settings = NULL;
lv_obj_t *ui_animation_container = NULL;

// Stats tab elements
static lv_obj_t *ui_label_stats_time = NULL;
static lv_obj_t *ui_label_stats_date = NULL;
static lv_obj_t *ui_label_stats_weather = NULL;
static lv_obj_t *ui_label_stats_temp = NULL;
static lv_obj_t *ui_bar_battery = NULL;
static lv_obj_t *ui_label_battery = NULL;

static void (*tab_change_callback)(TabIndex, TabIndex) = NULL;

// Tab change event handler
static void tab_changed_event(lv_event_t *e) {
    lv_obj_t *tabview = lv_event_get_target(e);
    static TabIndex previous_tab = TAB_INDEX_STATS;
    TabIndex current_tab = (TabIndex)lv_tabview_get_tab_act(tabview);
    
    if (tab_change_callback && current_tab != previous_tab) {
        tab_change_callback(previous_tab, current_tab);
    }
    
    previous_tab = current_tab;
}

void ui_tabs_init(lv_obj_t *parent) {
    if (parent == NULL) return;
    
    // Create tabview (positioned below status bar)
    ui_tabview_main = lv_tabview_create(parent, LV_DIR_TOP, 22);
    lv_obj_set_size(ui_tabview_main, 240, 135 - STATUSBAR_HEIGHT);
    lv_obj_align(ui_tabview_main, LV_ALIGN_BOTTOM_MID, 0, 0);
    
    // Apply theme
    lv_obj_set_style_bg_color(ui_tabview_main, PIPBOY_BLACK, 0);
    lv_obj_set_style_text_color(ui_tabview_main, PIPBOY_GREEN, 0);
    
    // Style the tab buttons
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(ui_tabview_main);
    lv_obj_set_style_bg_color(tab_btns, PIPBOY_GREEN_DARK, 0);
    lv_obj_set_style_bg_opa(tab_btns, PIPBOY_OPA_70, 0);
    lv_obj_set_style_text_color(tab_btns, PIPBOY_GREEN_BRIGHT, 0);
    lv_obj_set_style_text_font(tab_btns, &lv_font_montserrat_10, 0);
    
    // Active tab style
    lv_obj_set_style_bg_color(tab_btns, PIPBOY_GREEN, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(tab_btns, PIPBOY_OPA_COVER, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(tab_btns, PIPBOY_BLACK, LV_PART_ITEMS | LV_STATE_CHECKED);
    
    // Add tabs
    ui_tab_stats = lv_tabview_add_tab(ui_tabview_main, "STAT");
    ui_tab_radio = lv_tabview_add_tab(ui_tabview_main, "RADIO");
    ui_tab_inventory = lv_tabview_add_tab(ui_tabview_main, "ITEMS");
    ui_tab_settings = lv_tabview_add_tab(ui_tabview_main, "DATA");
    
    // Set tab backgrounds
    lv_obj_set_style_bg_color(ui_tab_stats, PIPBOY_BG_DARK, 0);
    lv_obj_set_style_bg_color(ui_tab_radio, PIPBOY_BG_DARK, 0);
    lv_obj_set_style_bg_color(ui_tab_inventory, PIPBOY_BG_DARK, 0);
    lv_obj_set_style_bg_color(ui_tab_settings, PIPBOY_BG_DARK, 0);
    
    // Create tab contents
    ui_tabs_create_stats(ui_tab_stats);
    ui_tabs_create_radio(ui_tab_radio);
    ui_tabs_create_inventory(ui_tab_inventory);
    ui_tabs_create_settings(ui_tab_settings);
    
    // Add tab change event
    lv_obj_add_event_cb(ui_tabview_main, tab_changed_event, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Start with Stats tab
    lv_tabview_set_act(ui_tabview_main, TAB_INDEX_STATS, LV_ANIM_OFF);
}

void ui_tabs_create_stats(lv_obj_t *parent) {
    if (!parent) return;
    
    // Title
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "STAT");
    ui_theme_apply_label_title(title);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 2);
    
    // Time display
    ui_label_stats_time = lv_label_create(parent);
    lv_label_set_text(ui_label_stats_time, "12:00 PM");
    lv_obj_set_style_text_color(ui_label_stats_time, PIPBOY_GREEN_BRIGHT, 0);
    lv_obj_set_style_text_font(ui_label_stats_time, &lv_font_montserrat_16, 0);
    lv_obj_align(ui_label_stats_time, LV_ALIGN_TOP_MID, 0, 22);
    
    // Date display
    ui_label_stats_date = lv_label_create(parent);
    lv_label_set_text(ui_label_stats_date, "Mon, Jan 21, 2026");
    ui_theme_apply_label_normal(ui_label_stats_date);
    lv_obj_align(ui_label_stats_date, LV_ALIGN_TOP_MID, 0, 42);
    
    // Weather section
    ui_label_stats_weather = lv_label_create(parent);
    lv_label_set_text(ui_label_stats_weather, "Weather: --");
    ui_theme_apply_label_small(ui_label_stats_weather);
    lv_obj_align(ui_label_stats_weather, LV_ALIGN_TOP_LEFT, 5, 60);
    
    ui_label_stats_temp = lv_label_create(parent);
    lv_label_set_text(ui_label_stats_temp, "Temp: --°C");
    ui_theme_apply_label_small(ui_label_stats_temp);
    lv_obj_align(ui_label_stats_temp, LV_ALIGN_TOP_LEFT, 5, 73);
    
    // Battery section
    ui_label_battery = lv_label_create(parent);
    lv_label_set_text(ui_label_battery, "PWR:");
    ui_theme_apply_label_small(ui_label_battery);
    lv_obj_align(ui_label_battery, LV_ALIGN_BOTTOM_LEFT, 5, -18);
    
    ui_bar_battery = lv_bar_create(parent);
    lv_obj_set_size(ui_bar_battery, 120, 12);
    lv_obj_align(ui_bar_battery, LV_ALIGN_BOTTOM_LEFT, 35, -18);
    lv_bar_set_range(ui_bar_battery, 0, 100);
    lv_bar_set_value(ui_bar_battery, 75, LV_ANIM_OFF);
    ui_theme_apply_bar(ui_bar_battery);
    
    // Animation container (right side)
    ui_animation_container = lv_obj_create(parent);
    lv_obj_set_size(ui_animation_container, 70, 70);
    lv_obj_align(ui_animation_container, LV_ALIGN_RIGHT_MID, -5, 0);
    ui_theme_apply_container(ui_animation_container);
    lv_obj_clear_flag(ui_animation_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Initialize animation system
    ui_animations_init(ui_animation_container);
}

void ui_tabs_create_radio(lv_obj_t *parent) {
    if (!parent) return;
    
    // Use the existing ui_radio module
    ui_radio_init(parent);
}

void ui_tabs_create_inventory(lv_obj_t *parent) {
    if (!parent) return;
    
    // Title
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "ITEMS");
    ui_theme_apply_label_title(title);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 2);
    
    // Placeholder content
    lv_obj_t *subtitle = lv_label_create(parent);
    lv_label_set_text(subtitle, "[SD Card File Browser]");
    ui_theme_apply_label_normal(subtitle);
    lv_obj_align(subtitle, LV_ALIGN_CENTER, 0, -10);
    
    lv_obj_t *info = lv_label_create(parent);
    lv_label_set_text(info, "Future:\n- File Manager\n- Config Editor\n- Data Logger\n- Asset Viewer");
    ui_theme_apply_label_small(info);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, 20);
}

void ui_tabs_create_settings(lv_obj_t *parent) {
    if (!parent) return;
    
    // Use the existing ui_settings module
    ui_settings_init(parent);
}

void ui_tabs_switch_to(TabIndex index) {
    if (ui_tabview_main && index >= 0 && index < TAB_INDEX_COUNT) {
        lv_tabview_set_act(ui_tabview_main, index, LV_ANIM_ON);
    }
}

TabIndex ui_tabs_get_active() {
    if (ui_tabview_main) {
        return (TabIndex)lv_tabview_get_tab_act(ui_tabview_main);
    }
    return TAB_INDEX_STATS;
}

void ui_tabs_set_change_callback(void (*callback)(TabIndex old_tab, TabIndex new_tab)) {
    tab_change_callback = callback;
}

// Update functions accessible from main
extern "C" {
    void ui_tabs_update_time(const char *time_str) {
        if (ui_label_stats_time) {
            lv_label_set_text(ui_label_stats_time, time_str);
        }
    }
    
    void ui_tabs_update_date(const char *date_str) {
        if (ui_label_stats_date) {
            lv_label_set_text(ui_label_stats_date, date_str);
        }
    }
    
    void ui_tabs_update_weather(const char *weather_str, const char *temp_str) {
        if (ui_label_stats_weather) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Weather: %s", weather_str);
            lv_label_set_text(ui_label_stats_weather, buf);
        }
        if (ui_label_stats_temp) {
            char buf[32];
            snprintf(buf, sizeof(buf), "Temp: %s", temp_str);
            lv_label_set_text(ui_label_stats_temp, buf);
        }
    }
    
    void ui_tabs_update_battery(int percentage) {
        if (ui_bar_battery) {
            lv_bar_set_value(ui_bar_battery, percentage, LV_ANIM_ON);
        }
    }
}
