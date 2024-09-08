// ToDo
/* 
Create a Log file with the time and date...DONE!
Contingency if no GPSLock
Log data into file...DONE!
Transmit GPS over LoRa (will wait till i start boardcasting my location)
Statemachine?

time offset for AUS timezone...Done! (And DST)
Refactor code <- Do last

To investigate:
Temperature/Barometer drift
GPS Drift
GPS Data rate
Accelerometer Drift

Dual core usage -> going to bbe hard
  Zero core for data collection
  One core for data writing and transmission
*/

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
#define GPS_BAUD 9600 // Can I increase this?

#define LoRa_FREQ 433.0 // MUST BE WITHIN LEGALLY 
#define LoRa_INT 15
#define LoRa_RST 16


WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

typedef DFRobot_BMP280_IIC    BMP; 
BMP   bmp(&Wire, BMP::eSdoLow);
uint32_t BMP280pressure = 0;
float BMP280temperature;

Adafruit_ICM20948 icm;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t mag;
sensors_event_t temp; // Unused

char testChar;

const int chipSelect = 10;
File testFile;
File dataFile;

HardwareSerial gpsSerial(2);
TinyGPSPlus gps;


char fileName[21];

// ------ FUNCTION DEFINITION ------ //


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
    delay(100); 
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

    
  Wire.begin(I2C_SDA,I2C_SCL); // Set I2C Pins
  SPI.begin(SPI_CLK,SPI_MISO,SPI_MOSI); // Set SPI Pins


  ws2812fx.init();
  ws2812fx.setBrightness(10);
  ws2812fx.setSpeed(100);
  ws2812fx.setColor(0x252525);
  // ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
  ws2812fx.start();
  ws2812fx.service();

  Serial.begin(115200);

  delay (1000);

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



  // ------ BMP280 TEST ------ //
  Serial.print("BMP280 Test...");
  bmp.reset();
  bmp.begin();
  delay(50);
  // Check if pressure within reason
  if (bmp.getPressure() > uint32_t(99315) & bmp.getPressure() < uint32_t(103315)) {
    Serial.println("pass");
    FlashGreen();
  } else {
    Serial.println("fail");
    FlashRed();
  }

  // Flash blue to indicate next module test 
  SetLEDBlue();
  
  // ------ ICM20948 TEST ------ //
  Serial.print("ICM20948 Test...");
  icm.begin_I2C();
  icm.getEvent(&accel, &gyro, &temp, &mag);
  // Checks if Z is point up or is atleast giving a value
  if (accel.acceleration.z > 0) { 
    Serial.println("pass");
    FlashGreen();
  } else {
    Serial.println("fail");
    FlashRed();
  }

  SetLEDBlue();


  // ------ SDCARD Test ------ //
  Serial.print("SDCARD Test...");
  SD.begin(SPI_CSSD);
  if (SD.exists("/test.txt")) {SD.remove("/test.txt");} // If testing file already exist remove it
  File testFile = SD.open("/test.txt", FILE_WRITE);
  if (testFile.print("testing...")) {
    Serial.println("pass");
    FlashGreen();
  } else {
    Serial.println("fail");
    FlashRed();
  }
  testFile.close();

  // Flash blue to indicate next module test 
  SetLEDBlue();

  // ------ NEO-6M (GPS) Test ------ //
  /* 
  3 Green flashes mean success, 2 green ending in orange means receiving from module but invalid data (probably no lock)
  Maybe a purple flash to inidcate time lock?
  */
  Serial.print("NEO-6M (GPS) Test...");
  while (gpsSerial.available() > 0) // Wait till data comes in
  if (gps.encode(gpsSerial.read())); // Pointless if statement ?
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    Serial.println("fail");
    FlashRed();
  } else if (!gps.location.isValid()) {FlashGreenOrange(); Serial.println("pass, but no data");}
    else {Serial.println("pass"); FlashGreen();}

  
  SetLEDBlue();

  //LoRa setup and test
  Serial.print("LoRa TX Test...");
  LoRa.setPins(SPI_CSRF,LoRa_RST,LoRa_INT);
  LoRa.setTxPower(1);
  // LoRa.setSpreadingFactor(10);
    while (!LoRa.begin(433E6)) { // 433MHz is the 70cm Amateur band, make sure you got a licence
    Serial.println("fail");
    FlashRed();
    while (1);
  }
  // Send a message to rf95_server

  // send packet
  // LoRa.beginPacket();
  // LoRa.print("LoRa Test");
  // LoRa.endPacket();
  FlashGreen(); // Not sure how to check if packet successfully sent
  Serial.println("pass");

  // Testing Complete Set standby mode
  ws2812fx.setColor(0x0000FF); // BLUE 
  ws2812fx.service();

  while (!gps.time.isValid())
  {
    while (gpsSerial.available() > 0) // Wait till data comes in
    gps.encode(gpsSerial.read());
  }

  //Create log file named by time and date from GPS
  sprintf(fileName, "/%i%02i%02i_%02i%02i%02i.txt", 
          gps.date.year(), 
          gps.date.month(), 
          gps.date.day(), 
          (gps.time.hour()+10) % 24, // UTC+10 = AEST
          gps.time.minute(), 
          gps.time.second());

  Serial.print("Creating File: ");
  Serial.println(fileName);
  File dataFile = SD.open(fileName, FILE_WRITE);
  if (!SD.exists(fileName)) {Serial.println("Failed to create file");} else {Serial.println("Success!");}
  // Header of file
  dataFile.println("#time(ms),#lat,#long,#alt(m),#pressure(Pa),#temperature(C),#accX,#accY,#accZ,#gyroX,#gyroY,#gyroZ,#magX,#magY,#magZ");
  dataFile.close();
}

