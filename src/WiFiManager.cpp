// WiFiManager.cpp - Wi-Fi Management Implementation
#include "WiFiManager.h"
#include <WiFi.h>
#include <esp_ping.h>
#include <time.h>

WiFiManager::WiFiManager() : enabled(false), scanning(false), lastPingTime(0), timeSync(false) {}

bool WiFiManager::enable() {
    if (enabled) return true;
    
    WiFi.mode(WIFI_STA);
    delay(100);
    
    enabled = true;
    Serial.println("WiFi: Enabled");
    return true;
}

bool WiFiManager::disable() {
    if (!enabled) return true;
    
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    
    enabled = false;
    Serial.println("WiFi: Disabled");
    return true;
}

bool WiFiManager::isEnabled() {
    return enabled;
}

bool WiFiManager::startScan() {
    if (!enabled) {
        if (!enable()) return false;
    }
    
    Serial.println("WiFi: Starting scan...");
    WiFi.scanDelete();
    WiFi.scanNetworks(true); // Async scan
    
    scanning = true;
    return true;
}

bool WiFiManager::scanComplete() {
    if (!scanning) return false;
    
    int16_t result = WiFi.scanComplete();
    
    if (result == WIFI_SCAN_RUNNING) {
        return false;
    }
    
    scanning = false;
    networks.clear();
    
    if (result > 0) {
        Serial.printf("WiFi: Found %d networks\n", result);
        
        for (int i = 0; i < result && i < WIFI_SCAN_MAX; i++) {
            WiFiNetwork net;
            net.ssid = WiFi.SSID(i);
            net.rssi = WiFi.RSSI(i);
            net.encryption = WiFi.encryptionType(i);
            net.channel = WiFi.channel(i);
            
            networks.push_back(net);
        }
    }
    
    WiFi.scanDelete();
    return true;
}

std::vector<WiFiNetwork> WiFiManager::getNetworks() {
    return networks;
}

bool WiFiManager::connect(const char* ssid, const char* password) {
    if (!enabled) {
        if (!enable()) return false;
    }
    
    Serial.printf("WiFi: Connecting to %s...\n", ssid);
    
    WiFi.begin(ssid, password);
    
    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start > WIFI_CONNECT_TIMEOUT) {
            Serial.println("WiFi: Connection timeout");
            return false;
        }
        delay(100);
    }
    
    Serial.println("WiFi: Connected!");
    Serial.printf("WiFi: IP %s\n", WiFi.localIP().toString().c_str());
    
    return true;
}

bool WiFiManager::disconnect() {
    WiFi.disconnect();
    Serial.println("WiFi: Disconnected");
    return true;
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getSSID() {
    return WiFi.SSID();
}

int32_t WiFiManager::getRSSI() {
    return WiFi.RSSI();
}

String WiFiManager::getIPAddress() {
    if (!isConnected()) return "0.0.0.0";
    return WiFi.localIP().toString();
}

String WiFiManager::getMACAddress() {
    return WiFi.macAddress();
}

bool WiFiManager::ping(const char* host, uint8_t count) {
    if (!isConnected()) return false;
    
    // Simplified ping - just check DNS resolution and TCP connection
    IPAddress ip;
    if (!WiFi.hostByName(host, ip)) {
        Serial.printf("WiFi: Failed to resolve %s\n", host);
        return false;
    }
    
    uint32_t start = millis();
    
    // Simple connectivity test (actual ping would require raw sockets)
    WiFiClient client;
    bool result = client.connect(ip, 80, 1000);
    client.stop();
    
    lastPingTime = millis() - start;
    
    Serial.printf("WiFi: Ping to %s: %u ms\n", host, lastPingTime);
    return result;
}

uint32_t WiFiManager::getPingTime() {
    return lastPingTime;
}

bool WiFiManager::syncTime(const char* ntpServer) {
    if (!isConnected()) return false;
    
    Serial.printf("WiFi: Syncing time from %s...\n", ntpServer);
    
    configTime(0, 0, ntpServer);
    
    // Wait for time sync (max 5 seconds)
    uint32_t start = millis();
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) {
        if (millis() - start > 5000) {
            Serial.println("WiFi: Time sync timeout");
            return false;
        }
        delay(100);
    }
    
    timeSync = true;
    Serial.println("WiFi: Time synchronized");
    
    return true;
}

bool WiFiManager::hasTimeSync() {
    return timeSync;
}

String WiFiManager::encryptionTypeStr(wifi_auth_mode_t type) {
    switch (type) {
        case WIFI_AUTH_OPEN: return "Open";
        case WIFI_AUTH_WEP: return "WEP";
        case WIFI_AUTH_WPA_PSK: return "WPA";
        case WIFI_AUTH_WPA2_PSK: return "WPA2";
        case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
        case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-EAP";
        case WIFI_AUTH_WPA3_PSK: return "WPA3";
        default: return "Unknown";
    }
}
