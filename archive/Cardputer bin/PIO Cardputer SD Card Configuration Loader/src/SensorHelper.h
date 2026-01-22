#ifndef SENSOR_HELPER_H
#define SENSOR_HELPER_H

#include <M5Unified.h>

// Sensor data structures
struct IMUData {
    float accelX, accelY, accelZ;  // m/s²
    float gyroX, gyroY, gyroZ;     // degrees/s
    float pitch, roll, yaw;        // degrees
    float temperature;              // °C
    uint32_t timestamp;
};

struct EnvironmentData {
    float temperature;   // °C
    float humidity;      // %
    float pressure;      // hPa
    float altitude;      // meters
    uint32_t timestamp;
};

struct LightData {
    uint16_t ambient;    // lux
    uint16_t ir;         // IR intensity
    uint16_t visible;    // visible light
    uint32_t timestamp;
};

class SensorHelper {
public:
    SensorHelper();
    
    // Initialization
    bool begin();
    void end();
    
    // IMU (Inertial Measurement Unit)
    bool hasIMU();
    IMUData readIMU();
    void calibrateIMU();
    void updateIMU();  // Call regularly for accurate data
    
    // Environment sensors
    bool hasEnvironmentSensor();
    EnvironmentData readEnvironment();
    
    // Light sensor
    bool hasLightSensor();
    LightData readLight();
    uint16_t getAmbientLight();
    
    // Temperature
    float getTemperature();  // From IMU or environment sensor
    
    // Motion detection
    bool isMoving();
    bool detectShake();
    bool detectTap();
    
    // Orientation
    void getOrientation(float &pitch, float &roll, float &yaw);
    bool isLevelHorizontal(float tolerance = 5.0);  // degrees
    
    // Power sensor
    float getBatteryVoltage();
    int getBatteryLevel();  // 0-100%
    bool isCharging();
    float getBatteryCurrent();  // mA
    
    // Status
    bool isInitialized();
    void printSensorInfo();
    
    // Calibration
    void saveCalibration();
    void loadCalibration();
    
private:
    bool initialized;
    bool imuAvailable;
    bool envAvailable;
    bool lightAvailable;
    
    IMUData lastIMU;
    EnvironmentData lastEnv;
    LightData lastLight;
    
    // IMU calibration offsets
    float accelOffsetX, accelOffsetY, accelOffsetZ;
    float gyroOffsetX, gyroOffsetY, gyroOffsetZ;
    
    // Motion detection thresholds
    float shakeThreshold;
    float tapThreshold;
    
    // Internal helpers
    void calculateOrientation();
    float lowPassFilter(float current, float previous, float alpha = 0.5);
};

// Global sensor helper instance
extern SensorHelper sensors;

#endif // SENSOR_HELPER_H
