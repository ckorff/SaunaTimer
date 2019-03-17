// This is the headeer file for the LED I2C driver module based on a SAA1064 chip
#include <Wire.h>
#include "LED_7Segment_4Digit.h"

byte saa1064 = 0x70 >> 1; // define the I2C bus address for our SAA1064 (pin 1 to GND) ****
/* **** We bitshift the address as the SAA1064 doesn't have the address 0x70 (ADR pin
to GND) but 0x38 and Arduino uses 7-bit addresses- so 8-bit addresses have to
be shifted to the right one bit. Thanks to Malcolm Cox */

int digits[32]={63, 6, 91, 79, 102, 109, 125,7, 127, 111, 119, 124, 57, 94, 121, 113, 63+128, 6+128, 91+128, 79+128, 102+128, 109+128, 125+128,7+128, 127+128, 111+128, 119+128, 124+128, 57+128, 94+128, 121+128, 113+128};
// these are the byte representations of pins required to display each digit 0~9 then A~F
// Log message to cloud, message is a printf-formatted string

LED_7Segment_4Digit::LED_7Segment_4Digit(uint8_t led_Addr)
{
  _Addr = led_Addr;
  _Brightness = 0;
  _ControlByte = 0b00000111;
}

void LED_7Segment_4Digit::init()
// turns on dynamic mode and adjusts segment current to 12mA
{
 Wire.beginTransmission(_Addr);
 Wire.write(0b00000000); // this is the instruction byte. Zero means the next byte is the control byte
 Wire.write(_ControlByte); // control byte (dynamic mode on, digits 1+3 on, digits 2+4 on, 12mA segment current
 Wire.endTransmission();
}


void LED_7Segment_4Digit::test()
// show all digits 0~9, A~F on all digits of display
{
  for (int z=0; z <= 4; z++)
  {
    for (int i=0; i <= 7; i++)
    {
      Wire.beginTransmission(_Addr);
      Wire.write(1); // instruction byte - first digit to control is 1 (right hand side)
      Wire.write(digits[8])+128; // digit 1 (RHS)
      Wire.write(digits[8])+128; // digit 2
      Wire.write(digits[8])+128; // digit 3
      Wire.write(digits[8])+128; // digit 4 (LHS)
      Wire.endTransmission();
      delay(20);
      setBrightness(i);
    }

    for (int i=0; i <= 7; i++)
    {
      Wire.beginTransmission(_Addr);
      Wire.write(1); // instruction byte - first digit to control is 1 (right hand side)
      Wire.write(digits[8])+128; // digit 1 (RHS)
      Wire.write(digits[8])+128; // digit 2
      Wire.write(digits[8])+128; // digit 3
      Wire.write(digits[8])+128; // digit 4 (LHS)
      Wire.endTransmission();
      delay(20);
      decreaseBrightness();
    }
  }
  clear();
}

void LED_7Segment_4Digit::print(uint8_t digit0, uint8_t digit1, uint8_t digit2, uint8_t digit3)
{
  Wire.beginTransmission(_Addr);
  Wire.write(1); // instruction byte - first digit to control is 1 (right hand side)
  Wire.write(digits[digit0]); // digit 1 (RHS)
  Wire.write(digits[digit1]); // digit 2
  Wire.write(digits[digit2]); // digit 3
  Wire.write(digits[digit3]); // digit 4 (LHS)
  Wire.endTransmission();
}

void LED_7Segment_4Digit::setBrightness(uint8_t newBrightness)
{
  _ControlByte = (_ControlByte & 0b10001111)|(newBrightness << 4);
  _Brightness = newBrightness;
  init();
}

void LED_7Segment_4Digit::increaseBrightness()
{
  if (_Brightness < 7)
  {
    _Brightness++;
    _ControlByte = (_ControlByte & 0b10001111)|(_Brightness << 4);
    init();
  }
}

void LED_7Segment_4Digit::decreaseBrightness()
{
  if (_Brightness > 0)
  {
    _Brightness--;
    _ControlByte = (_ControlByte & 0b10001111)|(_Brightness << 4);
    init();
  }
}

void LED_7Segment_4Digit::clear()
// clears all digits
{
 Wire.beginTransmission(_Addr);
 Wire.write(1); // instruction byte - first digit to control is 1 (right hand side)
 Wire.write(0); // digit 1 (RHS)
 Wire.write(0); // digit 2
 Wire.write(0); // digit 3
 Wire.write(0); // digit 4 (LHS)
 Wire.endTransmission();
}
