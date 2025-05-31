#ifndef ENCODER_H
#define ENCODER_H

// #define ENCODER_DEBUG

#include <Arduino.h>
#include "types.h"

#define ENCODER_DELTA_TIME_MSEC 500

class Encoder
{

private:
    int m_pinA;
    int m_pinB;

    long m_counter;
    int m_pinA_prev;
    int m_pinA_value;
    boolean m_isClockWise;

    int m_pinAVal;

    ulong m_timerMSec;
    uint m_timeDelta;
    long m_pos1;
    long m_pos2;
    float m_speed;

public:
    Encoder();
    Encoder(int pinA, int pinB);
    void service(ulong currentTimeMSec);
    int getCount();
    void reset();
    int getSpeed();
    void setTimeDelta(uint delta);
    uint getTimeDelta();
};

#endif