#include "WeatherHelper.h"
#include "Config.h"
#include <HTTPClient.h>
#include <lvgl.h>
#include "ui_main.h"


void weatherTask(void *pvParameters) {
    Serial.println("Weather Task started");
    while (true) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Fetching weather data...");
            fetchWeatherData();  // Fetch the weather data
        }
        vTaskDelay(300000 / portTICK_PERIOD_MS);  // Delay 5 minutes between weather fetches
    }
}


void fetchWeatherData() {
    String url = String("https://api.weatherapi.com/v1/current.json?key=") + API_KEY + "&q=" + LOCATION;

    HTTPClient http;
    http.begin(url);  // Specify the URL
    int httpResponseCode = http.GET();  // Send the request

    if (httpResponseCode > 0) {
        String response = http.getString();  // Get the response payload
        Serial.println(response);  // Print the response for debugging

        // Pass the JSON data to be parsed and used in the UI
        updateWeatherUI(response);
    } else {
        Serial.printf("Error in HTTP request: %d\n", httpResponseCode);
        if (httpResponseCode == -1) {
            Serial.println("Check your WiFi connection or API key.");
        }
    }

    http.end();  // Close connection
}


// Helper function to parse JSON and update the LVGL UI elements
void updateWeatherUI(const String &jsonData) {
    // Initialize a JSON document
    DynamicJsonDocument doc(2048);

    // Parse the JSON data
    DeserializationError error = deserializeJson(doc, jsonData);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    // Extract weather data
    float temp_c = doc["current"]["temp_c"];
    const char* condition = doc["current"]["condition"]["text"];

    // Format temperature and weather strings
    char tempStr[32];
    snprintf(tempStr, sizeof(tempStr), "%d°C", (int)round(temp_c));
    
    // Update the new UI
    ui_update_weather(condition, tempStr);

    Serial.printf("Weather updated: %s, %s\n", condition, tempStr);
}
