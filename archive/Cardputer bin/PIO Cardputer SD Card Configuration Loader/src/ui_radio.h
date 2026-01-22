#ifndef UI_RADIO_H
#define UI_RADIO_H

#include <lvgl.h>

// Radio tab UI elements
extern lv_obj_t *ui_label_radio_status;
extern lv_obj_t *ui_label_frequency;
extern lv_obj_t *ui_label_rssi;
extern lv_obj_t *ui_label_snr;
extern lv_obj_t *ui_label_packets_rx;
extern lv_obj_t *ui_label_packets_tx;
extern lv_obj_t *ui_textarea_messages;
extern lv_obj_t *ui_btn_send_test;
extern lv_obj_t *ui_slider_frequency;
extern lv_obj_t *ui_slider_power;

// Initialize radio tab
void ui_radio_init(lv_obj_t *parent);

// Update functions
void ui_radio_update_status(bool initialized, bool receiving);
void ui_radio_update_frequency(float freq);
void ui_radio_update_rssi(float rssi);
void ui_radio_update_snr(float snr);
void ui_radio_update_stats(uint32_t rx, uint32_t tx);
void ui_radio_add_message(const char *msg, bool outgoing);
void ui_radio_clear_messages();

// Callbacks
void ui_radio_send_test_clicked(lv_event_t *e);
void ui_radio_frequency_changed(lv_event_t *e);
void ui_radio_power_changed(lv_event_t *e);

#endif // UI_RADIO_H
