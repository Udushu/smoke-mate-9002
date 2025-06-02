#include "knob.h"

Knob::Knob(int pinA, int pinB, int pinButton, ulong longPressTimeMSec, uint debounceTimeMSec)
    : m_encoder(pinA, pinB), m_button(pinButton)
{
    m_button.setDebounceTime(debounceTimeMSec);

    m_buttonPressedTimeMSec = 0;
    m_state = KNOB_STATE_IDLE;

    m_longPressTimeMSec = longPressTimeMSec;
}

void Knob::service(ulong currentTimeMSec)
{
    m_encoder.service(currentTimeMSec);
    m_button.loop(); // MUST call the loop() function first

    switch (m_state)
    {
    case KNOB_STATE_IDLE:

        // Check if the button is pressed
        if (m_button.isPressed())
        {
            m_buttonPressedTimeMSec = currentTimeMSec;
            m_state = KNOB_STATE_BUTTON_PRESSED;
            break;
        }
        // Check if the encoder is rotating
        if (m_encoder.getCount() > 1)
        {
            m_state = KNOB_STATE_ROTATING_UP;
            break;
        }
        else if (m_encoder.getCount() < -1)
        {
            m_state = KNOB_STATE_ROTATING_DOWN;
            break;
        }
        else
        {
            // No action, stay in idle state
        }
        break;

    case KNOB_STATE_BUTTON_PRESSED:
        // Check if the button is still pressed
        if (m_button.isReleased())
        {
            // Check for long press
            if (currentTimeMSec - m_buttonPressedTimeMSec >= m_longPressTimeMSec &&
                currentTimeMSec - m_buttonPressedTimeMSec < KNOB_ULTRALONG_BUTTON_PRESS_TIME_MSEC)
            {
                m_state = KNOB_STATE_LONG_BUTTON_PRESSED;
            }
            else if (currentTimeMSec - m_buttonPressedTimeMSec >= KNOB_ULTRALONG_BUTTON_PRESS_TIME_MSEC)
            {
                m_state = KNOB_STATE_ULTRA_LONG_BUTTON_PRESSED;
            }
            else
            {
                m_state = KNOB_STATE_SHORT_BUTTON_PRESSED;
            }
        }
        break;

    case KNOB_STATE_SHORT_BUTTON_PRESSED:

        // Do nothing, just wait for the reset
        break;

    case KNOB_STATE_LONG_BUTTON_PRESSED:

        // Do nothing, just wait for the reset
        break;

    case KNOB_STATE_ULTRA_LONG_BUTTON_PRESSED:
        // Do nothing, just wait for the reset
        break;

    case KNOB_STATE_ROTATING_UP:

        // Do nothing, just wait for the reset
        break;

    case KNOB_STATE_ROTATING_DOWN:
        // Do nothing, just wait for the reset
        break;
    }
}

void Knob::reset()
{
    m_state = KNOB_STATE_IDLE;
    m_button.resetCount();
    m_encoder.reset();
}

bool Knob::isShortButtonPressed()
{
    if (m_state == KNOB_STATE_SHORT_BUTTON_PRESSED)
    {
        reset();
        return true;
    }
    return false;
}

bool Knob::isLongButtonPressed()
{
    if (m_state == KNOB_STATE_LONG_BUTTON_PRESSED)
    {
        reset();
        return true;
    }
    return false;
}

bool Knob::isUltraLongButtonPressed()
{
    if (m_state == KNOB_STATE_ULTRA_LONG_BUTTON_PRESSED)
    {
        reset();
        return true;
    }
    return false;
}

bool Knob::isRotatingUp()
{
    if (m_state == KNOB_STATE_ROTATING_UP)
    {
        reset();
        return true;
    }
    return false;
}

bool Knob::isRotatingDown()
{
    if (m_state == KNOB_STATE_ROTATING_DOWN)
    {
        reset();
        return true;
    }
    return false;
}