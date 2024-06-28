#include <Arduino.h>
#include <binary.h>
#include <DFRobot_BMP280.h>
#include <Wire.h>
#include <Talkie.h>
#include <Vocab_US_Large.h>
#include <Vocab_US_Acorn.h>

Talkie voice; // For the talking w/ buzzer
typedef DFRobot_BMP280_IIC    BMP;    // ******** use abbreviations instead of full names ********

BMP   bmp(&Wire, BMP::eSdoLow);

float SEA_LEVEL_PRESSURE = 102060.0;  // Mean Sea Level Pressure in Pa. (MSLP) (QNH). You could update this to current data, but might be a hassle. Need to test differences. Standard pressure is 101325Pa
#define bufferLength 64 // Size of the buffer array

int potPin = A3; // Pin In for Potentiometer. *Unused
int ledPin = 13; // Inbuilt LED for flashing 
int apogeeValue = 0; // Highest encountered value.
int buffer[bufferLength]; // buffer for the data input
//int bufferLength = 64; 
int currentState = 0; // State machine Launch, Powered Ascent, etc.
int i = 0; // For iteration
int timeOut = 0;
float sum = 0; // For calculating average
float bufferAvg = 0; // The average of the buffer, for filtering out spikes in data.
bool debugMode = 1; // for serial monitor. but if serial is plugged in then must be debug right?
bool array[16]; // Bit array for flashing out binary of apogee during recovery stage.

float temperature = 0; // temperature from the BMP280 *UNUSED
uint32_t pressure = 0; // Pressure from the BMP280
uint32_t altitude = 0; // Altitude calculation using pressure and MSLP from the BMP280
float groundLevelPressure = 0;
int glp = 0;

int numberString[7]; // Can be a number from 0 up to 999,999
bool leadingZeroFlag = false; // For checking leading zeros of the number readout


float sumX = 15;  // Used in least squares calculation
float sumY = 0;   // Used in least squares calculation 
float sumXY = 0;  // Used in least squares calculation
float sumX2 = 55; // Used in least squares calculation
float m = 0;        // Used in least squares calculation

// Function declarations

void pntSerial();
void NumberCallout(uint32_t inputNumber); // Converts a number to single digit base 10 and reads it out. (Max 999,999)
void SpeakNumber(int number); // Speaks the number
uint32_t AltitudeCalculation(uint32_t pressure); // Calculates altitude from pressure, based of pressure at sea level. uint32_t because the BMP has a +-1m accuracy anyway
void printLastOperateStatus(BMP::eStatus_t eStatus); // Not me
void UpdateBuffer();
void StandBy(); 
void PoweredAscent();
void Apogee();
void SlowDescent();
void Recovery();
void state();
float Slope();

void setup() {
  //Initialize serial and wait for port to open:
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);

  bmp.reset();
  Serial.println("bmp read data test");
  while(bmp.begin() != BMP::eStatusOK) {
    Serial.println("bmp begin failed");
    printLastOperateStatus(bmp.lastOperateStatus);
    delay(2000);
  }
  Serial.println("bmp begin success");
  delay(100);

  while (!Serial) {
    // if(Serial) // Checking if serial port is connected, if so debugMode is activated
    // {
    //   debugMode = 1;
    // }; // wait for serial port to connect. Needed for native USB
    // delay(1000);
    // i++;
    voice.say(sp4_TERMINAL);
  }
  Serial.print("Serial Successful!");

  // Take the first reading and apply to the entre buffer
  altitude = AltitudeCalculation(bmp.getPressure());
  for (i=0;i<bufferLength;i++)
  {
    buffer[i]=altitude;
  }

  groundLevelPressure = bmp.getPressure(); // Redundant
  SEA_LEVEL_PRESSURE = bmp.getPressure();
  glp = SEA_LEVEL_PRESSURE;
  voice.say(sp5_GROUND);
  voice.say(sp4_PRESSURE);
  voice.say(sp4_IS);
  Serial.println(groundLevelPressure);
  Serial.println((uint32_t)groundLevelPressure/100);
  NumberCallout((uint32_t)groundLevelPressure/100);
}

