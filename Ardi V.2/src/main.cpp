#include <Arduino.h>
#include <Wire.h>

#include <DFRobot_BMP280.h>
#include <Adafruit_ICM20948.h>

#include <SPI.h>
#include <SD.h>
// #include <Adafruit_ICM20X.h>
  
typedef DFRobot_BMP280_IIC    BMP;
BMP   bmp(&Wire, BMP::eSdoHigh);
uint32_t BMP280pressure = 0;
float BMP280temperature;

Adafruit_ICM20948 icm;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t mag;
sensors_event_t temp; // Unused

const int SDCardChipSelect = 10;

int fileNumber = 0;
char fileName[8];
boolean validFile = false;

unsigned long time;
unsigned long lastTime = 0;
unsigned long deltaTime = 0;

void ICM20948Setup();
void FileSetup();
void PrintToSerial();
void WriteToSDCard();

void setup() {
  Serial.begin(115200);

  // Boot and test modules
  Serial.print("BMP280..."); 
  if (bmp.begin()) { // Returns a false?
    Serial.println("passed");
  } else { 
    Serial.println("failed");
  }

  Serial.print("ICM20948...");
  if (icm.begin_I2C()) {
    Serial.println("passed");
  } else { 
    Serial.println("failed");
  }
  // ICM20948Setup();

  Serial.print("SDcard..."); 
  if (SD.begin(SDCardChipSelect)) { // Returns a false?
    Serial.println("passed");
  } else { 
    Serial.println("failed");
  }

  // FileSetup();

}

void loop() {
  
  delay(1000);
  // time = millis();

  icm.getEvent(&accel, &gyro, &temp, &mag);
  BMP280pressure = bmp.getPressure();
  BMP280temperature = bmp.getTemperature();
  // deltaTime = time - lastTime;
  // lastTime = millis();
  PrintToSerial();
  // WriteToSDCard();

}  
  
// ----------------------------------------------------------- //

// void ICM20948Setup() {
//   // icm.setAccelRange(ICM20948_ACCEL_RANGE_16_G);
//   Serial.print("Accelerometer range set to: ");
//   switch (icm.getAccelRange()) {
//   case ICM20948_ACCEL_RANGE_2_G:
//     Serial.println("+-2G");
//     break;
//   case ICM20948_ACCEL_RANGE_4_G:
//     Serial.println("+-4G");
//     break;
//   case ICM20948_ACCEL_RANGE_8_G:
//     Serial.println("+-8G");
//     break;
//   case ICM20948_ACCEL_RANGE_16_G:
//     Serial.println("+-16G");
//     break;
//   }
//   Serial.println("OK");

//   // icm.setGyroRange(ICM20948_GYRO_RANGE_2000_DPS);
//   Serial.print("Gyro range set to: ");
//   switch (icm.getGyroRange()) {
//   case ICM20948_GYRO_RANGE_250_DPS:
//     Serial.println("250 degrees/s");
//     break;
//   case ICM20948_GYRO_RANGE_500_DPS:
//     Serial.println("500 degrees/s");
//     break;
//   case ICM20948_GYRO_RANGE_1000_DPS:
//     Serial.println("1000 degrees/s");
//     break;
//   case ICM20948_GYRO_RANGE_2000_DPS:
//     Serial.println("2000 degrees/s");
//     break;
//   }

//   //  icm.setAccelRateDivisor(4095);
//   uint16_t accel_divisor = icm.getAccelRateDivisor();
//   float accel_rate = 1125 / (1.0 + accel_divisor);

//   Serial.print("Accelerometer data rate divisor set to: ");
//   Serial.println(accel_divisor);
//   Serial.print("Accelerometer data rate (Hz) is approximately: ");
//   Serial.println(accel_rate);

//   //  icm.setGyroRateDivisor(255);
//   uint8_t gyro_divisor = icm.getGyroRateDivisor();
//   float gyro_rate = 1100 / (1.0 + gyro_divisor);

