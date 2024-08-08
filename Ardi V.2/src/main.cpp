#include <Arduino.h>
#include <Wire.h>

#include <DFRobot_BMP280.h>
#include <Adafruit_ICM20948.h>

#include <SPI.h>
#include <SD.h>
// #include <Adafruit_ICM20X.h>
  
typedef DFRobot_BMP280_IIC    BMP;
BMP   bmp(&Wire, BMP::eSdoLow);

Adafruit_ICM20948 icm;

const int SDCardChipSelect = 10;

int fileNumber = 0;
char fileName[8];
boolean validFile = false;

void setup() {
  Serial.begin(115200);

  // Boot and test modules
  Serial.print("BMP280...");
  if (bmp.begin()) {
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

  Serial.print("SDcard...");
  if (SD.begin(SDCardChipSelect)) {
    Serial.println("passed");
  } else { 
    Serial.println("failed");
  }

  // File setup
  sprintf(fileName, "%02d.txt", fileNumber);

  Serial.println("Checking for previous files...");
  while (validFile == false) {
    if (SD.exists(fileName)){
      Serial.print(fileName); Serial.println(" exists. Generating new name.");
      fileNumber = fileNumber + 1;
      sprintf(fileName, "%02d.txt", fileNumber);
    } else {
      validFile = true;
      Serial.print(fileName); Serial.println(" is a valid file name");
    };
  }
}

void loop() {
 
  delay(1000);
  Serial.println(bmp.getPressure());
  Serial.println(bmp.getTemperature());
  }