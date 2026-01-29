#ifndef SYSTEM_STATS_H
#define SYSTEM_STATS_H

#include <Arduino.h>
#include <M5Unified.h>

// ============================================================================
// SystemStats - System Performance Monitor
// ============================================================================
// Monitors CPU, memory, PSRAM, battery, and uptime
// Background task with 2-second update interval
// Automatically calls ui_shell_update_*() functions
// ============================================================================

class SystemStats {
public:
    void begin();
    void update();
    
    // CPU and Memory stats
    uint32_t getFreeHeap();
    uint32_t getMinFreeHeap();
    uint32_t getMaxAllocHeap();
    uint32_t getTotalHeap();
    uint32_t getPSRAMFree();
    uint32_t getPSRAMSize();
    uint8_t getCPUFreqMHz();
    
    // Task stats
    uint32_t getUptime();
    float getCPUUsage();
    uint8_t getTaskCount();
    
    // Battery stats (via M5Unified)
    int getBatteryLevel();
    bool isCharging();
    float getBatteryVoltage();
    float getBatteryCurrent();
    
    // WiFi stats
    int getWiFiRSSI();
    bool isWiFiConnected();
    
    // Background task control
    void startMonitorTask();
    void stopMonitorTask();
    
    // Print all stats to serial
    void printStats();
    
private:
    unsigned long _lastUpdate;
    uint32_t _freeHeap;
    uint32_t _minFreeHeap;
    uint32_t _psramFree;
    int _batteryLevel;
    bool _charging;
    int _wifiRSSI;
    TaskHandle_t _taskHandle;
    
    static void monitorTask(void *pvParameters);
};

// Global instance
extern SystemStats systemStats;

#endif // SYSTEM_STATS_H
