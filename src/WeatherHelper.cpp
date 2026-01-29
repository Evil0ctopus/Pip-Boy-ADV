// ============================================================================
// WeatherHelper - Weather API Integration
// ============================================================================
// Fetches weather from WeatherAPI.com using HTTPS
// Background task with 10-minute update interval
// ============================================================================

#include "WeatherHelper.h"
#include "Config.h"
#include "ui_shell.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// Static member initialization
bool WeatherHelper::_initialized = false;
WeatherData WeatherHelper::_currentWeather;
TaskHandle_t WeatherHelper::_taskHandle = nullptr;
unsigned long WeatherHelper::_lastFetch = 0;

bool WeatherHelper::begin() {
    _initialized = true;
    _currentWeather.condition = "";
    _currentWeather.temperature = 0.0f;
    _currentWeather.feelsLike = 0.0f;
    _currentWeather.humidity = 0;
    _currentWeather.windSpeed = 0.0f;
    _currentWeather.windDirection = "";
    _currentWeather.cloudCover = 0;
    _currentWeather.iconCode = "";
    _currentWeather.lastUpdate = 0;
    _currentWeather.valid = false;
    _lastFetch = 0;
    
    Serial.println("WeatherHelper: Initialized");
    return true;
}

bool WeatherHelper::fetchWeather() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WeatherHelper: WiFi not connected");
        return false;
    }
    
    String apiKey = Config::getWeatherAPIKey();
    String location = Config::getWeatherLocation();
    
    if (apiKey.length() == 0 || location.length() == 0) {
        Serial.println("WeatherHelper: API key or location not configured");
        return false;
    }
    
    // Build WeatherAPI.com URL
    String url = "https://api.weatherapi.com/v1/current.json?key=" + apiKey + "&q=" + location + "&aqi=no";
    
    HTTPClient http;
    http.begin(url);
    http.setTimeout(10000);  // 10 second timeout
    
    Serial.println("WeatherHelper: Fetching weather...");
    int httpCode = http.GET();
    
    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            http.end();
            
            if (parseWeatherJSON(payload)) {
                _lastFetch = millis();
                Serial.printf("WeatherHelper: Weather updated - %s, %.1f°C\n", 
                             _currentWeather.condition.c_str(), 
                             _currentWeather.temperature);
                
                // Update UI
                ui_shell_update_weather(_currentWeather.condition.c_str(), 
                                       getTemperatureText().c_str());
                return true;
            } else {
                Serial.println("WeatherHelper: Failed to parse JSON");
            }
        } else {
            Serial.printf("WeatherHelper: HTTP error: %d\n", httpCode);
        }
    } else {
        Serial.printf("WeatherHelper: Connection failed: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();
    return false;
}

bool WeatherHelper::parseWeatherJSON(const String& json) {
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, json);
    
    if (error) {
        Serial.printf("WeatherHelper: JSON parse error: %s\n", error.c_str());
        return false;
    }
    
    // Extract current weather data
    JsonObject current = doc["current"];
    if (!current) {
        Serial.println("WeatherHelper: No 'current' object in JSON");
        return false;
    }
    
    _currentWeather.condition = current["condition"]["text"].as<String>();
    _currentWeather.iconCode = current["condition"]["icon"].as<String>();
    
    // Check units preference
    if (Config::getWeatherUnits() == "imperial") {
        _currentWeather.temperature = current["temp_f"].as<float>();
        _currentWeather.feelsLike = current["feelslike_f"].as<float>();
        _currentWeather.windSpeed = current["wind_mph"].as<float>();
    } else {
        _currentWeather.temperature = current["temp_c"].as<float>();
        _currentWeather.feelsLike = current["feelslike_c"].as<float>();
        _currentWeather.windSpeed = current["wind_kph"].as<float>();
    }
    
    _currentWeather.humidity = current["humidity"].as<int>();
    _currentWeather.cloudCover = current["cloud"].as<int>();
    _currentWeather.windDirection = current["wind_dir"].as<String>();
    _currentWeather.lastUpdate = millis();
    _currentWeather.valid = true;
    
    return true;
}

WeatherData WeatherHelper::getCurrentWeather() {
    return _currentWeather;
}

String WeatherHelper::getConditionText() {
    return _currentWeather.valid ? _currentWeather.condition : "Unknown";
}

String WeatherHelper::getTemperatureText() {
    if (!_currentWeather.valid) {
        return "--°";
    }
    
    String unit = (Config::getWeatherUnits() == "imperial") ? "°F" : "°C";
    return String(_currentWeather.temperature, 1) + unit;
}

String WeatherHelper::getHumidityText() {
    if (!_currentWeather.valid) {
        return "--%";
    }
    return String(_currentWeather.humidity) + "%";
}

String WeatherHelper::getWindText() {
    if (!_currentWeather.valid) {
        return "-- km/h";
    }
    
    String unit = (Config::getWeatherUnits() == "imperial") ? " mph" : " km/h";
    return String(_currentWeather.windSpeed, 1) + unit + " " + _currentWeather.windDirection;
}

bool WeatherHelper::isDataValid() {
    return _currentWeather.valid;
}

unsigned long WeatherHelper::getLastUpdate() {
    return _lastFetch;
}

void WeatherHelper::startUpdateTask() {
    if (_taskHandle == nullptr) {
        xTaskCreatePinnedToCore(
            updateTask,
            "WeatherTask",
            8192,  // Larger stack for HTTPS
            nullptr,
            1,
            &_taskHandle,
            1
        );
        Serial.println("WeatherHelper: Update task started");
    }
}

void WeatherHelper::stopUpdateTask() {
    if (_taskHandle != nullptr) {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
        Serial.println("WeatherHelper: Update task stopped");
    }
}

void WeatherHelper::updateTask(void *pvParameters) {
    // Wait 30 seconds before first fetch to allow WiFi to stabilize
    vTaskDelay(pdMS_TO_TICKS(30000));
    
    while (true) {
        if (WiFi.status() == WL_CONNECTED) {
            fetchWeather();
        } else {
            Serial.println("WeatherHelper: Waiting for WiFi...");
        }
        
        // Update every 10 minutes
        vTaskDelay(pdMS_TO_TICKS(600000));
    }
}

// Legacy function implementations for compatibility
void weatherTask(void *pvParameters) {
    WeatherHelper::startUpdateTask();
    vTaskDelete(NULL);  // Delete this wrapper task
}

void fetchWeatherData() {
    WeatherHelper::fetchWeather();
}

void updateWeatherUI(const String &jsonData) {
    WeatherHelper::parseWeatherJSON(jsonData);
}
