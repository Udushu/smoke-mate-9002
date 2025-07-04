#include "blower.h"

const char *blowerStateToString(BlowerState state)
{
    switch (state)
    {
    case BLOWER_STATE_IDLE:
        return "IDLE";
    case BLOWER_STATE_STARTING:
        return "STARTING";
    case BLOWER_STATE_RUNNING_NORMAL_SPEED:
        return "RUNNING_NS";
    case BLOWER_STATE_RUNNING_LOW_SPEED:
        return "RUNNING_LS";
    case BLOWER_STATE_STOPPING:
        return "STOPPING";
    default:
        return "UNKNOWN";
    }
}

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
    m_prevState = BLOWER_STATE_IDLE; // Initialize previous state to idle
    m_demandedPWM = 255;
    m_blowerOnTimeMSec = 0;
    m_isLowSpeed = false;
}

void Blower::service(ulong currentTimeMsec)
{
#ifdef DEBUG_BLOWER
    String debugStr;
#endif

#ifdef DEBUG_BLOWER

    if (m_state != m_prevState)
    {
        debugStr = "Blower State: ";
        DEBUG_PRINTLN(debugStr);
        debugStr = "  ";
        debugStr += String(blowerStateToString(m_prevState));
        debugStr += "->";
        debugStr += String(blowerStateToString(m_state));
        DEBUG_PRINTLN(debugStr);
    }
#endif

    m_currentTimeMsec = currentTimeMsec;

    switch (m_state)
    {
    case BLOWER_STATE_IDLE:
        // Do nothing, waiting for start command
        break;

    case BLOWER_STATE_STARTING:
        start(); // Start the blower motor
        // Set the state based on the speed
        m_state = m_isLowSpeed ? BLOWER_STATE_RUNNING_LOW_SPEED : BLOWER_STATE_RUNNING_NORMAL_SPEED;
        m_blowerStartTimeMsec = m_currentTimeMsec; // Record the start time of the blower operation
        break;

    case BLOWER_STATE_RUNNING_NORMAL_SPEED:
        // Do nothing special while running at normal speed
        break;

    case BLOWER_STATE_RUNNING_LOW_SPEED:

        // Check if the blower has been running for the defined low speed active time
        if (m_currentTimeMsec - m_blowerStartTimeMsec >= m_blowerOnTimeMSec)
        {
            // If the blower has been running for the defined time, stop it
            stop();
        }

        if (m_currentTimeMsec - m_blowerStartTimeMsec >= BLOWER_LOW_PWM_DUTY_CYCLE_MSEC)
        {
            // If the blower has been running for the low speed duty cycle time, restart it
            m_state = BLOWER_STATE_STARTING; // Set the state to starting to restart the blower
#ifdef DEBUG_BLOWER
            debugStr = "Blower low speed restart";
            DEBUG_PRINTLN(debugStr);
#endif
        }

        break;

    case BLOWER_STATE_STOPPING:
        stop();                      // Stop the blower motor
        m_state = BLOWER_STATE_IDLE; // Set the state back to idle after stopping
        break;

    default:
        // Handle unexpected state
        Serial.println("Blower in unknown state!");
        break;
    }

    m_prevState = m_state; // Update the previous state to the current state
}

void Blower::start()
{
    digitalWrite(m_pinEnb, HIGH); // Enable the motor
    analogWrite(m_pinPWM, m_actualPWM);
    digitalWrite(m_pinA, HIGH);
    digitalWrite(m_pinB, LOW);
}

void Blower::stop()
{
    digitalWrite(m_pinEnb, LOW); // Disable the motor
    analogWrite(m_pinPWM, 0);
    digitalWrite(m_pinA, LOW);
    digitalWrite(m_pinB, LOW);
}

BlowerState Blower::getState()
{
    return m_state;
}

void Blower::setPWM(uint pwm)
{

    // Check if the PWM value is the same as the current demanded PWM
    if (pwm == m_demandedPWM)
    {
#ifdef DEBUG_BLOWER
        DEBUG_PRINTLN("setPWM: NO CHANGE");
#endif
        // If the PWM value is the same, do nothing
        return;
    }

    // Check the PWM valud is zero, stop the blower if it is
    if (pwm == 0)
    {
#ifdef DEBUG_BLOWER
        DEBUG_PRINTLN("setPWM: STOPPING");
#endif
        m_state = BLOWER_STATE_STOPPING;
        m_demandedPWM = 0; // Set the demanded PWM to zero
        m_actualPWM = 0;   // Set the actual PWM to zero
        return;
    }

    // Update the demanded PWM value
    m_demandedPWM = pwm;

    // Constrain the PWM value to the defined limits
    m_actualPWM = constrain(pwm, BLOWER_MIN_PWM, BLOWER_MAX_PWM);

    // Calculate the actuve time in milliseconds for the low speed mode
    if (m_demandedPWM < BLOWER_MIN_PWM)
    {
        // Set the blower to low speed mode
        m_isLowSpeed = true;
        // Calculate what fraction of the low speed PWM is requested
        float requestedFraction = static_cast<float>(m_demandedPWM) / BLOWER_MIN_PWM;
        // Calculate the active time in milliseconds based on the requested fraction
        m_blowerOnTimeMSec = static_cast<ulong>(BLOWER_LOW_PWM_DUTY_CYCLE_MSEC * requestedFraction);
    }
    else
    {
        // Set the blower to normal speed mode
        m_isLowSpeed = false;
        m_blowerOnTimeMSec = 0; // No active time in normal speed mode
    }
#ifdef DEBUG_BLOWER
    String debugStr = "setPWM: ";
    debugStr += String(m_demandedPWM);
    DEBUG_PRINTLN(debugStr);
    debugStr = "blowerOnTimeMSec: ";
    debugStr += String(m_blowerOnTimeMSec);
    DEBUG_PRINTLN(debugStr);
#endif

    // Set the state to starting
    m_state = BLOWER_STATE_STARTING;
}

uint Blower::getPWM()
{
    // Return the current demanded PWM value
    return m_demandedPWM;
}
