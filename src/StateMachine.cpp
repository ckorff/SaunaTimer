#include <Particle.h>
#include "StateMachine.h"
#include "MQTT.h"


/********** StateMachine ********************/
/*  initialize object                       */
StateMachine::StateMachine()
{
  _currentState = 0;
  _stateChanged = TRUE;
  _counter = 0;
  updateData = FALSE;
}

uint8_t StateMachine::currentState()
{
  return _currentState;
}

bool StateMachine::stateChanged()
{
  return _stateChanged;
}

bool StateMachine::stateChanged(bool value)
{
  _stateChanged = value;
  return _stateChanged;
}

void StateMachine::onButton(uint8_t triggerProperty)
{
  if (_currentState != _buttonTrigger[_currentState][triggerProperty])
  {
    _stateChanged = TRUE;
    _currentState = _buttonTrigger[_currentState][triggerProperty];
  }
}

void StateMachine::onTimeout()
{
  if (_currentState != _timerTrigger[_currentState])
  {
    _stateChanged = TRUE;
    _currentState = _timerTrigger[_currentState];
  }

}

void StateMachine::minuteTimer()
{
  if ( _counter < 20)
  {
    _counter++;
  }
  else
  {
    updateData = TRUE;
    _counter = 0;
  }
}

uint8_t StateMachine::debug()
{
  return _counter;
}

/********** DataStorage *********************/
/*  initialize object                       */
DataStorage::DataStorage()
{

  dstRequested = FALSE;
  updateTemp = FALSE;
  outsideTemp = 0;
}

/********** SaunaClock **********************/
/*  initialize object                       */
SaunaClock::SaunaClock()
{
  for (int i = 0; i < 4; i++)
  {
    _counter[i] = 0;
  }
}
void SaunaClock::secondTimer()
{
  if (_counter[3] == 0)
  {
    _counter[3] = 9;
    if (_counter[2] == 0)
    {
      _counter[2] = 5;
      if (_counter[1] == 0)
      {
        _counter[1] = 9;
        if (_counter[0] == 0)
        {_counter[0] = 5;}
        else
        {_counter[0]--;}

      }
      else
      {
        _counter[1]--;
      }
    }
    else
    {_counter[2]--;}
  }
  else
  {_counter[3]--;}
}
void SaunaClock::init()
{

}
void SaunaClock::reset()
{
  _counter[0] = 1;
  _counter[1] = 5;
  _counter[2] = 0;
  _counter[3] = 0;
}
void SaunaClock::start()
{

}
void SaunaClock::stop()
{

}
void SaunaClockonTimer()
{

}
uint8_t SaunaClock::getDigit(uint8_t digit)
{
  return _counter[digit];
}
