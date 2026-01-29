// ============================================================================
// TimeHelper - NTP Time Synchronization and Display
// ============================================================================
// Handles NTP time sync, formatting, and UI updates
// Non-blocking background task for time updates
// ============================================================================

#include "TimeHelper.h"
#include "ui_shell.h"
#include <WiFi.h>

// Global instance
TimeHelper timeHelper;

// Static member initialization
bool TimeHelper::_initialized = false;
bool TimeHelper::_synced = false;
String TimeHelper::_timezone = "";
const char* TimeHelper::_ntp1 = nullptr;
const char* TimeHelper::_ntp2 = nullptr;
unsigned long TimeHelper::_lastSync = 0;
TaskHandle_t TimeHelper::_taskHandle = nullptr;

bool TimeHelper::begin(const char* timezone, const char* ntp1, const char* ntp2) {
    _timezone = String(timezone);
    _ntp1 = ntp1;
    _ntp2 = ntp2;
    _initialized = false;
    _synced = false;
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("TimeHelper: WiFi not connected, deferring sync");
        _initialized = true;
        return true;  // Allow initialization, will sync when WiFi connects
    }
    
    // Configure NTP with timezone
    configTime(0, 0, _ntp1, _ntp2);
    setenv("TZ", timezone, 1);
    tzset();
    
    _initialized = true;
    
    // Wait up to 5 seconds for initial sync
    for (int i = 0; i < 10; i++) {
        if (syncNTP()) {
            _synced = true;
            _lastSync = millis();
            Serial.println("TimeHelper: NTP synchronized successfully");
            
            // Start background update task
            startUpdateTask();
            return true;
        }
        delay(500);
    }
    
    Serial.println("TimeHelper: Initial NTP sync timeout, will retry in background");
    startUpdateTask();  // Start task anyway, it will retry
    return false;
}

bool TimeHelper::syncNTP() {
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 1000)) {
        _synced = true;
        _lastSync = millis();
        return true;
    }
    return false;
}

bool TimeHelper::getFormattedTime(char* buffer, size_t size, const char* format) {
    if (!_initialized) return false;
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 1000)) {
        strftime(buffer, size, format, &timeinfo);
        return true;
    }
    return false;
}

bool TimeHelper::getFormattedDate(char* buffer, size_t size, const char* format) {
    if (!_initialized) return false;
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 1000)) {
        strftime(buffer, size, format, &timeinfo);
        return true;
    }
    return false;
}

bool TimeHelper::isTimeSynced() {
    return _synced;
}

unsigned long TimeHelper::getEpochTime() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 1000)) {
        return mktime(&timeinfo);
    }
    return 0;
}

void TimeHelper::startUpdateTask() {
    if (_taskHandle == nullptr) {
        xTaskCreatePinnedToCore(
            updateTask,
            "TimeUpdateTask",
            4096,
            nullptr,
            1,
            &_taskHandle,
            1
        );
        Serial.println("TimeHelper: Update task started");
    }
}

void TimeHelper::stopUpdateTask() {
    if (_taskHandle != nullptr) {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
        Serial.println("TimeHelper: Update task stopped");
    }
}

void TimeHelper::updateTask(void *pvParameters) {
    char timeStr[16];
    char dateStr[32];
    
    while (true) {
        // Re-sync every 5 minutes
        if (millis() - _lastSync > 300000) {
            if (timeHelper.syncNTP()) {
                Serial.println("TimeHelper: NTP re-synchronized");
            }
        }
        
        // Update UI every 10 seconds
        if (_synced) {
            if (timeHelper.getFormattedTime(timeStr, sizeof(timeStr))) {
                ui_shell_update_time(timeStr);
            }
            
            if (timeHelper.getFormattedDate(dateStr, sizeof(dateStr))) {
                ui_shell_update_date(dateStr);
            }
        } else {
            // Not synced yet, try to sync
            if (WiFi.status() == WL_CONNECTED) {
                timeHelper.syncNTP();
            }
        }
        
        // Update every 10 seconds
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
