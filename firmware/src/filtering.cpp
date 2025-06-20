#include "filtering.h"

Filter::Filter(FilterType type, float param, float initial)
    : m_type(type), m_param(param), m_value(initial), m_initialized(false) {}

void Filter::setType(FilterType type, float param)
{
    m_type = type;
    m_param = param;
    m_initialized = false;
}

float Filter::update(float sample)
{
    switch (m_type)
    {
    case FilterType::NONE:
        return updateNone(sample);
    case FilterType::EWMA:
        return updateEWMA(sample);
    // Add more filter types here as needed
    default:
        return sample;
    }
}

float Filter::value() const
{
    return m_value;
}

void Filter::reset(float initial)
{
    m_value = initial;
    m_initialized = false;
}

float Filter::updateNone(float sample)
{
    m_value = sample;
    m_initialized = true;
    return m_value;
}

float Filter::updateEWMA(float sample)
{
    if (!m_initialized)
    {
        m_value = sample;
        m_initialized = true;
    }
    else
    {
        m_value = m_param * sample + (1.0f - m_param) * m_value;
    }
    return m_value;
}