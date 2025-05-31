#include "motor.h"

Motor::Motor() : m_pinPWM(0), m_pinA(0), m_pinB(0) {}

Motor::Motor(uint pinPWM, uint pinA, uint pinB, uint enb) : m_pinPWM(pinPWM),
                                                            m_pinA(pinA),
                                                            m_pinB(pinB),
                                                            m_pinEnb(enb)
{
    pinMode(m_pinPWM, OUTPUT);
    pinMode(m_pinA, OUTPUT);
    pinMode(m_pinB, OUTPUT);
    pinMode(m_pinEnb, OUTPUT);

    m_state = MOTOR_STATE_IDLE;
    m_pwm = 255;

    // initialize the motor id with string of spaces
    for (int i = 0; i < 12; i++)
    {
        m_id[i] = ' ';
    }
}

void Motor::open()
{
#ifdef DEBUG_MOTOR
    // Print motor id string, function name and pwm value
    Serial.print(m_id);
    Serial.print(" Motor::open() -> PWM:");
    Serial.println(m_pwm);
#endif
    digitalWrite(m_pinEnb, HIGH); // Enable the motor
    analogWrite(m_pinPWM, m_pwm);
    digitalWrite(m_pinA, HIGH);
    digitalWrite(m_pinB, LOW);
    m_state = MOTOR_STATE_OPEN;
}

void Motor::close()
{
#ifdef DEBUG_MOTOR
    // Print motor id string, function name and pwm value
    Serial.print(m_id);
    Serial.print(" Motor::close() -> PWM:");
    Serial.println(m_pwm);
#endif
    digitalWrite(m_pinEnb, LOW); // Enable the motor
    analogWrite(m_pinPWM, m_pwm);
    digitalWrite(m_pinA, LOW);
    digitalWrite(m_pinB, HIGH);
    m_state = MOTOR_STATE_CLOSE;
}

void Motor::stop()
{
#ifdef DEBUG_MOTOR
    // Print motor id string and function name
    Serial.print(m_id);
    Serial.println(" Motor::stop()");
#endif
    analogWrite(m_pinPWM, 0);
    digitalWrite(m_pinA, LOW);
    digitalWrite(m_pinB, LOW);
    m_state = MOTOR_STATE_IDLE;
}

MotorState Motor::getState()
{
    return m_state;
}

void Motor::setPWM(uint pwm)
{
#ifdef DEBUG_MOTOR
    Serial.print(m_id);
    Serial.print(" Motor::setPWM() -> PWM:");
    Serial.println(pwm);
#endif
    m_pwm = pwm;
}

uint Motor::getPWM()
{
    return m_pwm;
}

void Motor::setID(const char *id)
{
    strncpy(m_id, id, 12);
    m_id[12] = '\0';
}