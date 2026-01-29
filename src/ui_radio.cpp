#include "ui_radio.h"
#include "ui_main.h"
#include "LoRaHelper.h"
#include "AudioHelper.h"

// Radio tab UI elements
lv_obj_t *ui_label_radio_status = NULL;
lv_obj_t *ui_label_frequency = NULL;
lv_obj_t *ui_label_rssi = NULL;
lv_obj_t *ui_label_snr = NULL;
lv_obj_t *ui_label_packets_rx = NULL;
lv_obj_t *ui_label_packets_tx = NULL;
lv_obj_t *ui_textarea_messages = NULL;
lv_obj_t *ui_btn_send_test = NULL;
lv_obj_t *ui_slider_frequency = NULL;
lv_obj_t *ui_slider_power = NULL;

// Initialize radio tab
void ui_radio_init(lv_obj_t *parent) {
    if (parent == NULL) return;
    
    // Title
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "RADIO");
    lv_obj_set_style_text_color(title, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 5, 2);
    
    // Status label
    ui_label_radio_status = lv_label_create(parent);
    lv_label_set_text(ui_label_radio_status, "Status: Initializing...");
    lv_obj_set_style_text_color(ui_label_radio_status, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_radio_status, &lv_font_montserrat_14, 0);
    lv_obj_align(ui_label_radio_status, LV_ALIGN_TOP_LEFT, 5, 20);
    
    // Frequency display
    ui_label_frequency = lv_label_create(parent);
    lv_label_set_text(ui_label_frequency, "Freq: 915.0 MHz");
    lv_obj_set_style_text_color(ui_label_frequency, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_frequency, &lv_font_montserrat_14, 0);
    lv_obj_align(ui_label_frequency, LV_ALIGN_TOP_LEFT, 5, 35);
    
    // RSSI display
    ui_label_rssi = lv_label_create(parent);
    lv_label_set_text(ui_label_rssi, "RSSI: -- dBm");
    lv_obj_set_style_text_color(ui_label_rssi, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_rssi, &lv_font_montserrat_14, 0);
    lv_obj_align(ui_label_rssi, LV_ALIGN_TOP_RIGHT, -5, 20);
    
    // SNR display
    ui_label_snr = lv_label_create(parent);
    lv_label_set_text(ui_label_snr, "SNR: -- dB");
    lv_obj_set_style_text_color(ui_label_snr, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_snr, &lv_font_montserrat_14, 0);
    lv_obj_align(ui_label_snr, LV_ALIGN_TOP_RIGHT, -5, 35);
    
    // Packet stats
    ui_label_packets_rx = lv_label_create(parent);
    lv_label_set_text(ui_label_packets_rx, "RX: 0");
    lv_obj_set_style_text_color(ui_label_packets_rx, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_packets_rx, &lv_font_montserrat_14, 0);
    lv_obj_align(ui_label_packets_rx, LV_ALIGN_TOP_LEFT, 5, 50);
    
    ui_label_packets_tx = lv_label_create(parent);
    lv_label_set_text(ui_label_packets_tx, "TX: 0");
    lv_obj_set_style_text_color(ui_label_packets_tx, PIPBOY_GREEN, 0);
    lv_obj_set_style_text_font(ui_label_packets_tx, &lv_font_montserrat_14, 0);
    lv_obj_align(ui_label_packets_tx, LV_ALIGN_TOP_LEFT, 60, 50);
    
    // Message area
    ui_textarea_messages = lv_textarea_create(parent);
    lv_obj_set_size(ui_textarea_messages, 220, 45);
    lv_obj_align(ui_textarea_messages, LV_ALIGN_TOP_LEFT, 5, 65);
    lv_textarea_set_text(ui_textarea_messages, "");
    lv_obj_set_style_bg_color(ui_textarea_messages, PIPBOY_BG, 0);
    lv_obj_set_style_text_color(ui_textarea_messages, PIPBOY_GREEN, 0);
    lv_obj_set_style_border_color(ui_textarea_messages, PIPBOY_DARK_GREEN, 0);
    lv_textarea_set_one_line(ui_textarea_messages, false);
    
    // Send test button
    ui_btn_send_test = lv_btn_create(parent);
    lv_obj_set_size(ui_btn_send_test, 100, 25);
    lv_obj_align(ui_btn_send_test, LV_ALIGN_BOTTOM_RIGHT, -5, -2);
    lv_obj_set_style_bg_color(ui_btn_send_test, PIPBOY_DARK_GREEN, 0);
    lv_obj_add_event_cb(ui_btn_send_test, ui_radio_send_test_clicked, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *btn_label = lv_label_create(ui_btn_send_test);
    lv_label_set_text(btn_label, "Send Test");
    lv_obj_set_style_text_color(btn_label, PIPBOY_GREEN, 0);
    lv_obj_center(btn_label);
}

