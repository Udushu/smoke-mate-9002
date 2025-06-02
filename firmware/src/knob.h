#ifndef KNOB_H
#define KNOB_H
#include <Arduino.h>
#include "types.h"
#include "encoder.h"
#include <ezButton.h>
// #define KNOB_DEBUG

#define KNOB_ULTRALONG_BUTTON_PRESS_TIME_MSEC 5000 // Time for ultra long button press in milliseconds

enum KNOB_STATE
{
    KNOB_STATE_IDLE = 0,
    KNOB_STATE_BUTTON_PRESSED,
    KNOB_STATE_SHORT_BUTTON_PRESSED,
    KNOB_STATE_LONG_BUTTON_PRESSED,
    KNOB_STATE_ULTRA_LONG_BUTTON_PRESSED,
    KNOB_STATE_ROTATING_UP,
    KNOB_STATE_ROTATING_DOWN
};

class Knob
{
private:
    Encoder m_encoder;
    ezButton m_button;

    ulong m_buttonPressedTimeMSec;

    KNOB_STATE m_state;

    ulong m_longPressTimeMSec;

public:
    Knob(int pinA, int pinB, int pinButton, ulong longPressTimeMSec = 1000, uint debounceTimeMSec = 50);
    void service(ulong currentTimeMSec);
    void reset();
    bool isShortButtonPressed();
    bool isLongButtonPressed();
    bool isUltraLongButtonPressed();
    bool isRotatingUp();
    bool isRotatingDown();
};

#endif