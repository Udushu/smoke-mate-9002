
#ifndef FILTERING_H
#define FILTERING_H

/**
 * @file filtering.h
 * @brief Defines filtering utilities for signal processing.
 *
 * This header provides the Filter class, which supports different filtering algorithms,
 * including the Exponentially Weighted Moving Average (EWMA) filter.
 *
 * The EWMA filter is a simple and efficient method for smoothing noisy data.
 * It computes the filtered value as a weighted average of the current sample and the previous filtered value:
 *
 *     y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
 *
 * where:
 *   - y[n] is the filtered value at step n,
 *   - x[n] is the current input sample,
 *   - alpha is the smoothing factor (0 < alpha <= 1).
 *
 * A higher alpha discounts older observations faster, making the filter more responsive to recent changes.
 * The Filter class allows selecting the filter type and configuring its parameters.
 */

enum class FilterType
{
    NONE,
    EWMA
};

class Filter
{
public:
    Filter(FilterType type = FilterType::NONE, float param = 0.0f, float initial = 0.0f);

    float update(float sample);
    float value() const;
    void reset(float initial = 0.0f);
    void setType(FilterType type, float param = 0.0f);

private:
    FilterType m_type;
    float m_param; // Used for EWMA alpha, can be extended for other filters
    float m_value;
    bool m_initialized;

    float updateNone(float sample);
    float updateEWMA(float sample);
};

#endif // FILTERING_H