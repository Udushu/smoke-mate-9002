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
}

BangBangState BangBang::getState() const
{
    return m_state;
}