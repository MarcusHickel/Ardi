#include <WS2812FX.h>
#include <Arduino.h>

#include <DFRobot_BMP280.h>
#include <Wire.h>

#define LED_COUNT 1
#define LED_PIN 21

#define I2C_SCL 1
#define I2C_SDA 2

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

typedef DFRobot_BMP280_IIC    BMP; 
BMP   bmp(&Wire, BMP::eSdoLow);

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


  Wire.begin(I2C_SDA,I2C_SCL);
  // BMP280 TEST
  bmp.reset();
  bmp.begin();
  delay(50);
  Serial.println(bmp.getPressure());
  if (bmp.getPressure() > uint32_t(99315) & bmp.getPressure() < uint32_t(103315)) {
      for (int i = 0; i < 3; i++) {
        delay(100);
        ws2812fx.setColor(0x000000); 
        ws2812fx.service();
        delay(100);
        ws2812fx.setColor(0x00FF00); // GREEN 
        ws2812fx.service();
      }
  } else {
      for (int i = 0; i < 3; i++) {
      delay(100);
      ws2812fx.setColor(0x000000); 
      ws2812fx.service();
      delay(100);
      ws2812fx.setColor(0xFF0000); // red 
      ws2812fx.service();
    }
  }
  


}


void loop() {
  Serial.println("Hello world!");
  Serial.println(bmp.getPressure());
  delay(1000);
}
