// ============================================================================
// Config - Configuration Manager
// ============================================================================
// Loads configuration from SD card /config.txt
// Parses key=value pairs and provides getters
// Fail-safe with defaults
// ============================================================================

#include "Config.h"
#include <FS.h>
#include <SD.h>

// Static member initialization
bool Config::_initialized = false;
bool Config::_loaded = false;
String Config::_lastError = "";

String Config::_wifiSSID = "";
String Config::_wifiPassword = "";
String Config::_timeZone = "PST8PDT,M3.2.0,M11.1.0";  // Default: Pacific Time
String Config::_weatherAPIKey = "";
String Config::_weatherLocation = "";
String Config::_weatherUnits = "metric";  // Default: Celsius

// Legacy global variables
String WIFI_SSID = "";
String WIFI_PASSWORD = "";
String TIME_ZONE = "PST8PDT,M3.2.0,M11.1.0";
String API_KEY = "";
String LOCATION = "";

bool Config::begin() {
    if (_initialized) {
        return _loaded;
    }
    
    setDefaults();
    _initialized = true;
    
    // Try to load from SD card
    if (!load()) {
        Serial.println("Config: Using default values");
        _lastError = "Failed to load config file, using defaults";
        return false;
    }
    
    return true;
}

bool Config::load(const char* filename) {
    if (!SD.begin()) {
        _lastError = "SD card not available";
        Serial.println("Config: SD card not available");
        return false;
    }
    
    File configFile = SD.open(filename, FILE_READ);
    if (!configFile) {
        _lastError = String("Failed to open ") + filename;
        Serial.printf("Config: Failed to open %s\n", filename);
        return false;
    }
    
    Serial.printf("Config: Loading from %s...\n", filename);
    
    int lineCount = 0;
    while (configFile.available()) {
        String line = configFile.readStringUntil('\n');
        line.trim();
        
        // Skip empty lines and comments
        if (line.length() == 0 || line.startsWith("#") || line.startsWith("//")) {
            continue;
        }
        
        parseLine(line);
        lineCount++;
    }
    
    configFile.close();
    
    // Update legacy global variables
    WIFI_SSID = _wifiSSID;
    WIFI_PASSWORD = _wifiPassword;
    TIME_ZONE = _timeZone;
    API_KEY = _weatherAPIKey;
    LOCATION = _weatherLocation;
    
    _loaded = true;
    Serial.printf("Config: Loaded %d configuration lines\n", lineCount);
    Serial.println("Config: Settings loaded:");
    Serial.printf("  WiFi SSID: %s\n", _wifiSSID.c_str());
    Serial.printf("  WiFi Password: %s\n", _wifiPassword.length() > 0 ? "********" : "NOT SET");
    Serial.printf("  Time Zone: %s\n", _timeZone.c_str());
    Serial.printf("  Weather API Key: %s\n", _weatherAPIKey.length() > 0 ? "********" : "NOT SET");
    Serial.printf("  Weather Location: %s\n", _weatherLocation.c_str());
    Serial.printf("  Weather Units: %s\n", _weatherUnits.c_str());
    
    return true;
}

bool Config::save(const char* filename) {
    if (!SD.begin()) {
        _lastError = "SD card not available";
        return false;
    }
    
    File configFile = SD.open(filename, FILE_WRITE);
    if (!configFile) {
        _lastError = String("Failed to open ") + filename + " for writing";
        return false;
    }
    
    configFile.println("# Pip-Boy ADV Configuration File");
    configFile.println("# Generated automatically - Edit with care");
    configFile.println();
    
    configFile.println("# WiFi Configuration");
    configFile.printf("WIFI_SSID=%s\n", _wifiSSID.c_str());
    configFile.printf("WIFI_PASSWORD=%s\n", _wifiPassword.c_str());
    configFile.println();
    
    configFile.println("# Time Configuration");
    configFile.printf("TIME_ZONE=%s\n", _timeZone.c_str());
    configFile.println();
    
    configFile.println("# Weather Configuration");
    configFile.printf("API_KEY=%s\n", _weatherAPIKey.c_str());
    configFile.printf("LOCATION=%s\n", _weatherLocation.c_str());
    configFile.printf("UNITS=%s\n", _weatherUnits.c_str());
    
    configFile.close();
    
    Serial.printf("Config: Saved to %s\n", filename);
    return true;
}

void Config::parseLine(const String& line) {
    int separatorIndex = line.indexOf('=');
    if (separatorIndex == -1) {
        return;  // Invalid line
    }
    
    String key = line.substring(0, separatorIndex);
    String value = line.substring(separatorIndex + 1);
    
    key.trim();
    value.trim();
    
    // Remove quotes if present
    if (value.startsWith("\"") && value.endsWith("\"")) {
        value = value.substring(1, value.length() - 1);
    }
    
    // Parse known keys
    if (key.equalsIgnoreCase("WIFI_SSID")) {
        _wifiSSID = value;
    } else if (key.equalsIgnoreCase("WIFI_PASSWORD")) {
        _wifiPassword = value;
    } else if (key.equalsIgnoreCase("TIME_ZONE")) {
        _timeZone = value;
    } else if (key.equalsIgnoreCase("API_KEY")) {
        _weatherAPIKey = value;
    } else if (key.equalsIgnoreCase("LOCATION")) {
        _weatherLocation = value;
    } else if (key.equalsIgnoreCase("UNITS")) {
        _weatherUnits = value;
    }
}

void Config::setDefaults() {
    _wifiSSID = "";
    _wifiPassword = "";
    _timeZone = "PST8PDT,M3.2.0,M11.1.0";
    _weatherAPIKey = "";
    _weatherLocation = "";
    _weatherUnits = "metric";
}

// Getters
String Config::getWiFiSSID() { return _wifiSSID; }
String Config::getWiFiPassword() { return _wifiPassword; }
String Config::getTimeZone() { return _timeZone; }
String Config::getWeatherAPIKey() { return _weatherAPIKey; }
String Config::getWeatherLocation() { return _weatherLocation; }
String Config::getWeatherUnits() { return _weatherUnits; }

// Setters
void Config::setWiFiSSID(const String& ssid) { _wifiSSID = ssid; WIFI_SSID = ssid; }
void Config::setWiFiPassword(const String& password) { _wifiPassword = password; WIFI_PASSWORD = password; }
void Config::setTimeZone(const String& tz) { _timeZone = tz; TIME_ZONE = tz; }
void Config::setWeatherAPIKey(const String& key) { _weatherAPIKey = key; API_KEY = key; }
void Config::setWeatherLocation(const String& location) { _weatherLocation = location; LOCATION = location; }
void Config::setWeatherUnits(const String& units) { _weatherUnits = units; }

// Status
bool Config::isLoaded() { return _loaded; }
String Config::getLastError() { return _lastError; }
