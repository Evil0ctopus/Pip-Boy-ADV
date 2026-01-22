# Cardputer ADV Binary with SD Card Configuration Loader

The .bin file is compiled specifically for **M5Stack Cardputer ADV** and loads configuration values for WiFi credentials, time zone, API key, and location from a `config.txt` file stored on the SD card.

## Cardputer ADV .bin Setup

1. Update `config.txt` with your data and copy to the root directory of your SD card (not inside any folder).
```
WIFI_SSID=YourSSID
WIFI_PASSWORD=YourPassword
TIME_ZONE=AST-3
API_KEY=YourWeatherAPIKey
LOCATION=YourCity
```

2. Use the [ESP Web Flasher](https://espressif.github.io/esptool-js/) to flash the .bin to **Cardputer ADV**. Set the flash address to `0x0` when flashing.

## Hardware Requirements
- M5Stack Cardputer ADV (ESP32-S3 StampS3)
- SD card formatted as FAT32
- Optional: LoRa SX1262 Hat for wireless communication

