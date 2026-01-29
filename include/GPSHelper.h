// GPSHelper.h - M5Stack GPS Unit Support (NEO-6M/NEO-M8N)
// NMEA parser and GPS data management
#ifndef GPS_HELPER_H
#define GPS_HELPER_H

#include <Arduino.h>
#include <HardwareSerial.h>

#define GPS_SERIAL Serial2
#define GPS_BAUD 9600
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17

struct GPSData {
    bool valid;
    float latitude;
    float longitude;
    float altitude;
    float speed;          // Speed in km/h
    uint8_t satellites;
    uint8_t fixQuality;   // 0=invalid, 1=GPS, 2=DGPS
    float hdop;           // Horizontal dilution of precision
    int year, month, day;
    int hour, minute, second;
    char latDirection;    // 'N' or 'S'
    char lonDirection;    // 'E' or 'W'
};

class GPSHelper {
public:
    GPSHelper();
    bool begin();
    bool isAvailable();
    void update();
    
    // Data access
    GPSData* getData();
    bool hasValidFix();
    String getLatitudeString();
    String getLongitudeString();
    String getTimeString();
    String getDateString();
    
    // Statistics
    uint32_t getSentenceCount();
    uint32_t getErrorCount();
    
private:
    bool available;
    GPSData data;
    String nmeaBuffer;
    uint32_t sentenceCount;
    uint32_t errorCount;
    uint32_t lastUpdate;
    
    void parseNMEA(String sentence);
    void parseGGA(String sentence);
    void parseRMC(String sentence);
    void parseGSA(String sentence);
    float parseCoordinate(String coord, char direction);
    uint8_t parseHex(char c);
    bool verifyChecksum(String sentence);
};

#endif // GPS_HELPER_H
