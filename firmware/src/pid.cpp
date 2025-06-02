#include "pid.h"

PID::PID(float kP, float kI, float kD, ulong updateInterval)
    : m_kP(kP), m_kI(kI), m_kD(kD),
      m_previousError(0.0f), m_integral(0.0f),
      m_lastTimeMsec(0), m_updateIntervalMsec(updateInterval),
      m_isEnabled(false),
      m_lastOutput(0)
{
}

void PID::setKp(float kP) { m_kP = kP; }
void PID::setKi(float kI) { m_kI = kI; }
void PID::setKd(float kD) { m_kD = kD; }

float PID::getKp() const { return m_kP; }
float PID::getKi() const { return m_kI; }
float PID::getKd() const { return m_kD; }

void PID::enable()
{
    m_isEnabled = true;
    m_integral = 0.0f;
    m_previousError = 0.0f;
    m_lastTimeMsec = 0;
}

void PID::disable()
{
    m_isEnabled = false;
    m_integral = 0.0f;
    m_previousError = 0.0f;
    m_lastTimeMsec = 0;
}

bool PID::isEnabled() const
{
    return m_isEnabled;
}

int PID::service(int currentTemp, int targetTemp, ulong currentTimeMSec)
{
    if (!m_isEnabled)
        return 0;

    // Only update at the specified interval
    if (m_lastTimeMsec != 0 && (currentTimeMSec - m_lastTimeMsec) < m_updateIntervalMsec)
        // If not enough time has passed, return the last output
        return m_lastOutput;

    float error = static_cast<float>(targetTemp - currentTemp);

    float deltaTime = 1.0f;
    if (m_lastTimeMsec != 0)
        deltaTime = (currentTimeMSec - m_lastTimeMsec) / 1000.0f; // seconds

    // Integral with anti-windup (clamp)
    m_integral += error * deltaTime;
    const float integralMax = 10000.0f;
    if (m_integral > integralMax)
        m_integral = integralMax;
    if (m_integral < -integralMax)
        m_integral = -integralMax;

    // Derivative
    float derivative = 0.0f;
    if (deltaTime > 0)
        derivative = (error - m_previousError) / deltaTime;

    // PID Output
    float output = m_kP * error + m_kI * m_integral + m_kD * derivative;

    // Save state for next calculation
    m_previousError = error;
    m_lastTimeMsec = currentTimeMSec;
    m_lastOutput = static_cast<int>(output);

    // Done!
    return m_lastOutput;
}