void loop()
{
  state();
  // float   temperature = bmp.getTemperature(); // Dont need yet. Ignore for now
  pressure = bmp.getPressure();
  altitude = AltitudeCalculation(pressure); // bmp.calAltitude(SEA_LEVEL_PRESSURE, pressure); I wanted more control over the calculation.

  if(debugMode){pntSerial();}
  // NumberCallout(altitude);
  // voice.say(sp2_METER);
  // voice.say(spa__S);

}

// State machine
void state(){
  switch (currentState) 
  {
    case 0: StandBy();       break; // Stand by:
    case 1: PoweredAscent(); break; // poweredAscent: record every 0.5s or 2Hz
    case 2: Apogee();        break; // Apogee: assign largest variable in array to output 
    case 3: SlowDescent();   break; // Slow descent: buffer data every 1s
    case 4: Recovery();      break; // Recovery: Stop receiving data and output Apogee  
  }
}



void StandBy() // buffer data at low rate, as soon as large jump (over noise floor) switch to launch case
{
  // recording new value into buffer
  UpdateBuffer();
  delay(1000);
  // Is new value peaking?  if yes switch to Powered Ascent
  if (Slope() > 1) // 
  {
    currentState = 1; // Current state is changed to Powered Ascent (1)
    voice.say(sp3_INCREASING); // Hard to hear during testing change to beep.
  }

  if (timeOut > 10)
  {
    voice.say(sp4_ON);
    timeOut = 0;
    Serial.println(timeOut);
  }
  else
  {
    timeOut++;
  }
}

void PoweredAscent(){
  Serial.println("Powered Ascent");
  UpdateBuffer();

  // Check if we've reached apogee
  // see if the slope is negative
  m = Slope();
  if (m > 0) {
    Serial.println(m);
    Serial.println(Slope());
    currentState = 2;
    Serial.println("SWITCH TO APOGEE");
  }
  if ( m < 0){Serial.println("m<0");}

}

void Apogee(){
  for ( i=0 ; i<bufferLength ; i++)
  {
    if(apogeeValue < buffer[i]) {apogeeValue = buffer[i];}
  }

  if (buffer[bufferLength-1]<apogeeValue) 
  { 
  currentState = 3;
  voice.say(sp3_HIGH);
  }

  UpdateBuffer();
}

void SlowDescent(){
  UpdateBuffer();

  m = Slope();
  if (m == (0)) // Move on to recovery when slope change is 0 
  {
    currentState = 4;
  } 
}

void Recovery(){
  Serial.println("Recovery mode");

  voice.say(sp2_READY);
  voice.say(sp2_START);
  voice.say(sp5_ALTITUDE);
  delay(100);
  NumberCallout(apogeeValue);
  voice.say(sp2_METER);
  voice.say(spa__S);

  for (i = 0; i < 15; ++i) // Converts the value into an array of bits
  {
    array[i] = (apogeeValue >> i) & 1;
  }

  Serial.print("BitArray: ");
  for (i=0;i<15;i++) // Prints the bit array
  {
    Serial.print(array[i]);
  } 
  Serial.println();

  // Flashes the bits out on onboard LED short flash for 1 long flash for 0
  // Flashes in reverse order?
  
  for (i=0;i<15;i++) {
    digitalWrite(ledPin, HIGH);

    if (array[i]) {delay(250);} 
    else { delay(1000); }

    digitalWrite(ledPin, LOW);
    delay(500);
  }
  delay(3000);
}

void UpdateBuffer(){
  for (i=0;i<bufferLength;i++)
  {
    buffer[i]=buffer[i+1];
  }
  buffer[bufferLength-1] = altitude;

}

// Linear Regression / Least Squares
float Slope(){
  
  sumY = 0;
  sumXY = 0;

  for (i=bufferLength-5;i<bufferLength;i++){
    sumY += buffer[i];
  }

  for (i=bufferLength-5;i<bufferLength;i++) {
    sumXY += buffer[i]*(bufferLength-i);
  }

  float i = 5;
  m = ((i*sumXY) - (sumX*sumY)) / ((i*sumX2) - (sumX*sumX));
  return m;
}


