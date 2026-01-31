#ifndef CONFIG_DISABLED_H
#define CONFIG_DISABLED_H

#include <Arduino.h>

class Config {
public:
    static bool begin() { return true; }
    static bool load(const char* = "/config.txt") { return true; }
    static bool save(const char* = "/config.txt") { return true; }
    static String getWiFiSSID() { return String(); }
    static String getWiFiPassword() { return String(); }
    static void setWiFiSSID(const String&) {}
    static void setWiFiPassword(const String&) {}
    static String getTimeZone() { return String(); }
    static void setTimeZone(const String&) {}
    static String getWeatherAPIKey() { return String(); }
    static String getWeatherLocation() { return String(); }
    static String getWeatherUnits() { return String(); }
    static void setWeatherAPIKey(const String&) {}
    static void setWeatherLocation(const String&) {}
    static void setWeatherUnits(const String&) {}
    static bool isLoaded() { return false; }
    static String getLastError() { return String("disabled"); }
};

extern String WIFI_SSID;
extern String WIFI_PASSWORD;
extern String TIME_ZONE;
extern String API_KEY;
extern String LOCATION;

#endif // CONFIG_DISABLED_H
