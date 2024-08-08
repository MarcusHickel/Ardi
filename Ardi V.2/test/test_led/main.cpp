#include <Arduino.h>
#include <unity.h>

void blink() {
      // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);   delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);  delay(100);
  digitalWrite(LED_BUILTIN, LOW);   delay(100);

 digitalWrite(LED_BUILTIN, HIGH);  delay(1000);
 digitalWrite(LED_BUILTIN, LOW);   delay(1000);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  UNITY_BEGIN();
  RUN_TEST(blink);
  UNITY_END();
}

void loop() {}