// ============================================================================
// WiFiHelper - WiFi Connection Manager
// ============================================================================
// Auto-connecting WiFi with reconnect logic
// Background monitoring task
// ============================================================================

#include "WiFiHelper.h"
#include "Config.h"
#include "ui_shell.h"
#include "LEDHelper.h"
#include <WiFi.h>

// Static member initialization
bool WiFiHelper::_initialized = false;
bool WiFiHelper::_autoReconnect = true;
unsigned long WiFiHelper::_lastConnectionAttempt = 0;
TaskHandle_t WiFiHelper::_taskHandle = nullptr;

bool WiFiHelper::begin() {
    _initialized = true;
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);  // We handle reconnection ourselves
    
    Serial.println("WiFiHelper: Initialized");
    return true;
}

bool WiFiHelper::connect() {
    String ssid = Config::getWiFiSSID();
    String password = Config::getWiFiPassword();
    
    if (ssid.length() == 0) {
        Serial.println("WiFiHelper: No SSID configured");
        ui_shell_update_wifi(false, -100);
        return false;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFiHelper: Already connected");
        return true;
    }
    
    Serial.printf("WiFiHelper: Connecting to %s...\n", ssid.c_str());
    _lastConnectionAttempt = millis();
    
    // Visual feedback via LED
    LEDHelper led;
    led.begin();
    led.setLedColor(RED);
    
    WiFi.begin(ssid.c_str(), password.c_str());
    
    // Wait up to 20 seconds for connection
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 40) {
        delay(500);
        Serial.print(".");
        attempts++;
        
        if (attempts % 4 == 0) {
            led.setLedColor((attempts % 8 == 0) ? RED : LedColor::GREEN);
        }
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\nWiFiHelper: Connected! IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("  SSID: %s\n", WiFi.SSID().c_str());
        Serial.printf("  RSSI: %d dBm\n", WiFi.RSSI());
        
        led.setLedColor(BLUE);
        ui_shell_update_wifi(true, WiFi.RSSI());
        
        return true;
    } else {
        Serial.println("\nWiFiHelper: Connection failed");
        led.setLedColor(RED);
        ui_shell_update_wifi(false, -100);
        return false;
    }
}

void WiFiHelper::disconnect() {
    WiFi.disconnect();
    Serial.println("WiFiHelper: Disconnected");
    ui_shell_update_wifi(false, -100);
}

bool WiFiHelper::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

int WiFiHelper::getRSSI() {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.RSSI();
    }
    return -100;
}

String WiFiHelper::getIPAddress() {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.localIP().toString();
    }
    return "0.0.0.0";
}

String WiFiHelper::getSSID() {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.SSID();
    }
    return "";
}

void WiFiHelper::enableAutoReconnect(bool enable) {
    _autoReconnect = enable;
    Serial.printf("WiFiHelper: Auto-reconnect %s\n", enable ? "enabled" : "disabled");
}

bool WiFiHelper::isAutoReconnectEnabled() {
    return _autoReconnect;
}

void WiFiHelper::startMonitorTask() {
    if (_taskHandle == nullptr) {
        xTaskCreatePinnedToCore(
            monitorTask,
            "WiFiMonitorTask",
            4096,
            nullptr,
            1,
            &_taskHandle,
            1
        );
        Serial.println("WiFiHelper: Monitor task started");
    }
}

void WiFiHelper::stopMonitorTask() {
    if (_taskHandle != nullptr) {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
        Serial.println("WiFiHelper: Monitor task stopped");
    }
}

void WiFiHelper::monitorTask(void *pvParameters) {
    bool wasConnected = false;
    
    while (true) {
        bool currentlyConnected = (WiFi.status() == WL_CONNECTED);
        
        // Connection state changed
        if (currentlyConnected != wasConnected) {
            if (currentlyConnected) {
                Serial.println("WiFiHelper: Connection established");
                ui_shell_update_wifi(true, WiFi.RSSI());
            } else {
                Serial.println("WiFiHelper: Connection lost");
                ui_shell_update_wifi(false, -100);
                
                // Attempt reconnection if enabled
                if (_autoReconnect) {
                    // Wait at least 10 seconds between attempts
                    if (millis() - _lastConnectionAttempt > 10000) {
                        Serial.println("WiFiHelper: Attempting reconnection...");
                        connect();
                    }
                }
            }
            wasConnected = currentlyConnected;
        }
        
        // Update RSSI if connected
        if (currentlyConnected) {
            ui_shell_update_wifi(true, WiFi.RSSI());
        }
        
        // Check every 5 seconds
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// Legacy function for compatibility
void wifiTask(void *pvParameters) {
    WiFiHelper::begin();
    WiFiHelper::connect();
    WiFiHelper::startMonitorTask();
    vTaskDelete(NULL);  // Delete this wrapper task
}
