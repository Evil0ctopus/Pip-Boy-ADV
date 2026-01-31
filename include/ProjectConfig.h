#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

// Central compile-time feature flags
#ifdef DISABLE_WIFI
#include "WiFiManagerDisabled.h"
#else
#include "WiFiManager.h"
#endif

#ifdef DISABLE_CONFIG
#include "ConfigDisabled.h"
#else
#include "Config.h"
#endif

#ifdef DISABLE_WEATHER
#include "WeatherHelperDisabled.h"
#else
#include "WeatherHelper.h"
#endif

#endif // PROJECT_CONFIG_H
