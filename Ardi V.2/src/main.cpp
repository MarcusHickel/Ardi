#include <Arduino.h>
#include <DFRobot_BMP280.h>
  
typedef DFRobot_BMP280_IIC    BMP;
BMP   bmp(&Wire, BMP::eSdoLow);

void setup() {
  Serial.begin(115200);
  Serial.println("bmp read data test");
  Serial.println(bmp.begin());
}

void loop() {
 
  delay(1000);
  Serial.println(bmp.getPressure());
  }