#ifndef WEATHER_HELPER_H
#define WEATHER_HELPER_H

#if defined(DISABLE_WEATHER)
#include "WeatherHelperDisabled.h"
#else

#include <Arduino.h>
#include <ArduinoJson.h>

// ============================================================================
// WeatherHelper - Weather API Integration
// ============================================================================
// Fetches weather from WeatherAPI.com using HTTPS
// Background task updates every 10 minutes
// Automatically calls ui_shell_update_weather()
// ============================================================================

struct WeatherData {
    String condition;
    float temperature;
    float feelsLike;
    int humidity;
    float windSpeed;
    String windDirection;
    int cloudCover;
    String iconCode;
    unsigned long lastUpdate;
    bool valid;
};

class WeatherHelper {
public:
    static bool begin();
    static bool fetchWeather();
    static WeatherData getCurrentWeather();
    
    // Start/stop background update task
    static void startUpdateTask();
    static void stopUpdateTask();
    
    // Get formatted strings for UI
    static String getConditionText();
    static String getTemperatureText();
    static String getHumidityText();
    static String getWindText();
    
    static bool isDataValid();
    static unsigned long getLastUpdate();
    
    // Parse JSON response (public for legacy updateWeatherUI function)
    static bool parseWeatherJSON(const String& json);
    
private:
    static bool _initialized;
    static WeatherData _currentWeather;
    static TaskHandle_t _taskHandle;
    static unsigned long _lastFetch;
    
    static void updateTask(void *pvParameters);
};

// Legacy function for compatibility
void weatherTask(void *pvParameters);
void fetchWeatherData();
void updateWeatherUI(const String &jsonData);

#endif // DISABLE_WEATHER

#endif // WEATHER_HELPER_H
