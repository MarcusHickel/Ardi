#include <WS2812FX.h>
#include <unity.h>
#include <Arduino.h>

#define LED_COUNT 1
#define LED_PIN 21

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

int i = 0;

void blinkRGB() {
    while (++i < 3) {
        ws2812fx.setColor(0xFF0000); // Red
        ws2812fx.service();
        delay(1000);
        ws2812fx.setColor(0x00FF00); // Green
        ws2812fx.service();
        delay(1000);
        ws2812fx.setColor(0x0000FF); // Blue
        ws2812fx.service();
        delay(1000);
    }
}

void setup() {
  ws2812fx.init();
  ws2812fx.setBrightness(10);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(0x252525);
  // ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
  ws2812fx.start();

  UNITY_BEGIN();
  RUN_TEST(blinkRGB);
  UNITY_END();

}

void loop() {

}
