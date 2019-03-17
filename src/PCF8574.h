
#include <Particle.h>
#ifndef _PCF8574_H
#define _PCF8574_H

class PCF8574
{
  public:
  PCF8574(int address);

  uint8_t read8();
  uint8_t read(uint8_t pin);
  uint8_t value();

  void write8(uint8_t value);
  void write(uint8_t pin, uint8_t value);

  void toggle(uint8_t pin);
  void shiftRight(uint8_t n=1);
  void shiftLeft(uint8_t n=1);
  bool buttonPushed(uint8_t pin);

  int lastError();

  private:
  int _address;
  uint8_t _data;
  int _error;
  double _lastTimevalidPush = millis();
  bool _oldState[8] = {true};
  bool _buttonPushed[8];
};

#endif
