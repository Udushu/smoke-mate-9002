#include "bangbang.h"

BangBang::BangBang(int low, int high, int hyst)
    : m_lowThreshold(low), m_highThreshold(high), m_hysteresis(hyst), m_state(BANGBANG_STATE_IDLE)
{
}

void BangBang::setThresholds(int low, int high)
{
    m_lowThreshold = low;
    m_highThreshold = high;
}

void BangBang::setHysteresis(int hyst)
{
    m_hysteresis = hyst;
}

BangBangState BangBang::service(int currentTemp, ulong currentTimeMSec)
{
#ifdef DEBUG_BANGBANG
    DEBUG_PRINTLN("BangBang::service() - Enter");
    // Print the current state, thresholds, and temperature
    DEBUG_PRINT("---> Current State: ");
    convertBangBangStateToString();
    DEBUG_PRINT("---> Low Threshold: ");
    DEBUG_PRINTLN(m_lowThreshold);
    DEBUG_PRINT("---> High Threshold: ");
    DEBUG_PRINTLN(m_highThreshold);
    DEBUG_PRINT("---> Hysteresis: ");
    DEBUG_PRINTLN(m_hysteresis);
    DEBUG_PRINT("---> Current Temperature: ");
    DEBUG_PRINTLN(currentTemp);

#endif

    switch (m_state)
    {
    case BANGBANG_STATE_IDLE:
        if (currentTemp > m_highThreshold)
        {
            m_state = BANGBANG_STATE_COOL;
        }
        else if (currentTemp < m_lowThreshold)
        {
            m_state = BANGBANG_STATE_HEAT;
        }
        else
        {
            m_state = BANGBANG_STATE_IDLE;
        }
        break;

    case BANGBANG_STATE_HEAT:
        if (currentTemp >= (m_lowThreshold + m_hysteresis))
        {
            m_state = BANGBANG_STATE_IDLE; // Stop heating if above low threshold plus hysteresis
        }
        break;

    case BANGBANG_STATE_COOL:
        if (currentTemp <= (m_highThreshold - m_hysteresis))
        {
            m_state = BANGBANG_STATE_IDLE; // Stop cooling if below high threshold minus hysteresis
        }
        break;

    default:
        break;
    }

#ifdef DEBUG_BANGBANG
    DEBUG_PRINTLN("BangBang::service() - Exit");
#endif

    return m_state;
}

void BangBang::convertBangBangStateToString()
{
    switch (m_state)
    {
    case BANGBANG_STATE_IDLE:
        DEBUG_PRINTLN("IDLE");
        break;
    case BANGBANG_STATE_HEAT:
        DEBUG_PRINTLN("HEAT");
        break;
    case BANGBANG_STATE_COOL:

        DEBUG_PRINTLN("COOL");
        break;
    default:
        DEBUG_PRINTLN("UNKNOWN");
        break;
    }
}

BangBangState BangBang::getState() const
{
    return m_state;
}