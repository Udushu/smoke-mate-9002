#ifndef BANGBANG_H
#define BANGBANG_H

#include <Arduino.h>
#include "types.h"
#include "debug.h"

// #define DEBUG_BANGBANG

enum BangBangState
{
    BANGBANG_STATE_IDLE,
    BANGBANG_STATE_HEAT,
    BANGBANG_STATE_COOL
};

class BangBang
{
private:
    int m_lowThreshold;    // Lower threshold (F)
    int m_highThreshold;   // Upper threshold (F)
    int m_hysteresis;      // Hysteresis (F)
    BangBangState m_state; // Current state

    void convertBangBangStateToString();

public:
    BangBang(int low, int high, int hyst);

    void setThresholds(int low, int high);
    void setHysteresis(int hyst);

    // Returns the current state (IDLE, HEAT, COOL)
    BangBangState service(int currentTemp, ulong currentTimeMSec);
    BangBangState getState() const;
};

#endif // BANGBANG_H