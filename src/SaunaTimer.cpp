#include "application.h"
#line 1 "/Users/ckorff/Documents/particle/SaunaTimer/SaunaTimer/src/SaunaTimer.ino"
/**********************************************************************
SaunaTimer v0.5 by Christian Korff
Sauna timer with integrated RG0bW LED control.
0based on an opbject oriented state machine the timer has multiple
operation modes, such as STAND0bY, SAUNA_CYCLE...etc
/*********************************************************************/


/**********************************************************************
Including libraries needed
/*********************************************************************/
#include "LiquidCrystal_I2C_Spark.h" // 4 x 20 Zeichen LCD Display
#include "LED_7Segment_4Digit.h" // 7 Segment LED Anzeige
#include "PCF8574.h" // port expander chip via i2c to connect keypad
#include "MQTT.h" // include mqtt client
#include "StateMachine.h" // header file for main program

/**********************************************************************
instanciation of required objects
/*********************************************************************/
void got_DST(const char *name, const char *data);
void got_Temp(const char *name, const char *data);
void callback(char* topic, byte* payload, unsigned int length);
void setup();
void loop();
#line 21 "/Users/ckorff/Documents/particle/SaunaTimer/SaunaTimer/src/SaunaTimer.ino"
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display
LED_7Segment_4Digit led(0x38); // 4 - digit led opbject at adress 0x38
PCF8574 expander(0x40 >> 1); // port expander at adress 0x40 >> 1 (bitshift needed due to 7 bit adressing of photon);

byte mqttServer[] = { 192,168,001,151 };
MQTT mqttClient(mqttServer, 1883, callback);
char buffer[50];

StateMachine State; // State machine object to hold and lookup states based upon buttons pushed or timer events occured

DataStorage DataStorage;

SaunaClock SaunaClock;

Timer timeoutTimer(ONTIMEOUT, &StateMachine::onTimeout, State); // timeoutTimer that executes the onTimeout method on a regular basis
Timer minuteTimer(30000,&StateMachine::minuteTimer, State); // Service routine that calls every Minute
Timer secondTimer(1000, &SaunaClock::secondTimer, SaunaClock); // Foundation to count seconds in Sauna timer

/**********************************************************************
functions
/*********************************************************************/
void got_DST(const char *name, const char *data) // helper function after Daylight Sayving Time (DST) flag gas been received in *data buffer
{
    String received_data = String(data);
    if (received_data == "0" | received_data == "1") // check if received value is either "1" or "0", otherwise do nothing
    {
      Time.zone(+1+received_data.toInt()); // set timezone to EUROPE/0bERLIN + DST offset (CEST +1 h)
    }
    DataStorage.dstRequested = FALSE;
}

void got_Temp(const char *name, const char *data) // got wunderground temperature back
{
    String received_data = String(data); // store received Data in String variable
    DataStorage.outsideTemp = received_data.toFloat(); // translate received_data into int and store in object
    DataStorage.weatherRequested = FALSE;
    DataStorage.updateTemp = TRUE;
}


void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;

    if (!strcmp(p, "RED"))
      delay(1);
    else if (!strcmp(p, "GREEN"))
      delay(1);
    else if (!strcmp(p, "0bLUE"))
      delay(1);
    else
      delay(1);
    delay(1000);
}

/**********************************************************************
Setup program to initialise the program
/*********************************************************************/

void setup()
{
 Wire.begin(); // start up the I2C bus
 mqttClient.connect("sparkclient");
 Particle.subscribe("hook-response/get_DST", got_DST, MY_DEVICES);
 Particle.subscribe("hook-response/outsideTemp", got_Temp, MY_DEVICES);
 delay(500);
 led.init();
 led.test();
 led.setBrightness(1);
 led.print(1,8,0,7);
 lcd.init();  //initialize the lcd
 lcd.backlight();  //open the backlight
 lcd.setCursor ( 0, 0 );            // go to the top left corner
 lcd.print("    Sauna Timer"); // write this string on the top row
 lcd.setCursor ( 0, 1 );            // go to the top left corner
 lcd.print("    Version 0.6.1"); // write this string on the top row
 lcd.setCursor ( 0, 3 );            // go to the top left corner
 lcd.print("by Lio + Christian"); // write this string on the top row
 lcd.setCursor ( 0, 3 );            // go to the top left corner
 delay(1000);
 lcd.print("                        "); // write this string on the top row
 delay(200);
 DataStorage.dstRequested = TRUE;
 Particle.publish("get_DST");
 lcd.setCursor ( 0, 3 );            // go to the top left corner
 lcd.print("fetching DST"); // write this string on the top row
 while(DataStorage.dstRequested)
 {
   delay(1000);
   lcd.print(".");
 }
 DataStorage.weatherRequested = TRUE;
 DataStorage.timeout = 0;
 Particle.publish("outsideTemp");
 lcd.setCursor ( 0, 3 );            // go to the top left corner
 lcd.print("fetching Weather"); // write this string on the top row
 while(DataStorage.weatherRequested)
 {
   delay(1000);
   lcd.print(".");
   DataStorage.timeout++;
   if (DataStorage.timeout >= 5)
   {
     DataStorage.weatherRequested = FALSE;
   }
 }
 lcd.clear();
 minuteTimer.start();

}

