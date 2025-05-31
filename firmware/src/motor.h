#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "types.h"

#define DEBUG_MOTOR

enum MotorState
{
    MOTOR_STATE_IDLE,
    MOTOR_STATE_OPEN,
    MOTOR_STATE_CLOSE,
};

class Motor
{
private:
    // motor id string of 12 char for debugging
    char m_id[13];
    uint m_pinA;
    uint m_pinB;
    uint m_pinPWM;
    uint m_pinEnb;
    MotorState m_state;
    uint m_pwm;

public:
    Motor();
    Motor(uint pinPWM, uint pinA, uint pinB, uint enb);
    void open();
    void close();
    void stop();
    MotorState getState();
    void setPWM(uint pwm);
    uint getPWM();
    void setID(const char *id);
};

#endif