// Update status
void ui_radio_update_status(bool initialized, bool receiving) {
    if (ui_label_radio_status == NULL) return;
    
    char buf[64];
    if (!initialized) {
        snprintf(buf, sizeof(buf), "Status: Not initialized");
    } else if (receiving) {
        snprintf(buf, sizeof(buf), "Status: Receiving");
    } else {
        snprintf(buf, sizeof(buf), "Status: Idle");
    }
    
    lv_label_set_text(ui_label_radio_status, buf);
}

// Update frequency display
void ui_radio_update_frequency(float freq) {
    if (ui_label_frequency == NULL) return;
    
    char buf[32];
    snprintf(buf, sizeof(buf), "Freq: %.1f MHz", freq);
    lv_label_set_text(ui_label_frequency, buf);
}

// Update RSSI display
void ui_radio_update_rssi(float rssi) {
    if (ui_label_rssi == NULL) return;
    
    char buf[32];
    snprintf(buf, sizeof(buf), "RSSI: %.0f dBm", rssi);
    lv_label_set_text(ui_label_rssi, buf);
}

// Update SNR display
void ui_radio_update_snr(float snr) {
    if (ui_label_snr == NULL) return;
    
    char buf[32];
    snprintf(buf, sizeof(buf), "SNR: %.1f dB", snr);
    lv_label_set_text(ui_label_snr, buf);
}

// Update packet statistics
void ui_radio_update_stats(uint32_t rx, uint32_t tx) {
    if (ui_label_packets_rx == NULL || ui_label_packets_tx == NULL) return;
    
    char buf[32];
    snprintf(buf, sizeof(buf), "RX: %lu", rx);
    lv_label_set_text(ui_label_packets_rx, buf);
    
    snprintf(buf, sizeof(buf), "TX: %lu", tx);
    lv_label_set_text(ui_label_packets_tx, buf);
}

// Add message to display
void ui_radio_add_message(const char *msg, bool outgoing) {
    if (ui_textarea_messages == NULL) return;
    
    const char *prefix = outgoing ? "> " : "< ";
    
    // Get current text
    const char *current = lv_textarea_get_text(ui_textarea_messages);
    
    // Build new text (keep last few messages)
    char newText[512];
    snprintf(newText, sizeof(newText), "%s%s%s\n", current, prefix, msg);
    
    // Trim if too long
    if (strlen(newText) > 400) {
        const char *trimmed = newText + (strlen(newText) - 400);
        lv_textarea_set_text(ui_textarea_messages, trimmed);
    } else {
        lv_textarea_set_text(ui_textarea_messages, newText);
    }
    
    // Scroll to bottom
    lv_textarea_set_cursor_pos(ui_textarea_messages, LV_TEXTAREA_CURSOR_LAST);
}

// Clear messages
void ui_radio_clear_messages() {
    if (ui_textarea_messages == NULL) return;
    lv_textarea_set_text(ui_textarea_messages, "");
}

// Send test packet callback
void ui_radio_send_test_clicked(lv_event_t *e) {
    if (!lora.isInitialized()) {
        ui_radio_add_message("LoRa not initialized!", true);
        audio.playError();
        return;
    }
    
    // Send test packet
    const char *testMsg = "Pip-Boy ADV Test";
    
    if (lora.send(testMsg)) {
        ui_radio_add_message(testMsg, true);
        audio.playSuccess();
        
        // Update stats
        LoRaStats stats = lora.getStats();
        ui_radio_update_stats(stats.packets_received, stats.packets_sent);
    } else {
        ui_radio_add_message("Send failed!", true);
        audio.playError();
    }
}

// Frequency slider callback
void ui_radio_frequency_changed(lv_event_t *e) {
    // Future: Adjust frequency based on slider
    audio.playBeep(2000, 30);
}

// Power slider callback
void ui_radio_power_changed(lv_event_t *e) {
    // Future: Adjust TX power based on slider
    audio.playBeep(2000, 30);
}
