#ifndef WIFI_MANAGER_DISABLED_H
#define WIFI_MANAGER_DISABLED_H

#include <Arduino.h>
#include <vector>

struct WiFiNetwork {
    String ssid;
    int32_t rssi = 0;
    uint8_t encryption = 0;
    uint8_t channel = 0;
};

class WiFiManager {
public:
    WiFiManager() = default;
    bool enable() { return false; }
    bool disable() { return true; }
    bool isEnabled() { return false; }
    bool startScan() { return false; }
    bool scanComplete() { return true; }
    std::vector<WiFiNetwork> getNetworks() { return {}; }
    bool connect(const char* , const char* ) { return false; }
    bool disconnect() { return true; }
    bool isConnected() { return false; }
    String getSSID() { return String(); }
    int32_t getRSSI() { return -100; }
    String getIPAddress() { return String("0.0.0.0"); }
    String getMACAddress() { return String(); }
    bool ping(const char* , uint8_t = 4) { return false; }
    uint32_t getPingTime() { return 0; }
    bool syncTime(const char* = "pool.ntp.org") { return false; }
    bool hasTimeSync() { return false; }
};

#endif // WIFI_MANAGER_DISABLED_H
