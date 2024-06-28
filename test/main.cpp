#include <Adafruit_BMP280.h>
#include <Arduino.h>
#include <SPIFlash.h>
#include <unity.h>

SPIFlash flash(FLASH_CS);

void test_flash() { TEST_ASSERT_TRUE(flash.begin()); }

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_flash);
  UNITY_END();
}

void loop() {}