void loop() {
  int loopTimerStart = millis();

 
  int timeStamp = millis(); // Move int outside
  icm.getEvent(&accel, &gyro, &temp, &mag);
  BMP280pressure = bmp.getPressure();
  BMP280temperature = bmp.getTemperature();
  while (gpsSerial.available() > 0) // Wait till data comes in
  if (gps.encode(gpsSerial.read()));
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
  }
  // Serial.print("IsValid: "); Serial.println(gps.location.isValid());
  // Serial.println("Sending Time over LoRa");

  // LoRa.beginPacket();
  // LoRa.print(gps.date.value());
  // LoRa.print(gps.time.value());
  // LoRa.endPacket();

  Serial.print("Writing to file: ");
  int writeTimerStart = millis();
  File dataFile = SD.open(fileName, FILE_APPEND);
  dataFile.print(timeStamp); dataFile.print(",");
  dataFile.print(gps.location.lat(), 6); dataFile.print(",");
  dataFile.print(gps.location.lng(), 6); dataFile.print(",");
  dataFile.print(gps.altitude.meters()); dataFile.print(",");
  dataFile.print(BMP280pressure);        dataFile.print(","); // ADD BMP CALCULATED ALT
  dataFile.print(BMP280temperature);     dataFile.print(",");
  dataFile.print(accel.acceleration.x);  dataFile.print(",");
  dataFile.print(accel.acceleration.y);  dataFile.print(",");
  dataFile.print(accel.acceleration.z);  dataFile.print(",");
  dataFile.print(gyro.gyro.x);           dataFile.print(",");
  dataFile.print(gyro.gyro.y);           dataFile.print(",");
  dataFile.print(gyro.gyro.z);           dataFile.print(",");
  dataFile.print(mag.magnetic.x);        dataFile.print(",");
  dataFile.print(mag.magnetic.y);        dataFile.print(",");
  dataFile.print(mag.magnetic.z);        dataFile.println("");
  int writeTimerEnd = millis();
  Serial.print(writeTimerEnd-writeTimerStart); Serial.print(",");
  int loopTimerEnd = millis();
  Serial.println(loopTimerEnd-loopTimerStart);



}
