#include "SensorHelper.h"
#include <cmath>

// Global instance
SensorHelper sensors;

SensorHelper::SensorHelper() : 
    initialized(false),
    imuAvailable(false),
    envAvailable(false),
    lightAvailable(false),
    accelOffsetX(0), accelOffsetY(0), accelOffsetZ(0),
    gyroOffsetX(0), gyroOffsetY(0), gyroOffsetZ(0),
    shakeThreshold(2.5),
    tapThreshold(3.0) {
    
    memset(&lastIMU, 0, sizeof(IMUData));
    memset(&lastEnv, 0, sizeof(EnvironmentData));
    memset(&lastLight, 0, sizeof(LightData));
}

bool SensorHelper::begin() {
    Serial.println("Initializing sensors...");
    
    // Check for IMU
    if (M5.Imu.isEnabled()) {
        imuAvailable = true;
        Serial.println("IMU detected");
    } else {
        Serial.println("No IMU found");
    }
    
    // Check for environment sensor (if available)
    // Cardputer ADV may not have this, depends on configuration
    envAvailable = false;
    
    // Check for light sensor
    // Cardputer ADV may have ambient light sensor
    lightAvailable = false;
    
    initialized = true;
    
    printSensorInfo();
    
    return initialized;
}

void SensorHelper::end() {
    initialized = false;
    imuAvailable = false;
    envAvailable = false;
    lightAvailable = false;
    Serial.println("Sensors deinitialized");
}

bool SensorHelper::hasIMU() {
    return imuAvailable;
}

IMUData SensorHelper::readIMU() {
    IMUData data;
    data.timestamp = millis();
    
    if (!imuAvailable) {
        memset(&data, 0, sizeof(IMUData));
        return data;
    }
    
    // Read from M5Unified IMU
    auto imu_data = M5.Imu.getImuData();
    
    // Acceleration (apply offsets)
    data.accelX = imu_data.accel.x - accelOffsetX;
    data.accelY = imu_data.accel.y - accelOffsetY;
    data.accelZ = imu_data.accel.z - accelOffsetZ;
    
    // Gyroscope (apply offsets)
    data.gyroX = imu_data.gyro.x - gyroOffsetX;
    data.gyroY = imu_data.gyro.y - gyroOffsetY;
    data.gyroZ = imu_data.gyro.z - gyroOffsetZ;
    
    // Temperature from IMU
    data.temperature = M5.Imu.getTemp();
    
    // Calculate orientation
    float ax = data.accelX;
    float ay = data.accelY;
    float az = data.accelZ;
    
    data.pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / M_PI;
    data.roll = atan2(ay, az) * 180.0 / M_PI;
    data.yaw = 0;  // Requires magnetometer for absolute yaw
    
    lastIMU = data;
    return data;
}

void SensorHelper::calibrateIMU() {
    if (!imuAvailable) return;
    
    Serial.println("Calibrating IMU... Keep device still");
    
    float sumAccelX = 0, sumAccelY = 0, sumAccelZ = 0;
    float sumGyroX = 0, sumGyroY = 0, sumGyroZ = 0;
    
    const int samples = 100;
    
    for (int i = 0; i < samples; i++) {
        auto imu_data = M5.Imu.getImuData();
        
        sumAccelX += imu_data.accel.x;
        sumAccelY += imu_data.accel.y;
        sumAccelZ += imu_data.accel.z;
        
        sumGyroX += imu_data.gyro.x;
        sumGyroY += imu_data.gyro.y;
        sumGyroZ += imu_data.gyro.z;
        
        delay(10);
    }
    
    accelOffsetX = sumAccelX / samples;
    accelOffsetY = sumAccelY / samples;
    accelOffsetZ = (sumAccelZ / samples) - 1.0;  // 1G expected on Z axis
    
    gyroOffsetX = sumGyroX / samples;
    gyroOffsetY = sumGyroY / samples;
    gyroOffsetZ = sumGyroZ / samples;
    
    Serial.println("IMU calibration complete");
    Serial.printf("Accel offsets: X=%.3f, Y=%.3f, Z=%.3f\n", accelOffsetX, accelOffsetY, accelOffsetZ);
    Serial.printf("Gyro offsets: X=%.3f, Y=%.3f, Z=%.3f\n", gyroOffsetX, gyroOffsetY, gyroOffsetZ);
}

void SensorHelper::updateIMU() {
    if (!imuAvailable) return;
    
    M5.Imu.update();
    readIMU();
}

bool SensorHelper::hasEnvironmentSensor() {
    return envAvailable;
}

EnvironmentData SensorHelper::readEnvironment() {
    EnvironmentData data;
    data.timestamp = millis();
    
    // Cardputer ADV doesn't have built-in environment sensor
    // This could be added via external I2C sensor
    memset(&data, 0, sizeof(EnvironmentData));
    
    return data;
}

