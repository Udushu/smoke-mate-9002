#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "types.h"

enum BlowerState
{
    BLOWER_STATE_IDLE,
    BLOWER_STATE_RUNNING,
    BLOWER_STATE_STOPPED,
};

#define BLOWER_MIN_PWM 70
#define BLOWER_MAX_PWM 255

class Blower
{
private:
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
    // void stop();
    BlowerState getState();
    void setPWM(uint pwm);
    uint getPWM();
};

#endif