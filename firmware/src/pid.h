#ifndef PID_H
#define PID_H

#include <Arduino.h>
#include "types.h"

class PID
{
private:
    float m_kP; // Proportional gain
    float m_kI; // Integral gain
    float m_kD; // Derivative gain

    float m_previousError;      // Previous error for derivative calculation
    float m_integral;           // Integral of the error
    ulong m_lastTimeMsec;       // Last time the PID was updated
    ulong m_updateIntervalMsec; // Update interval in milliseconds
    bool m_isEnabled;           // PID enabled/disabled state
    int m_lastOutput;           // Last output value
public:
    PID(float kP, float kI, float kD, ulong updateInterval = 1000);

    // Setters for PID gains
    void setKp(float kP);
    void setKi(float kI);
    void setKd(float kD);

    // Getters for PID gains
    float getKp() const;
    float getKi() const;
    float getKd() const;

    // Enable/disable PID
    void enable();
    void disable();
    bool isEnabled() const;

    // Calculate the control output based on the current temperature and target temperature
    int service(int currentTemp, int targetTemp, ulong currentTimeMSec);
};

#endif // PID_H