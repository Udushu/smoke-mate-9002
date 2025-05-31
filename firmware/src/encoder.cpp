#include "encoder.h"

Encoder::Encoder()
{
    m_pinA = 0;
    m_pinB = 0;

    m_counter = 0;
    m_pinA_prev = 0;
    m_pinA_value = 0;
    m_isClockWise = false;
}

Encoder::Encoder(int pinA, int pinB)
{
    m_pinA = pinA;
    m_pinB = pinB;

    m_counter = 0;
    m_pinA_prev = 0;
    m_pinA_value = 0;
    m_isClockWise = false;
    m_timeDelta = ENCODER_DELTA_TIME_MSEC;

    pinMode(m_pinA, INPUT);
    pinMode(m_pinB, INPUT);
}

void Encoder::service(ulong currentTimeMSec)
{
    m_pinA_value = digitalRead(m_pinA);
    if (m_pinA_value != m_pinA_prev)
    { // check if knob is rotating
        // if pin A state changed before pin B, rotation is counter-clockwise
        if (digitalRead(m_pinB) != m_pinA_value)
        {
            m_isClockWise = false;
            m_counter--;
        }
        else
        {
            // if pin B state changed before pin A, rotation is clockwise

            m_counter++;
            m_isClockWise = true;
        }

#ifdef ENCODER_DEBUG
        if (m_isClockWise)
        {
            Serial.print("Clockwise | ");
        }
        else
        {
            Serial.print("Counter-Clockwise | ");
        }
        Serial.print(m_counter);
        Serial.print(" | ");
        Serial.println();
#endif
    }

    m_pinA_prev = m_pinA_value;

    if (currentTimeMSec - m_timerMSec >= m_timeDelta)
    {
        m_timerMSec = currentTimeMSec;
        m_pos1 = m_pos2;
        m_pos2 = m_counter;
        m_speed = 1000 * ((float)m_pos2 - (float)m_pos1) / (m_timeDelta);
    }
}

int Encoder::getCount()
{
    return m_counter;
}

void Encoder::reset()
{
    m_counter = 0;
    m_pinA_prev = 0;
    m_pinA_value = 0;
    m_isClockWise = false;
}

int Encoder::getSpeed()
{
    // Round the speed to the nearest integer, cast to int and return
    return (int)(m_speed + 0.5);
}

void Encoder::setTimeDelta(uint delta)
{
    m_timeDelta = delta;
}

uint Encoder::getTimeDelta()
{
    return m_timeDelta;
}