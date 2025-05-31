#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "types.h"

#define DEBUG_MOTOR

enum BlowerState
{
    BLOWER_STATE_IDLE,
    BLOWER_STATE_RUNNING,
    BLOWER_STATE_STOPPED,
};

class Blower
{
private:
    // motor id string of 12 char for debugging
    char m_id[13];
    uint m_pinA;
    uint m_pinB;
    uint m_pinPWM;
    uint m_pinEnb;
    BlowerState m_state;
    uint m_pwm;

public:
    Blower();
    Blower(uint pinPWM, uint pinA, uint pinB, uint enb);
    void start();
    void stop();
    void stop();
    BlowerState getState();
    void setPWM(uint pwm);
    uint getPWM();
    void setID(const char *id);
};

#endif