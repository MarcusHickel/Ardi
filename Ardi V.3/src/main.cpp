#include <WS2812FX.h>
#include <Arduino.h>

#include <DFRobot_BMP280.h>
#include <Wire.h>

#include <Adafruit_ICM20948.h>

#include <SPI.h>
#include <SD.h>
// #include <FS.h>

#define LED_COUNT 1
#define LED_PIN 21

#define I2C_SCL 1
#define I2C_SDA 2

#define SPI_CLK 7
#define SPI_MISO 8
#define SPI_MOSI 9
#define SPI_CSSD 10

#define SPI_CSRF 11

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

typedef DFRobot_BMP280_IIC    BMP; 
BMP   bmp(&Wire, BMP::eSdoLow);

Adafruit_ICM20948 icm;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t mag;
sensors_event_t temp; // Unused

char testChar;

const int chipSelect = 10;
File dataFile;

void FlashRed() {
  for (int i = 0; i < 3; i++) {
    delay(100);
    ws2812fx.setColor(0x000000); 
    ws2812fx.service();
    delay(100);
    ws2812fx.setColor(0xFF0000); // red 
    ws2812fx.service();
  }
}

void FlashGreen() {
  for (int i = 0; i < 3; i++) {
    delay(100);
    ws2812fx.setColor(0x000000); 
    ws2812fx.service();
    delay(100);
    ws2812fx.setColor(0x00FF00); // red 
    ws2812fx.service();
  }
}

void setup() {
  ws2812fx.init();
  ws2812fx.setBrightness(10);
  ws2812fx.setSpeed(100);
  ws2812fx.setColor(0x252525);
  // ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
  ws2812fx.start();
  Serial.begin(115200);

  
  ws2812fx.service();
  delay(500);

  // Flash RGB 3 times for LED test
  for (int i = 0; i < 3; i++) {
    delay(200);
    ws2812fx.setColor(0xFF0000); // RED
    ws2812fx.service();
    delay(200);
    ws2812fx.setColor(0x00FF00); // GREEN 
    ws2812fx.service();
    delay(200);
    ws2812fx.setColor(0x0000FF); // BLUE 
    ws2812fx.service();
    
  }

  // Set I2C Pins
  Wire.begin(I2C_SDA,I2C_SCL);

  // BMP280 TEST
  bmp.reset();
  bmp.begin();
  delay(50);
  // Serial.println(bmp.getPressure());
  if (bmp.getPressure() > uint32_t(99315) & bmp.getPressure() < uint32_t(103315)) {
    FlashGreen();
  } else {
    FlashRed();
  }

  // Flash blue to indicate next module test 
  delay(200);
  ws2812fx.setColor(0x0000FF); // BLUE 
  ws2812fx.service();
  delay(200);
  
  //ICM20948 TEST
  icm.begin_I2C();
  icm.getEvent(&accel, &gyro, &temp, &mag);
  if (accel.acceleration.z > 0) { // Could make a more diverse test here
    FlashGreen();
  } else {
    FlashRed();
  }

  delay(200);
  ws2812fx.setColor(0x0000FF); // BLUE 
  ws2812fx.service();
  delay(200);


  SPI.begin(SPI_CLK,SPI_MISO,SPI_MOSI);
  //SDCARD Test
  SD.begin(SPI_CSSD);
  if (SD.exists("/test.txt")) {SD.remove("/test.txt");}
  File dataFile = SD.open("/test.txt", FILE_WRITE);
  if (dataFile.print("testing...")) {
    Serial.println("File written");
    FlashGreen();
  } else {
    Serial.println("Write failed");
    FlashRed();
  }
  dataFile.close();

}


void loop() {
  // icm.getEvent(&accel, &gyro, &temp, &mag);
  // Serial.println("Hello world!");
  // Serial.println(accel.acceleration.x);
  // Serial.println(accel.acceleration.y);
  // Serial.println(accel.acceleration.z);
  // Serial.println(bmp.getPressure());
  // delay(1000);
}
