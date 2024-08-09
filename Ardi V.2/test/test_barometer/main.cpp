#include <Arduino.h>
#include <DFRobot_BMP280.h>
#include <Wire.h>
#include <unity.h>

typedef DFRobot_BMP280_IIC    BMP; 
BMP   bmp(&Wire, BMP::eSdoLow);

void test_initialise() {
    TEST_ASSERT_TRUE(!bmp.begin()); // Return Enum of eStatus_t, 0 means OK
}

void test_pressure() { 
    delay(50); // This delay needs to be here for some reason otherwise, reads 68852
    TEST_ASSERT_INT32_WITHIN(uint32_t(2000), uint32_t(101315), bmp.getPressure());
}

void test_temperature() {
    TEST_ASSERT_FLOAT_WITHIN(30, 20, bmp.getTemperature());
}

void setup() {
    bmp.reset();
    UNITY_BEGIN();
    RUN_TEST(test_initialise);
    RUN_TEST(test_temperature);
    RUN_TEST(test_pressure);
    UNITY_END();
}

void loop() {}