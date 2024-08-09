#include <Wire.h>
#include <unity.h>
#include <SPI.h>
#include <SD.h>

char testChar;

const int chipSelect = 10;
File dataFile;

Sd2Card card;
SdVolume volume;
SdFile root;

void test_initialise() {
    delay(1000);
    TEST_ASSERT_TRUE(SD.begin(chipSelect));
}

void test_createFile() {
    dataFile = SD.open("test.txt", FILE_WRITE);
    dataFile.close();
    TEST_ASSERT_TRUE(SD.exists("test.txt"));
}

void test_write() {
    if (SD.exists("test.txt")) {SD.remove("test.txt");}
    File dataFile = SD.open("test.txt", FILE_WRITE);
    dataFile.print("test");
    TEST_ASSERT_EQUAL_INT16(4,dataFile.size());
    dataFile.close();
}

void test_read() {
    File dataFile = SD.open("test.txt", FILE_READ);

    testChar = dataFile.read();
    Serial.println(testChar);
    TEST_ASSERT_EQUAL_CHAR(0x74,testChar);
    testChar = dataFile.read();
    TEST_ASSERT_EQUAL_CHAR(0x65,testChar);
    testChar = dataFile.read();
    TEST_ASSERT_EQUAL_CHAR(0x73,testChar);
    testChar = dataFile.read();
    TEST_ASSERT_EQUAL_CHAR(0x74,testChar);

    dataFile.close();
}

void test_removeFile() {
    dataFile.close();
    dataFile = SD.open("test.txt", FILE_WRITE);
    SD.remove("test.txt");
    TEST_ASSERT_FALSE_MESSAGE(SD.exists("test.txt"),"Failed to delete file");
}


void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_initialise);
    RUN_TEST(test_createFile);
    RUN_TEST(test_write);
    RUN_TEST(test_read);
    RUN_TEST(test_removeFile);
    UNITY_END();
    dataFile.close();
}

void loop() {}