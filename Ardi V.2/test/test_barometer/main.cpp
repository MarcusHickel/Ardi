#include <Arduino.h>
#include <DFRobot_BMP280.h>
#include "Wire.h"
#include <unity.h>

typedef DFRobot_BMP280_IIC    BMP; 
BMP   bmp(&Wire, BMP::eSdoLow);

void test_barometer() {
    TEST_ASSERT_TRUE(!bmp.begin()); // Is returning a false enum?
    TEST_ASSERT_FLOAT_WITHIN(2000, 10400, float(bmp.getPressure()));
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_barometer);
    UNITY_END();
}

void loop() {}