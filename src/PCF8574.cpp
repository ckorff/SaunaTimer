

#include "PCF8574.h"

#include <Wire.h>

PCF8574::PCF8574(int address)
{
  _address = address;
  Wire.begin();
}

uint8_t PCF8574::read8()
{
  Wire.beginTransmission(_address);
  Wire.requestFrom(_address, 1);
  _data = Wire.read();
  _error = Wire.endTransmission();
  return _data;
}

uint8_t PCF8574::value()
{
  return _data;
}

void PCF8574::write8(uint8_t value)
{
  Wire.beginTransmission(_address);
  _data = value;
  Wire.write(_data);
  _error = Wire.endTransmission();
}

uint8_t PCF8574::read(uint8_t pin)
{
  PCF8574::read8();
  return (_data & (1<<pin)) > 0;
}

void PCF8574::write(uint8_t pin, uint8_t value)
{
  PCF8574::read8();
  if (value == LOW)
  {
    _data &= ~(1<<pin);
  }
  else
  {
    _data |= (1<<pin);
  }
  PCF8574::write8(_data);
}

void PCF8574::toggle(uint8_t pin)
{
  PCF8574::read8();
  _data ^=  (1 << pin);
  PCF8574::write8(_data);
}

void PCF8574::shiftRight(uint8_t n)
{
  if (n == 0 || n > 7 ) return;
  PCF8574::read8();
  _data >>= n;
  PCF8574::write8(_data);
}

void PCF8574::shiftLeft(uint8_t n)
{
  if (n == 0 || n > 7) return;
  PCF8574::read8();
  _data <<= n;
  PCF8574::write8(_data);
}

bool PCF8574::buttonPushed(uint8_t pin)
{
  _buttonPushed[pin] = false;
  if (read(pin) != true)
  {
    if (_oldState[pin] == true)
    {
      _oldState[pin] = false;
      _lastTimevalidPush = millis();
      _buttonPushed[pin] = true;
    }
  }
  else
  {
    if ((millis() - _lastTimevalidPush) <= 50)
    {

    }
    else
    {
      _oldState[pin] = true;
    }
  }
  return _buttonPushed[pin];
}

int PCF8574::lastError()
{
  int e = _error;
  _error = 0;
  return e;
}
//
// END OF FILE
//
