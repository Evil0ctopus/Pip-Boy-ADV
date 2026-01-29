#include "SystemStats.h"
#include "ui_shell.h"

SystemStats systemStats;

void SystemStats::begin() {
    _lastUpdate = 0;
    _freeHeap = 0;
    _minFreeHeap = 0;
    _psramFree = 0;
    _batteryLevel = 0;
    _charging = false;
    _wifiRSSI = -100;
    _taskHandle = nullptr;
    
    update();
    Serial.println("SystemStats: Initialized");
}

void SystemStats::update() {
    _lastUpdate = millis();
    
    // Update memory stats
    _freeHeap = ESP.getFreeHeap();
    _minFreeHeap = ESP.getMinFreeHeap();
    _psramFree = ESP.getFreePsram();
    
    // Update battery stats via M5Unified
    _batteryLevel = M5.Power.getBatteryLevel();
    _charging = M5.Power.isCharging();
    
    // WiFi stats removed for standalone operation
    _wifiRSSI = -100;
}

uint32_t SystemStats::getFreeHeap() {
    return _freeHeap;
}

uint32_t SystemStats::getMinFreeHeap() {
    return _minFreeHeap;
}

uint32_t SystemStats::getMaxAllocHeap() {
    return ESP.getMaxAllocHeap();
}

uint32_t SystemStats::getTotalHeap() {
    return ESP.getHeapSize();
}

uint32_t SystemStats::getPSRAMFree() {
    return _psramFree;
}

uint32_t SystemStats::getPSRAMSize() {
    return ESP.getPsramSize();
}

uint8_t SystemStats::getCPUFreqMHz() {
    return ESP.getCpuFreqMHz();
}

uint32_t SystemStats::getUptime() {
    return millis() / 1000;
}

float SystemStats::getCPUUsage() {
    // Simplified CPU usage estimation
    return 0.0;
}

uint8_t SystemStats::getTaskCount() {
    return uxTaskGetNumberOfTasks();
}

int SystemStats::getBatteryLevel() {
    return _batteryLevel;
}

bool SystemStats::isCharging() {
    return _charging;
}

float SystemStats::getBatteryVoltage() {
    return M5.Power.getBatteryVoltage() / 1000.0;  // Convert mV to V
}

float SystemStats::getBatteryCurrent() {
    return M5.Power.getBatteryCurrent() / 1000.0;  // Convert mA to A
}

int SystemStats::getWiFiRSSI() {
    return -100;  // Always disconnected in standalone mode
}

bool SystemStats::isWiFiConnected() {
    return false;  // Always disconnected in standalone mode
}

void SystemStats::startMonitorTask() {
    if (_taskHandle == nullptr) {
        xTaskCreatePinnedToCore(
            monitorTask,
            "SystemStatsTask",
            4096,
            nullptr,
            1,
            &_taskHandle,
            1
        );
        Serial.println("SystemStats: Monitor task started");
    }
}

void SystemStats::stopMonitorTask() {
    if (_taskHandle != nullptr) {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
        Serial.println("SystemStats: Monitor task stopped");
    }
}

void SystemStats::monitorTask(void *pvParameters) {
    while (true) {
        // Update stats
        systemStats.update();
        
        // Update UI
        ui_shell_update_cpu(systemStats.getCPUFreqMHz());
        ui_shell_update_memory(systemStats.getFreeHeap(), systemStats.getTotalHeap());
        ui_shell_update_psram(systemStats.getPSRAMFree(), systemStats.getPSRAMSize());
        ui_shell_update_uptime(systemStats.getUptime());
        ui_shell_update_battery(systemStats.getBatteryLevel(), systemStats.isCharging());
        
        // Update every 2 seconds
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void SystemStats::printStats() {
    Serial.println("\n=== System Statistics ===");
    Serial.printf("Uptime: %lu seconds\n", getUptime());
    Serial.printf("CPU Frequency: %d MHz\n", getCPUFreqMHz());
    Serial.printf("Free Heap: %lu bytes\n", _freeHeap);
    Serial.printf("Min Free Heap: %lu bytes\n", _minFreeHeap);
    Serial.printf("Max Alloc Heap: %lu bytes\n", getMaxAllocHeap());
    Serial.printf("Total Heap: %lu bytes\n", getTotalHeap());
    Serial.printf("PSRAM Free: %lu bytes\n", _psramFree);
    Serial.printf("PSRAM Total: %lu bytes\n", getPSRAMSize());
    Serial.printf("Battery: %d%% (%s)\n", _batteryLevel, _charging ? "Charging" : "Discharging");
    Serial.printf("Battery Voltage: %.2f V\n", getBatteryVoltage());
    Serial.printf("Battery Current: %.2f A\n", getBatteryCurrent());
    Serial.printf("WiFi RSSI: %d dBm\n", _wifiRSSI);
    Serial.printf("FreeRTOS Tasks: %d\n", getTaskCount());
    Serial.println("========================\n");
}
