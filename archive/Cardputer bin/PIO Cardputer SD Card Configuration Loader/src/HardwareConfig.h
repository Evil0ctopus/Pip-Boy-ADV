#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

// ============================================================================
// M5Stack Cardputer ADV Hardware Pin Definitions
// ============================================================================
// This file contains all hardware-specific pin definitions and I2C addresses
// for the M5Stack Cardputer ADV (ESP32-S3)
// ============================================================================

// I2C Bus Configuration
#define I2C_SDA_PIN 12
#define I2C_SCL_PIN 11
#define I2C_FREQ 400000  // 400kHz

// I2C Device Addresses
#define TCA8418_ADDR 0x34    // Keyboard controller
#define QMI8658_ADDR 0x6B    // IMU sensor (corrected from 0x18)
#define AXP2101_ADDR 0x34    // PMIC (Power Management IC)

// Display Configuration
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 135
#define DISPLAY_ROTATION 1    // Landscape orientation

// SD Card Pins (HSPI)
#define SD_SCK  40
#define SD_MISO 39
#define SD_MOSI 14
#define SD_CS   12
#define SD_SPI_FREQ 25000000  // 25MHz

// NeoPixel LED
#define LED_PIN 21
#define LED_COUNT 1

// Speaker/Buzzer
#define SPEAKER_PIN 41

// LoRa SX1262 Hat Pins (when attached)
#define LORA_SCK  36
#define LORA_MISO 37
#define LORA_MOSI 35
#define LORA_CS   34
#define LORA_RST  33
#define LORA_IRQ  39  // DIO1
#define LORA_BUSY 38

// Button Pins (if not using M5Unified)
// Note: Cardputer ADV keyboard is handled by TCA8418
// Side buttons are typically handled through M5Unified

#endif // HARDWARE_CONFIG_H