void pntSerial(){
  
  Serial.print("Current State: ");
  Serial.print(currentState); 

  // Print which state to serial
  switch (currentState) { 
    case 0: Serial.println(" (StandBy)"); break;
    case 1: Serial.println(" (PoweredAscent)");break;
    case 2: Serial.println(" (Apogee)"); break;
    case 3: Serial.println(" (Slow) descent"); break;
    case 4: Serial.println(" (Recovery)"); break;
  }

  Serial.print("temperature:    "); Serial.println(bmp.getTemperature()); 
  Serial.print("altitude:       "); Serial.println(altitude); 
  Serial.print("altitudeBMP:    "); Serial.println(bmp.calAltitude(SEA_LEVEL_PRESSURE, pressure));
  Serial.print("Pressure:       "); Serial.println(pressure); 
  Serial.print("apogeeValue:    "); Serial.println(apogeeValue);
  Serial.print("Buffer: ");
  for (i=0;i<bufferLength;i++)
  {
    Serial.print(buffer[i]);
    Serial.print(" ");
  } 
  Serial.println(); 
  Serial.print("slope "); Serial.println(Slope());

  Serial.println("----------- END OF OUTPUT ------------");

}

// Converts a number to single digit base 10 and reads it out. (Max 999,999)
void NumberCallout(uint32_t inputNumber) {

  // Serial.println(int(inputNumber));
  // Serial.println(int(inputNumber)%1000000/10000);
  // Serial.println(int(inputNumber)%100000/10000);
  // Serial.println(int(inputNumber)%10000/1000);
  // Serial.println(int(inputNumber)%1000/100);
  // Serial.println(int(inputNumber)%100/10);
  // Serial.println(int(inputNumber)%10);

  numberString[1] = int(inputNumber)%1000000/10000; // 100,000
  numberString[2] = int(inputNumber)%100000/10000;  //  10,000
  numberString[3] = int(inputNumber)%10000/1000;    //   1,000
  numberString[4] = int(inputNumber)%1000/100;      //     100
  numberString[5] = int(inputNumber)%100/10;        //      10
  numberString[6] = int(inputNumber)%10;            //       1

  // Checking through numbers until is not zero
  leadingZeroFlag = false;
  for (i = 0; i < 7;) {
      if (numberString[i] != 0 || leadingZeroFlag == true) {
        leadingZeroFlag = true;
        SpeakNumber(numberString[i]);
      }
      i++;
  }
}

// Makes the speaker speak the number using Talkie library, Speaker needs to be on pins 3 and 11. I could probably changes this but lazy
void SpeakNumber(int number){
  switch (number) {
    case 0: voice.say(sp3_ZERO);  break;
    case 1: voice.say(sp3_ONE);   break;
    case 2: voice.say(sp3_TWO);   break;
    case 3: voice.say(sp3_THREE); break;
    case 4: voice.say(sp3_FOUR);  break;
    case 5: voice.say(sp3_FIVE);  break;
    case 6: voice.say(sp3_SIX);   break;
    case 7: voice.say(sp3_SEVEN); break;
    case 8: voice.say(sp3_EIGHT); break;
    case 9: voice.say(sp3_NINE);  break;
  }
}

uint32_t AltitudeCalculation(uint32_t pressure){
  return 44330 * (1.0f - pow(pressure / SEA_LEVEL_PRESSURE, 0.1903)); // 0.1903 is a double here, see how accurate as a float
}

void printLastOperateStatus(BMP::eStatus_t eStatus) // Not me
{
  switch(eStatus) {
  case BMP::eStatusOK:    Serial.println("everything ok"); break;
  case BMP::eStatusErr:   Serial.println("unknow error"); break;
  case BMP::eStatusErrDeviceNotDetected:    Serial.println("device not detected"); break;
  case BMP::eStatusErrParameter:    Serial.println("parameter error"); break;
  default: Serial.println("unknow status"); break;
  }
}