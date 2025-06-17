#include "blower.h"

Blower::Blower() : m_pinPWM(0), m_pinA(0), m_pinB(0) {}

Blower::Blower(uint pinPWM, uint pinA, uint pinB, uint enb) : m_pinPWM(pinPWM),
                                                              m_pinA(pinA),
                                                              m_pinB(pinB),
                                                              m_pinEnb(enb)
{
    pinMode(m_pinPWM, OUTPUT);
    pinMode(m_pinA, OUTPUT);
    pinMode(m_pinB, OUTPUT);
    pinMode(m_pinEnb, OUTPUT);

    m_state = BLOWER_STATE_IDLE;
    m_pwm = 255;
}

void Blower::start()
{
    digitalWrite(m_pinEnb, HIGH); // Enable the motor
    analogWrite(m_pinPWM, m_pwm);
    digitalWrite(m_pinA, HIGH);
    digitalWrite(m_pinB, LOW);
    m_state = BLOWER_STATE_RUNNING;
}

void Blower::stop()
{
    digitalWrite(m_pinEnb, LOW); // Disable the motor
    analogWrite(m_pinPWM, m_pwm);
    digitalWrite(m_pinA, LOW);
    digitalWrite(m_pinB, HIGH);
    m_state = BLOWER_STATE_STOPPED;
}

BlowerState Blower::getState()
{
    return m_state;
}

void Blower::setPWM(uint pwm)
{

    pwm = constrain(pwm, BLOWER_MIN_PWM, BLOWER_MAX_PWM);
    m_pwm = pwm;
}

uint Blower::getPWM()
{
    return m_pwm;
}
