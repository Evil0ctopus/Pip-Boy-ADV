#ifndef TIME_HELPER_H
#define TIME_HELPER_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

// ============================================================================
// TimeHelper - NTP Time Synchronization
// ============================================================================
// Non-blocking NTP sync with background updates
// Automatically calls ui_shell_update_time() and ui_shell_update_date()
// ============================================================================

class TimeHelper {
public:
    bool begin(const char* timezone, const char* ntp1 = "pool.ntp.org", const char* ntp2 = "time.nist.gov");
    bool syncNTP();
    bool getFormattedTime(char* buffer, size_t size, const char* format = "%I:%M %p");
    bool getFormattedDate(char* buffer, size_t size, const char* format = "%a, %b %d, %Y");
    bool isTimeSynced();
    unsigned long getEpochTime();
    
    // Background task control
    void startUpdateTask();
    void stopUpdateTask();
    
private:
    static bool _initialized;
    static bool _synced;
    static String _timezone;
    static const char* _ntp1;
    static const char* _ntp2;
    static unsigned long _lastSync;
    static TaskHandle_t _taskHandle;
    
    // Background update task
    static void updateTask(void *pvParameters);
};

// Global instance
extern TimeHelper timeHelper;

#endif // TIME_HELPER_H
