#ifndef _StateMachine_H
#define _StateMachine_H

#define STANDBY_STATE 0
#define BEGIN_SAUNA_CYCLE_STATE 1
#define SAUNA_CYCLE_STATE 2
#define END_SAUNA_STATE 3

#define BUTTON_0 0 // Button 1 at the expander board
#define BUTTON_1 1 // Button 2 at the expander board

#define ONTIMEOUT 30000 // Defines the cycle time for the onTimeout service routine 30000 = 30 seconds

class StateMachine
{
  public:
  StateMachine();
  uint8_t currentState();
  uint8_t debug();
  void onButton(uint8_t);
  void onTimeout();
  void minuteTimer();
  bool stateChanged();
  bool stateChanged(bool);
  //uint8_t DST; // holding the variable if DST is set or not
  //int8_t outsideTemp; // holding the outside temperature, must be signed int, because temperature could be negative
  bool updateData; // bool wariable to track if new temperaturehas been derived from the cloud

  private:
  uint8_t _currentState; // protected value of current state, can only be changed via functions
  bool _stateChanged; // protected value of _stateChanged, can only be changed by the objects
  uint8_t _counter; // counter to update Temp alle 10 Minuten

  const uint8_t _buttonTrigger[4][2] = {{ BEGIN_SAUNA_CYCLE_STATE, STANDBY_STATE }, // action map for STANDBY_STATE (state 0), what happens after BUTTON_0 -> BEGIN_SAUNA_CYCLE_STATE or BUTTON_1 -> STANDBY_STATE
                                        { SAUNA_CYCLE_STATE,       STANDBY_STATE },
                                        { END_SAUNA_STATE,         BEGIN_SAUNA_CYCLE_STATE },
                                        { STANDBY_STATE,           SAUNA_CYCLE_STATE }};

  const uint8_t _timerTrigger[4] = { STANDBY_STATE, // action map for timerTrigger, what is the desired state after timeout occured? e.g. _currentState = STANDBY_STATE -> STANDBY_STATE
                                     STANDBY_STATE,
                                     SAUNA_CYCLE_STATE,
                                     STANDBY_STATE };
};

class DataStorage
{
  public:
  DataStorage();
  bool dstRequested;
  bool weatherRequested;
  bool updateTemp;
  int currentSecond; //needed to recognize change in seconds
  int timeout;
  float outsideTemp;
  float saunaTemp;
  float saunaHumidity;

  private:
};

class SaunaClock
{
  public:
  SaunaClock();
  void secondTimer();
  void init();
  void reset();
  void start();
  void stop();
  void onTimer();
  uint8_t getDigit(uint8_t);

  private:
  uint8_t _counter[4];
  bool _running;
};

#endif
