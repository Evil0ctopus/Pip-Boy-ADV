// WiFiManager.h - Wi-Fi Management Module
// Network scanning, connection, and diagnostics
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#if defined(DISABLE_WIFI)
#include "WiFiManagerDisabled.h"
#else

#include <Arduino.h>
#include <WiFi.h>
#include <vector>

#define WIFI_SCAN_MAX 32
#define WIFI_CONNECT_TIMEOUT 15000

struct WiFiNetwork {
    String ssid;
    int32_t rssi;
    wifi_auth_mode_t encryption;
    uint8_t channel;
};

class WiFiManager {
public:
    WiFiManager();
    
    // Control
    bool enable();
    bool disable();
    bool isEnabled();
    
    // Scanning
    bool startScan();
    bool scanComplete();
    std::vector<WiFiNetwork> getNetworks();
    
    // Connection
    bool connect(const char* ssid, const char* password);
    bool disconnect();
    bool isConnected();
    
    // Status
    String getSSID();
    int32_t getRSSI();
    String getIPAddress();
    String getMACAddress();
    
    // Diagnostics
    bool ping(const char* host, uint8_t count = 4);
    uint32_t getPingTime();
    
    // Time sync
    bool syncTime(const char* ntpServer = "pool.ntp.org");
    bool hasTimeSync();
    
private:
    bool enabled;
    bool scanning;
    std::vector<WiFiNetwork> networks;
    uint32_t lastPingTime;
    bool timeSync;
    
    String encryptionTypeStr(wifi_auth_mode_t type);
};

#endif // DISABLE_WIFI

#endif // WIFI_MANAGER_H
