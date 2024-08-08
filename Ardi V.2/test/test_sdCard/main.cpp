#include <Wire.h>
#include <unity.h>
#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;

void test_sdCard() {
    TEST_ASSERT_TRUE(SD.begin(chipSelect));
    if (SD.exists("test.txt")) {SD.remove("test.txt");}
    File dataFile = SD.open("test.txt", FILE_WRITE);
    TEST_ASSERT_TRUE(dataFile);
    dataFile.print("test");
    dataFile.close();
    TEST_MESSAGE("Check if 'test' was written to test.txt");
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_sdCard);
    UNITY_END();
}

void loop() {}