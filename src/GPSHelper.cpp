// GPSHelper.cpp - M5Stack GPS Unit Implementation
#include "GPSHelper.h"

GPSHelper::GPSHelper() : available(false), sentenceCount(0), errorCount(0), lastUpdate(0) {
    data.valid = false;
    data.latitude = 0.0;
    data.longitude = 0.0;
    data.altitude = 0.0;
    data.speed = 0.0;
    data.satellites = 0;
    data.fixQuality = 0;
    data.hdop = 0.0;
}

bool GPSHelper::begin() {
    GPS_SERIAL.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    
    // Wait briefly for GPS module
    delay(100);
    
    available = true;
    Serial.println("GPS Helper: Initialized on Serial2");
    Serial.printf("GPS: RX=%d, TX=%d, Baud=%d\n", GPS_RX_PIN, GPS_TX_PIN, GPS_BAUD);
    
    return true;
}

bool GPSHelper::isAvailable() {
    return available;
}

void GPSHelper::update() {
    if (!available) return;
    
    while (GPS_SERIAL.available()) {
        char c = GPS_SERIAL.read();
        
        if (c == '\n') {
            if (nmeaBuffer.length() > 0) {
                parseNMEA(nmeaBuffer);
                nmeaBuffer = "";
            }
        } else if (c != '\r') {
            nmeaBuffer += c;
            
            // Prevent buffer overflow
            if (nmeaBuffer.length() > 120) {
                nmeaBuffer = "";
            }
        }
    }
}

void GPSHelper::parseNMEA(String sentence) {
    if (sentence.length() < 10 || sentence.charAt(0) != '$') {
        errorCount++;
        return;
    }
    
    // Verify checksum
    if (!verifyChecksum(sentence)) {
        errorCount++;
        return;
    }
    
    sentenceCount++;
    lastUpdate = millis();
    
    // Determine sentence type
    if (sentence.indexOf("$GPGGA") == 0 || sentence.indexOf("$GNGGA") == 0) {
        parseGGA(sentence);
    } else if (sentence.indexOf("$GPRMC") == 0 || sentence.indexOf("$GNRMC") == 0) {
        parseRMC(sentence);
    } else if (sentence.indexOf("$GPGSA") == 0 || sentence.indexOf("$GNGSA") == 0) {
        parseGSA(sentence);
    }
}

void GPSHelper::parseGGA(String sentence) {
    // $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
    // Format: time,lat,N/S,lon,E/W,quality,sats,hdop,alt,M,geoid,M,,checksum
    
    int idx = 0;
    String fields[15];
    int fieldIdx = 0;
    
    for (int i = 0; i < sentence.length() && fieldIdx < 15; i++) {
        char c = sentence.charAt(i);
        if (c == ',' || c == '*') {
            fieldIdx++;
        } else {
            fields[fieldIdx] += c;
        }
    }
    
    // Parse fix quality
    if (fields[6].length() > 0) {
        data.fixQuality = fields[6].toInt();
        data.valid = (data.fixQuality > 0);
    }
    
    // Parse coordinates
    if (fields[2].length() > 0 && fields[3].length() > 0) {
        data.latitude = parseCoordinate(fields[2], fields[3].charAt(0));
        data.latDirection = fields[3].charAt(0);
    }
    
    if (fields[4].length() > 0 && fields[5].length() > 0) {
        data.longitude = parseCoordinate(fields[4], fields[5].charAt(0));
        data.lonDirection = fields[5].charAt(0);
    }
    
    // Parse satellites
    if (fields[7].length() > 0) {
        data.satellites = fields[7].toInt();
    }
    
    // Parse HDOP
    if (fields[8].length() > 0) {
        data.hdop = fields[8].toFloat();
    }
    
    // Parse altitude
    if (fields[9].length() > 0) {
        data.altitude = fields[9].toFloat();
    }
}

void GPSHelper::parseRMC(String sentence) {
    // $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
    // Format: time,status,lat,N/S,lon,E/W,speed,track,date,magvar,E/W,checksum
    
    int idx = 0;
    String fields[13];
    int fieldIdx = 0;
    
    for (int i = 0; i < sentence.length() && fieldIdx < 13; i++) {
        char c = sentence.charAt(i);
        if (c == ',' || c == '*') {
            fieldIdx++;
        } else {
            fields[fieldIdx] += c;
        }
    }
    
    // Parse status (A=active, V=void)
    if (fields[2].length() > 0) {
        data.valid = (fields[2].charAt(0) == 'A');
    }
    
    // Parse speed (knots to km/h)
    if (fields[7].length() > 0) {
        data.speed = fields[7].toFloat() * 1.852;
    }
    
    // Parse date (DDMMYY)
    if (fields[9].length() == 6) {
        data.day = fields[9].substring(0, 2).toInt();
        data.month = fields[9].substring(2, 4).toInt();
        data.year = 2000 + fields[9].substring(4, 6).toInt();
    }
    
    // Parse time (HHMMSS)
    if (fields[1].length() >= 6) {
        data.hour = fields[1].substring(0, 2).toInt();
        data.minute = fields[1].substring(2, 4).toInt();
        data.second = fields[1].substring(4, 6).toInt();
    }
}

void GPSHelper::parseGSA(String sentence) {
    // Parse DOP and active satellites (optional enhancement)
}

float GPSHelper::parseCoordinate(String coord, char direction) {
    if (coord.length() < 4) return 0.0;
    
    // Find decimal point
    int dotPos = coord.indexOf('.');
    if (dotPos < 0) return 0.0;
    
    // NMEA format: DDMM.MMMM or DDDMM.MMMM
    int degLen = (dotPos >= 4) ? dotPos - 2 : dotPos - 2;
    if (degLen < 0) degLen = 2;
    
    float degrees = coord.substring(0, degLen).toFloat();
    float minutes = coord.substring(degLen).toFloat();
    
    float decimal = degrees + (minutes / 60.0);
    
    // Apply direction
    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    
    return decimal;
}

bool GPSHelper::verifyChecksum(String sentence) {
    int starPos = sentence.indexOf('*');
    if (starPos < 0) return false;
    
    // Calculate checksum (XOR of all chars between $ and *)
    uint8_t checksum = 0;
    for (int i = 1; i < starPos; i++) {
        checksum ^= sentence.charAt(i);
    }
    
    // Parse provided checksum
    String checksumStr = sentence.substring(starPos + 1);
    if (checksumStr.length() < 2) return false;
    
    uint8_t providedChecksum = (parseHex(checksumStr.charAt(0)) << 4) | parseHex(checksumStr.charAt(1));
    
    return (checksum == providedChecksum);
}

uint8_t GPSHelper::parseHex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

GPSData* GPSHelper::getData() {
    return &data;
}

bool GPSHelper::hasValidFix() {
    return data.valid && data.fixQuality > 0;
}

String GPSHelper::getLatitudeString() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.6f %c", fabs(data.latitude), data.latDirection);
    return String(buf);
}

String GPSHelper::getLongitudeString() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.6f %c", fabs(data.longitude), data.lonDirection);
    return String(buf);
}

String GPSHelper::getTimeString() {
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", data.hour, data.minute, data.second);
    return String(buf);
}

String GPSHelper::getDateString() {
    char buf[16];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d", data.year, data.month, data.day);
    return String(buf);
}

uint32_t GPSHelper::getSentenceCount() {
    return sentenceCount;
}

uint32_t GPSHelper::getErrorCount() {
    return errorCount;
}
