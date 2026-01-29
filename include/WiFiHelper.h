#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

#include <Arduino.h>
#include <WiFi.h>

// ============================================================================
// WiFiHelper - WiFi Connection Manager
// ============================================================================
// Handles WiFi connection with auto-reconnect
// Uses credentials from Config module
// Automatically calls ui_shell_update_wifi()
// ============================================================================

class WiFiHelper {
public:
    static bool begin();
    static bool connect();
    static void disconnect();
    static bool isConnected();
    static int getRSSI();
    static String getIPAddress();
    static String getSSID();
    
    // Auto-reconnect management
    static void enableAutoReconnect(bool enable = true);
    static bool isAutoReconnectEnabled();
    
    // Start/stop monitoring task
    static void startMonitorTask();
    static void stopMonitorTask();
    
private:
    static bool _initialized;
    static bool _autoReconnect;
    static unsigned long _lastConnectionAttempt;
    static TaskHandle_t _taskHandle;
    
    static void monitorTask(void *pvParameters);
};

// Legacy function for compatibility
void wifiTask(void *pvParameters);

#endif // WIFI_HELPER_H
