#include <Arduino.h>
#include <unity.h>

#include <Adafruit_ICM20948.h>

Adafruit_ICM20948 icm;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t mag;
sensors_event_t temp; // Unused

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_initialise);
    RUN_TEST(test_accelerometer);
    RUN_TEST(test_magnetometer);
    RUN_TEST(test_gyroscope);
    RUN_TEST(test_temperature);
    UNITY_END();
}

void loop() {}