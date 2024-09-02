#include <WS2812FX.h>
#include <Arduino.h>

#include <DFRobot_BMP280.h>
#include <Wire.h>

#include <Adafruit_ICM20948.h>

#include <SPI.h>
#include <SD.h>
// #include <FS.h>

#include <TinyGPSPlus.h>

#include <LoRa.h>

#define LED_COUNT 1
#define LED_PIN 21

#define I2C_SCL 1
#define I2C_SDA 2

#define SPI_CLK 7
#define SPI_MISO 8
#define SPI_MOSI 9
#define SPI_CSSD 10 // SD Card Chip Select

#define SPI_CSRF 11 // LoRa Chip select

#define SERIAL_TX 5
#define SERIAL_RX 4
#define GPS_BAUD 9600

#define LoRa_FREQ 433.0 // MUST BE WITHIN LEGALLY 
#define LoRa_INT 15
#define LoRa_RST 16

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

HardwareSerial gpsSerial(2);
TinyGPSPlus gps;

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
    ws2812fx.setColor(0x00FF00); // green 
    ws2812fx.service();
  }
}

void FlashGreenOrange() {
  for (int i = 0; i < 2; i++) {
    delay(100);
    ws2812fx.setColor(0x000000); 
    ws2812fx.service();
    delay(100);
    ws2812fx.setColor(0x00FF00); // green 
    ws2812fx.service();
  }
    delay(100);
    ws2812fx.setColor(0x000000); 
    ws2812fx.service();
    delay(100);
    ws2812fx.setColor(0xFF3000); // orange 
    ws2812fx.service();
    delay(500); // Hold for a bit longer for visibility
}

   // Flash blue to indicate next module test 
void SetLEDBlue() {
  delay(200);
  ws2812fx.setColor(0x0000FF); // BLUE 
  ws2812fx.service();
  delay(200);
}

void setup() {
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, SERIAL_RX, SERIAL_TX); // Doing this now to give it time to warm up

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

  // ------ BMP280 TEST ------ //
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
  SetLEDBlue();
  
  // ------ ICM20948 TEST ------ //
  icm.begin_I2C();
  icm.getEvent(&accel, &gyro, &temp, &mag);
  if (accel.acceleration.z > 0) { // Could make a more diverse test here
    FlashGreen();
  } else {
    FlashRed();
  }

  SetLEDBlue();

  SPI.begin(SPI_CLK,SPI_MISO,SPI_MOSI);
  // ------ SDCARD Test ------ //
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

  // Flash blue to indicate next module test 
  SetLEDBlue();

  // ------ NEO-6M (GPS) Test ------ //
  /* 
  3 Green flashes mean success, 2 green ending in orange means receving from module but invalid data (probably no lock)
  */
  // gpsSerial.begin(GPS_BAUD, SERIAL_8N1, SERIAL_RX, SERIAL_TX); // Starting this earlier to give it time to 'warm up'
  while (gpsSerial.available() > 0) // Wait till data comes in
  if (gps.encode(gpsSerial.read())); // Pointless if statement 
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    FlashRed();
  } else if (!gps.location.isValid()) {FlashGreenOrange();}
    else {FlashGreen();}
  Serial.print("IsValid: "); Serial.println(gps.location.isValid());
  
  SetLEDBlue();

  //LoRa setup and test
  Serial.println("LoRa TX Test!");
  LoRa.setPins(SPI_CSRF,LoRa_RST,LoRa_INT);

    while (!LoRa.begin(433E6)) { // 433MHz is the 70cm Amateur, make sure you got a licence
    Serial.println("LoRa radio init failed");
    FlashRed();
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  Serial.println("Sending Packet");
  // Send a message to rf95_server

  // send packet
  LoRa.beginPacket();
  LoRa.print("LoRa Test");
  LoRa.endPacket();
  FlashGreen();

  // Testing Complete Set standby mode
  ws2812fx.setColor(0x0000FF); // BLUE 
  ws2812fx.service();
}


void loop() {
  
  ws2812fx.setColor(0x0000FF); // BLUE 
  ws2812fx.service();
  delay(100);
  ws2812fx.setColor(0x000000);
  ws2812fx.service();
  delay(1000);
  while (gpsSerial.available() > 0) // Wait till data comes in
  if (gps.encode(gpsSerial.read()));
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
  }
  Serial.print("IsValid: "); Serial.println(gps.location.isValid());
  Serial.print(gps.time.second());
  // icm.getEvent(&accel, &gyro, &temp, &mag);
  // Serial.println("Hello world!");
  // Serial.println(accel.acceleration.x);
  // Serial.println(accel.acceleration.y);
  // Serial.println(accel.acceleration.z);
  // Serial.println(bmp.getPressure());
  // delay(1000);
  //   while (gpsSerial.available() > 0){
  //   // get the byte data from the GPS
  //   char gpsData = gpsSerial.read();
  //   Serial.print(gpsData);
  // }
  // while (gpsSerial.available() > 0)
  // Serial.println((gpsSerial.available()));
  //   if (gps.encode(gpsSerial.read()));
  // Serial.print(F("  Date/Time: "));
  // if (gps.date.isValid())
  // {
  //   Serial.print(gps.date.day());
  //   Serial.print(F("/"));
  //   Serial.print(gps.date.month());
  //   Serial.print(F("/"));
  //   Serial.print(gps.date.year());
  // }
  // else
  // {
  //   Serial.print(F("INVALID"));
  // }

  // Serial.print(F(" "));
  // if (gps.time.isValid())
  // {
  //   if (gps.time.hour() < 10) Serial.print(F("0"));
  //   Serial.print(gps.time.hour());
  //   Serial.print(F(":"));
  //   if (gps.time.minute() < 10) Serial.print(F("0"));
  //   Serial.print(gps.time.minute());
  //   Serial.print(F(":"));
  //   if (gps.time.second() < 10) Serial.print(F("0"));
  //   Serial.print(gps.time.second());
  //   Serial.print(F("."));
  //   if (gps.time.centisecond() < 10) Serial.print(F("0"));
  //   Serial.print(gps.time.centisecond());
  // }
  // else
  // {
  //   Serial.print(F("INVALID"));
  // }
  // delay(1000);
  // Serial.println("-------------------------------");
}
