#ifndef UI_TABS_H
#define UI_TABS_H

#include <lvgl.h>

// Tab indices
enum TabIndex {
    TAB_INDEX_STATS = 0,
    TAB_INDEX_RADIO,
    TAB_INDEX_INVENTORY,
    TAB_INDEX_SETTINGS,
    TAB_INDEX_COUNT
};

// Tab objects
extern lv_obj_t *ui_tabview_main;
extern lv_obj_t *ui_tab_stats;
extern lv_obj_t *ui_tab_radio;
extern lv_obj_t *ui_tab_inventory;
extern lv_obj_t *ui_tab_settings;

// Animation container (in Stats tab)
extern lv_obj_t *ui_animation_container;

// Initialize tabview and all tabs
void ui_tabs_init(lv_obj_t *parent);

// Tab content creation
void ui_tabs_create_stats(lv_obj_t *parent);
void ui_tabs_create_radio(lv_obj_t *parent);
void ui_tabs_create_inventory(lv_obj_t *parent);
void ui_tabs_create_settings(lv_obj_t *parent);

// Tab switching
void ui_tabs_switch_to(TabIndex index);
TabIndex ui_tabs_get_active();

// Tab change callback
void ui_tabs_set_change_callback(void (*callback)(TabIndex old_tab, TabIndex new_tab));

#endif // UI_TABS_H
