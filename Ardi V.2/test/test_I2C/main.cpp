// I have to make it used the actual testing. rn its copy paste code.

#include "I2CScanner.h"
#include <unity.h>

I2CScanner scanner;

const byte address;

void debug(byte address)
{
	Serial.print("Found at 0x");
	Serial.println(address, HEX);
}

void BMP280Check() {
	TEST_ASSERT_TRUE(scanner.Check(0x69));
}

void ICM20948Check() {
	TEST_ASSERT_TRUE(scanner.Check(0x76));
}

void setup() 
{

	scanner.Init();
	scanner.Execute(debug);
	UNITY_BEGIN();
	RUN_TEST(BMP280Check);
	RUN_TEST(ICM20948Check);
	UNITY_END();

}

void loop() {}