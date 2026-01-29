#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <SD.h>

// ============================================================================
// Configuration Manager
// ============================================================================
// Loads and manages configuration from SD card /config.txt
// Provides getters for all configuration values
// Thread-safe and fail-safe
// ============================================================================

class Config {
public:
    // Initialize and load configuration from SD card
    static bool begin();
    
    // Load configuration from file
    static bool load(const char* filename = "/config.txt");
    
    // Save configuration to file
    static bool save(const char* filename = "/config.txt");
    
    // WiFi Configuration
    static String getWiFiSSID();
    static String getWiFiPassword();
    static void setWiFiSSID(const String& ssid);
    static void setWiFiPassword(const String& password);
    
    // Time Configuration
    static String getTimeZone();
    static void setTimeZone(const String& tz);
    
    // Weather Configuration
    static String getWeatherAPIKey();
    static String getWeatherLocation();
    static String getWeatherUnits();  // "metric" or "imperial"
    static void setWeatherAPIKey(const String& key);
    static void setWeatherLocation(const String& location);
    static void setWeatherUnits(const String& units);
    
    // Status
    static bool isLoaded();
    static String getLastError();
    
private:
    static bool _initialized;
    static bool _loaded;
    static String _lastError;
    
    // Configuration values
    static String _wifiSSID;
    static String _wifiPassword;
    static String _timeZone;
    static String _weatherAPIKey;
    static String _weatherLocation;
    static String _weatherUnits;
    
    // Helper functions
    static void parseLine(const String& line);
    static void setDefaults();
};

// Legacy global variables for backward compatibility
extern String WIFI_SSID;
extern String WIFI_PASSWORD;
extern String TIME_ZONE;
extern String API_KEY;
extern String LOCATION;

#endif // CONFIG_H
