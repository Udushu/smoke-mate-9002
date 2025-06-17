#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "types.h"

enum BlowerState
{
    BLOWER_STATE_IDLE,
    BLOWER_STATE_STARTING,
    BLOWER_STATE_RUNNING_NORMAL_SPEED,
    BLOWER_STATE_RUNNING_LOW_SPEED,
    BLOWER_STATE_STOPPING,
};

#define BLOWER_MIN_PWM 70
#define BLOWER_MAX_PWM 255
#define BLOWER_LOW_PWM_DUTY_CYCLE_MSEC 500 // Blower interval in milliseconds when running at low speed

class Blower
{
private:
    uint m_pinA;                 // Pin for controlling direction A
    uint m_pinB;                 // Pin for controlling direction B
    uint m_pinPWM;               // Pin for controlling PWM speed
    uint m_pinEnb;               // Pin for enabling the motor
    BlowerState m_state;         // Current state of the blower
    uint m_demandedPWM;          // Current demanded PWM value for the blower
    uint m_actualPWM;            // Actual PWM value being applied to the blower
    ulong m_blowerOnTimeMSec;    // Time for the blower to be active in millisconds at low speeds
    ulong m_currentTimeMsec;     // Current time in milliseconds
    ulong m_blowerStartTimeMsec; // Start time of the blower operation in milliseconds
    bool m_isLowSpeed;           // Flag to indicate if the blower is running at low speed

    void start();
    void stop();

public:
    Blower();
    Blower(uint pinPWM, uint pinA, uint pinB, uint enb);
    void service(ulong currentTimeMsec);
    BlowerState getState();
    void setPWM(uint pwm);
    uint getPWM();
};

#endif