//   Serial.print("Gyro data rate divisor set to: ");
//   Serial.println(gyro_divisor);
//   Serial.print("Gyro data rate (Hz) is approximately: ");
//   Serial.println(gyro_rate);

//   // icm.setMagDataRate(AK09916_MAG_DATARATE_10_HZ);
//   Serial.print("Magnetometer data rate set to: ");
//   switch (icm.getMagDataRate()) {
//   case AK09916_MAG_DATARATE_SHUTDOWN:
//     Serial.println("Shutdown");
//     break;
//   case AK09916_MAG_DATARATE_SINGLE:
//     Serial.println("Single/One shot");
//     break;
//   case AK09916_MAG_DATARATE_10_HZ:
//     Serial.println("10 Hz");
//     break;
//   case AK09916_MAG_DATARATE_20_HZ:
//     Serial.println("20 Hz");
//     break;
//   case AK09916_MAG_DATARATE_50_HZ:
//     Serial.println("50 Hz");
//     break;
//   case AK09916_MAG_DATARATE_100_HZ:
//     Serial.println("100 Hz");
//     break;
//   }
//   Serial.println();
// }

// void FileSetup() {
//   sprintf(fileName, "%02d.txt", fileNumber);

//   // Serial.println("File check...");
//   while (validFile == false) {
//     if (SD.exists(fileName)){
//       // Serial.print(fileName); Serial.println(" exists. Generating new name.");
//       fileNumber = fileNumber + 1;
//       sprintf(fileName, "%02d.txt", fileNumber);
//     } else {
//       validFile = true;
//       // Serial.print(fileName); Serial.println(" is a valid file name");
//     };
//   }
// }

void PrintToSerial() {
  // Serial.print(">timeStep:");Serial.println(deltaTime);    

  Serial.print(">Pressure:"); Serial.println(BMP280pressure);

  Serial.print(">BMPTemp:"); Serial.println(BMP280temperature);
  Serial.print(">ICMTemp:"); Serial.println(temp.temperature);

  Serial.print(">xGyro:"); Serial.println(gyro.gyro.x);
  Serial.print(">yGyro:"); Serial.println(gyro.gyro.y);
  Serial.print(">zGyro:"); Serial.println(gyro.gyro.z);

  Serial.print(">xAccl:"); Serial.println(accel.acceleration.x);
  Serial.print(">yAccl:"); Serial.println(accel.acceleration.y);
  Serial.print(">zAccl:"); Serial.println(accel.acceleration.z);

  Serial.print(">xMag:"); Serial.println(mag.magnetic.x);
  Serial.print(">yMag:"); Serial.println(mag.magnetic.y);
  Serial.print(">zMag:"); Serial.println(mag.magnetic.z);
  
  // Serial.println("---END---");
}

// void WriteToSDCard() {
//   File dataFile = SD.open(fileName, FILE_WRITE);
//   Serial.println(fileName);
//   dataFile.print("test1");
//   if (dataFile) {
//     dataFile.print(time);                 dataFile.print(",");
//     dataFile.print(deltaTime);            dataFile.print(",");
//     dataFile.print(accel.acceleration.x); dataFile.print(",");
//     dataFile.print(accel.acceleration.y); dataFile.print(",");
//     dataFile.print(accel.acceleration.z); dataFile.print(",");
//     dataFile.print(gyro.gyro.x);          dataFile.print(",");
//     dataFile.print(gyro.gyro.y);          dataFile.print(",");
//     dataFile.print(gyro.gyro.z);          dataFile.print(",");
//     dataFile.print(mag.magnetic.x);       dataFile.print(",");
//     dataFile.print(mag.magnetic.y);       dataFile.print(",");
//     dataFile.print(mag.magnetic.z);       dataFile.print(",");
//     dataFile.print(BMP280pressure);       dataFile.println("");
//     dataFile.print("test");
//     dataFile.close();
//   }
//   // if the file isn't open, pop up an error:
//   else {
//     Serial.println("error opening .txt");
//   }
// }
