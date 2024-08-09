#include <Arduino.h>
#include <unity.h>

#include <Adafruit_ICM20948.h>

Adafruit_ICM20948 icm;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t mag;
sensors_event_t temp; // Unused

void test_initialise(){
    // icm.setAccelRange(ICM20948_ACCEL_RANGE_16_G);
  Serial.print("Accelerometer range set to: ");
  switch (icm.getAccelRange()) {
  case ICM20948_ACCEL_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case ICM20948_ACCEL_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case ICM20948_ACCEL_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case ICM20948_ACCEL_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  Serial.println("OK");

  // icm.setGyroRange(ICM20948_GYRO_RANGE_2000_DPS);
  Serial.print("Gyro range set to: ");
  switch (icm.getGyroRange()) {
  case ICM20948_GYRO_RANGE_250_DPS:
    Serial.println("250 degrees/s");
    break;
  case ICM20948_GYRO_RANGE_500_DPS:
    Serial.println("500 degrees/s");
    break;
  case ICM20948_GYRO_RANGE_1000_DPS:
    Serial.println("1000 degrees/s");
    break;
  case ICM20948_GYRO_RANGE_2000_DPS:
    Serial.println("2000 degrees/s");
    break;
  }

  //  icm.setAccelRateDivisor(4095);
  uint16_t accel_divisor = icm.getAccelRateDivisor();
  float accel_rate = 1125 / (1.0 + accel_divisor);

  Serial.print("Accelerometer data rate divisor set to: ");
  Serial.println(accel_divisor);
  Serial.print("Accelerometer data rate (Hz) is approximately: ");
  Serial.println(accel_rate);

  //  icm.setGyroRateDivisor(255);
  uint8_t gyro_divisor = icm.getGyroRateDivisor();
  float gyro_rate = 1100 / (1.0 + gyro_divisor);

  Serial.print("Gyro data rate divisor set to: ");
  Serial.println(gyro_divisor);
  Serial.print("Gyro data rate (Hz) is approximately: ");
  Serial.println(gyro_rate);

  // icm.setMagDataRate(AK09916_MAG_DATARATE_10_HZ);
  Serial.print("Magnetometer data rate set to: ");
  switch (icm.getMagDataRate()) {
  case AK09916_MAG_DATARATE_SHUTDOWN:
    Serial.println("Shutdown");
    break;
  case AK09916_MAG_DATARATE_SINGLE:
    Serial.println("Single/One shot");
    break;
  case AK09916_MAG_DATARATE_10_HZ:
    Serial.println("10 Hz");
    break;
  case AK09916_MAG_DATARATE_20_HZ:
    Serial.println("20 Hz");
    break;
  case AK09916_MAG_DATARATE_50_HZ:
    Serial.println("50 Hz");
    break;
  case AK09916_MAG_DATARATE_100_HZ:
    Serial.println("100 Hz");
    break;
  }
  Serial.println();
}

void test_accelerometer() {
    TEST_ASSERT_FLOAT_WITHIN(10,0,accel.acceleration.x);
    TEST_ASSERT_FLOAT_WITHIN(10,0,accel.acceleration.y);
    TEST_ASSERT_FLOAT_WITHIN(10,0,accel.acceleration.z);
}

void test_magnetometer() {
    TEST_ASSERT_FLOAT_WITHIN(10,0,mag.magnetic.x);
    TEST_ASSERT_FLOAT_WITHIN(10,0,mag.magnetic.y);
    TEST_ASSERT_FLOAT_WITHIN(10,0,mag.magnetic.z);
}

void test_gyroscope() {
    TEST_ASSERT_FLOAT_WITHIN(10,0,gyro.gyro.x);
    TEST_ASSERT_FLOAT_WITHIN(10,0,gyro.gyro.y);
    TEST_ASSERT_FLOAT_WITHIN(10,0,gyro.gyro.z);
}

void test_temperature() {
    TEST_ASSERT_FLOAT_WITHIN(30,20,temp.temperature);

}

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