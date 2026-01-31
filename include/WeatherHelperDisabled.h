#ifndef WEATHER_HELPER_DISABLED_H
#define WEATHER_HELPER_DISABLED_H

#include <Arduino.h>

struct WeatherData {
    String condition;
    float temperature;
    float humidity;
    float windSpeed;
    String location;
};

class WeatherHelper {
public:
    static bool fetchWeather() { return false; }
    static WeatherData getCurrentWeather() { return {}; }
};

inline void weatherTask(void *) {}
inline void fetchWeatherData() {}
inline void updateWeatherUI(const String &) {}

#endif // WEATHER_HELPER_DISABLED_H