bool SensorHelper::hasLightSensor() {
    return lightAvailable;
}

LightData SensorHelper::readLight() {
    LightData data;
    data.timestamp = millis();
    
    // Cardputer ADV may have ambient light sensor
    // Implementation depends on hardware configuration
    memset(&data, 0, sizeof(LightData));
    
    return data;
}

uint16_t SensorHelper::getAmbientLight() {
    if (!lightAvailable) return 0;
    
    LightData light = readLight();
    return light.ambient;
}

float SensorHelper::getTemperature() {
    if (imuAvailable) {
        return M5.Imu.getTemp();
    } else if (envAvailable) {
        EnvironmentData env = readEnvironment();
        return env.temperature;
    }
    
    return 0.0;
}

bool SensorHelper::isMoving() {
    if (!imuAvailable) return false;
    
    IMUData imu = readIMU();
    
    // Check if acceleration magnitude differs significantly from 1G
    float accelMag = sqrt(imu.accelX * imu.accelX + 
                         imu.accelY * imu.accelY + 
                         imu.accelZ * imu.accelZ);
    
    return (abs(accelMag - 1.0) > 0.2);
}

bool SensorHelper::detectShake() {
    if (!imuAvailable) return false;
    
    IMUData imu = readIMU();
    
    // Check if any acceleration axis exceeds threshold
    if (abs(imu.accelX) > shakeThreshold ||
        abs(imu.accelY) > shakeThreshold ||
        abs(imu.accelZ) > shakeThreshold) {
        return true;
    }
    
    return false;
}

bool SensorHelper::detectTap() {
    if (!imuAvailable) return false;
    
    IMUData imu = readIMU();
    
    // Simple tap detection - check for sudden acceleration spike
    float accelMag = sqrt(imu.accelX * imu.accelX + 
                         imu.accelY * imu.accelY + 
                         imu.accelZ * imu.accelZ);
    
    return (accelMag > tapThreshold);
}

void SensorHelper::getOrientation(float &pitch, float &roll, float &yaw) {
    if (!imuAvailable) {
        pitch = roll = yaw = 0;
        return;
    }
    
    IMUData imu = readIMU();
    pitch = imu.pitch;
    roll = imu.roll;
    yaw = imu.yaw;
}

bool SensorHelper::isLevelHorizontal(float tolerance) {
    if (!imuAvailable) return false;
    
    float pitch, roll, yaw;
    getOrientation(pitch, roll, yaw);
    
    return (abs(pitch) < tolerance && abs(roll) < tolerance);
}

float SensorHelper::getBatteryVoltage() {
    return M5.Power.getBatteryVoltage() / 1000.0;  // mV to V
}

int SensorHelper::getBatteryLevel() {
    return M5.Power.getBatteryLevel();  // 0-100%
}

bool SensorHelper::isCharging() {
    return M5.Power.isCharging();
}

float SensorHelper::getBatteryCurrent() {
    return M5.Power.getBatteryCurrent();  // mA
}

bool SensorHelper::isInitialized() {
    return initialized;
}

void SensorHelper::printSensorInfo() {
    Serial.println("=== Sensor Status ===");
    Serial.print("IMU: "); Serial.println(imuAvailable ? "Available" : "Not available");
    Serial.print("Environment Sensor: "); Serial.println(envAvailable ? "Available" : "Not available");
    Serial.print("Light Sensor: "); Serial.println(lightAvailable ? "Available" : "Not available");
    
    if (imuAvailable) {
        IMUData imu = readIMU();
        Serial.println("IMU Data:");
        Serial.printf("  Accel: X=%.2f Y=%.2f Z=%.2f m/s²\n", imu.accelX, imu.accelY, imu.accelZ);
        Serial.printf("  Gyro: X=%.2f Y=%.2f Z=%.2f °/s\n", imu.gyroX, imu.gyroY, imu.gyroZ);
        Serial.printf("  Orientation: Pitch=%.1f° Roll=%.1f° Yaw=%.1f°\n", imu.pitch, imu.roll, imu.yaw);
        Serial.printf("  Temperature: %.1f°C\n", imu.temperature);
    }
    
    Serial.printf("Battery: %.2fV (%d%%)\n", getBatteryVoltage(), getBatteryLevel());
    Serial.printf("Charging: %s\n", isCharging() ? "Yes" : "No");
    Serial.println("====================");
}

void SensorHelper::saveCalibration() {
    // Save calibration to SD card or EEPROM - future implementation
    Serial.println("Calibration save not yet implemented");
}

void SensorHelper::loadCalibration() {
    // Load calibration from SD card or EEPROM - future implementation
    Serial.println("Calibration load not yet implemented");
}

void SensorHelper::calculateOrientation() {
    // Advanced orientation calculation with gyro fusion - future implementation
}

float SensorHelper::lowPassFilter(float current, float previous, float alpha) {
    return (alpha * current) + ((1.0 - alpha) * previous);
}