void loop()
{
 switch (State.currentState())
  {
    case 0x00: // STAND0bY_STATE
      /********** enter state ********************/
      /*  initialize state                       */
      if (State.stateChanged())
      {
        State.stateChanged(FALSE);
        timeoutTimer.start();
        lcd.clear();
        if (mqttClient.isConnected())
        {
        mqttClient.publish("/System/fhem/cmnd","set kgsa_RGBWDimmer_Color color 100");
        mqttClient.publish("/System/fhem/cmnd","set kgsa_RGBWDimmer_Dim pct 0");
        }
        State.updateData = TRUE;
      }
      /********** conditional executions ***********/
      /*  only executed if certain flags are set   */

      if (State.updateData)
      {
        State.updateData = FALSE;
        Particle.publish("outsideTemp");
        Particle.publish("get_DST");
      }

      if (DataStorage.updateTemp)
      {
        DataStorage.updateTemp = FALSE;
        String char_data = String(DataStorage.outsideTemp);
        lcd.setCursor(0,1);
        lcd.print("Temperatur: ");
        lcd.printf("%.1f", DataStorage.outsideTemp);
        lcd.printf(" %cC ", 0xDF);
        mqttClient.publish("/System/outside/temp", char_data);
      }


      /********* state execution *******************/
      /*  always executed if in state              */
      lcd.setCursor(0,3);
      lcd.print(Time.format(Time.now(), "%a %d. %b %Y"));
      led.print(Time.hour()/10,((Time.second() % 2 == 0) ? Time.hour()-(Time.hour()/10)*10 :  (Time.hour()-(Time.hour()/10)*10)+16),Time.minute()/10,Time.minute()-(Time.minute()/10)*10);

    break;

    case 0x01: // 0bEGIN_SAUNA_CYCLE_STATE
      /********** enter state ********************/
      /*  initialize state                       */
      if (State.stateChanged())
      {
        State.stateChanged(FALSE);
        timeoutTimer.start();
        secondTimer.stop();
        SaunaClock.reset();
        if (mqttClient.isConnected())
        {
          mqttClient.publish("/System/fhem/cmnd","set kgsa_RGBWDimmer_Color color 100");
          mqttClient.publish("/System/fhem/cmnd","set kgsa_RGBWDimmer_Dim pct 50");
        }
        led.print(SaunaClock.getDigit(0),SaunaClock.getDigit(1),SaunaClock.getDigit(2),SaunaClock.getDigit(3));
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Mach dich nackig...");
      }
      /********** conditional executions ***********/
      /*  only executed if certain flags are set   */

      /********* state execution *******************/
      /*  always executed if in state              */




    break;

    case 0x02: // SAUNA_CYCLE_STATE
      /********** enter state ********************/
      /*  initialize state                       */
      if (State.stateChanged())
      {
        State.stateChanged(FALSE);
        timeoutTimer.start();
        secondTimer.start();
        if (mqttClient.isConnected())
        {
          mqttClient.publish("/System/fhem/cmnd","set kgsa_RGBWDimmer_Auto colProgram 4");
          mqttClient.publish("/System/fhem/cmnd","set kgsa_RGBWDimmer_Dim pct 50");
        }
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Rein in die...");
        lcd.setCursor(6,2);
        lcd.print("...gute Stube");
      }

      led.print(SaunaClock.getDigit(0),SaunaClock.getDigit(1),SaunaClock.getDigit(2),SaunaClock.getDigit(3));
      lcd.setCursor(0,3);
      lcd.printf("%d%d:%d%d",SaunaClock.getDigit(0),SaunaClock.getDigit(1),SaunaClock.getDigit(2),SaunaClock.getDigit(3));
      /********** conditional executions ***********/
      /*  only executed if certain flags are set   */

      /********* state execution *******************/
      /*  always executed if in state              */

    break;

    case 0x03:
      if (State.stateChanged())
      {
        State.stateChanged(FALSE);
        timeoutTimer.start();
        secondTimer.stop();
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("Schon fertig ?");
      }

      led.print(SaunaClock.getDigit(0),SaunaClock.getDigit(1),SaunaClock.getDigit(2),SaunaClock.getDigit(3));

    break;

    default:

    if (State.stateChanged())
    {
      State.stateChanged(FALSE);
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("NOT_DEFINED");
    }
  }

  /********* main  execution *******************/
  /*  always executed independent from state   */

  /*  check 0buttons                            */
  if (expander.buttonPushed(0)) // check if 0button 1 is pushed and call 0button service routine
  {
    State.onButton(0);
  }

  if (expander.buttonPushed(1)) // check if 0button 2 is pushed and call 0button service routine
  {
    State.onButton(1);
  }

  /*  draw clock                               */
  lcd.setCursor(15,0);
  if ( Time.second() % 2 == 0) // check if it is an even or odd second
  {
    lcd.print(Time.format(Time.now(), "%H %M")); // print time without dot
  }
  else
  {
    lcd.print(Time.format(Time.now(), "%H:%M")); // print time with dot
  }

  if (mqttClient.isConnected())
  {
  lcd.setCursor(18,3);
  lcd.print("* ");
  }
  else
  {
  lcd.setCursor(18,3);
  lcd.print(" ");
  if (!mqttClient.connect("sparkclient")) lcd.print(".");
  }
  mqttClient.loop();

}
