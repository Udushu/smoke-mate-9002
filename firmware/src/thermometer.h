#ifndef THERMOMETER_H
#define THERMOMETER_H

#include <Arduino.h>
#include <SPI.h>
#include "types.h"
#include "debug.h"

// #define THERMOMETER_DEBUG

class Thermometer
{
private:
    uint m_spiCSPin;
    uint m_spiCLKPin;
    uint m_spiSOPin;

    ulong m_lastReadTimeMsec;
    ulong m_intervalMSec;

    int m_rawTemperature;
    int m_temperatureC;
    int m_temperatureF;

    float m_gain;
    float m_offset;

    bool m_isNewTemperatureAvailable;
    bool m_isSimulated = false;

    void simulateTemperature();

public:
    Thermometer();
    Thermometer(ulong intervalMsec, uint spiCSPin, uint spiCLKPin, uint spiSOPin);
    void read();
    int getTemperatureC();
    int getTemperatureF();
    void service(ulong currentTimeMSec);
    void setInterval(ulong intervalMsec);
    ulong getInterval();
    bool isNewTemperatureAvailable();
    void setCalibration(float gain, float offset);
    void setSimulated(bool isSimulated);
};

#endif