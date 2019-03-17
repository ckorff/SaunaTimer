#ifndef LED_7Segment_4Digit_h
#define LED_7Segment_4Digit_h

// This is the headeer file for the LED I2C driver module based on a SAA1064 chip

#define SAA1064_BRIGHTNESS 0x07 // Define LED_Brightness in 3mA stepps from 0A to 21mA
#define SAA1064_STATE 1 // 1 = Display ON; 0 = Display OFF
#define SAA1064_MODE 1 // 0 = static, only digit 1 & 2 will be displayed; 1 = dynamic, all 4 digits will be displayed

class LED_7Segment_4Digit
{
public:
    LED_7Segment_4Digit(uint8_t led_Addr);
    void init();
    void test();
    void print(uint8_t digit0, uint8_t digit1, uint8_t digit2, uint8_t digit3);
    void setBrightness(uint8_t);
    void increaseBrightness();
    void decreaseBrightness();
    void clear();
private:
    uint8_t _Addr;
    uint8_t _Brightness;
    uint8_t _ControlByte;
};

#endif
