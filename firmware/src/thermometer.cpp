#include "thermometer.h"

Thermometer::Thermometer()
{
    m_rawTemperature = 0;
    m_temperatureC = 0;
    m_temperatureF = 0;

    m_gain = 1.0;
    m_offset = 0.0;
}

Thermometer::Thermometer(ulong intervalMSec, uint spiCSPin, uint spiCLKPin, uint spiSOPin) : Thermometer()
{
    m_spiCSPin = spiCSPin;
    m_spiCLKPin = spiCLKPin;
    m_spiSOPin = spiSOPin;

    m_intervalMSec = intervalMSec;
    m_lastReadTimeMsec = 0;

    pinMode(m_spiCSPin, OUTPUT);
    pinMode(m_spiCLKPin, OUTPUT);
    pinMode(m_spiSOPin, INPUT);

    digitalWrite(m_spiCSPin, HIGH);
    digitalWrite(m_spiCLKPin, LOW);
    m_rawTemperature = 0;
    m_temperatureC = 0;
    m_temperatureF = 0;

    m_gain = 1.0;
    m_offset = 0.0;
}

void Thermometer::service(ulong currentTimeMSec)
{
    if (currentTimeMSec - m_lastReadTimeMsec >= m_intervalMSec || m_lastReadTimeMsec == 0)
    {
        m_lastReadTimeMsec = currentTimeMSec;
        read();
    }
}

void Thermometer::read()
{
#ifdef THERMOMETER_DEBUG

    DEBUG_PRINTLN();
    DEBUG_PRINTLN("Thermometer Read Start...");
#endif
    // Select the MAX6675 by pulling CS low
    digitalWrite(m_spiCSPin, LOW);
    delay(1);

    // Read the raw temperature value from MAX6675
    for (int i = 11; i >= 0; i--)
    {
        digitalWrite(m_spiCLKPin, HIGH);
        delayMicroseconds(1);
        digitalWrite(m_spiCLKPin, LOW);
        delayMicroseconds(1);

        bitWrite(m_rawTemperature, i, digitalRead(m_spiSOPin));
    }

    // Deselect the MAX6675 by pulling CS high
    digitalWrite(m_spiCSPin, HIGH);

    // Convert the raw temperature to degrees Celsius
    m_temperatureC = static_cast<int>(round(m_rawTemperature * 0.25));

    // Convert the raw temperature to degrees Farenhite (0°C × 9/5) + 32 = 32°F
    m_temperatureF = static_cast<int>(m_gain * (round((m_rawTemperature * 0.25) * (9.0 / 5.0) + 32)) + m_offset);

#ifdef THERMOMETER_DEBUG
    DEBUG_PRINTLN("Temperature: ");
    DEBUG_PRINTLN(m_temperatureF);
    DEBUG_PRINTLN("F ");
#endif

#ifdef THERMOMETER_SIMULATE
    // Simulate temperature for testing purposes
    simulateTemperature();
#endif

    m_isNewTemperatureAvailable = true;
}

int Thermometer::getTemperatureC()
{
    return m_temperatureC;
}

int Thermometer::getTemperatureF()
{
    return m_temperatureF;
}

void Thermometer::setInterval(ulong intervalMSec)
{
    m_intervalMSec = intervalMSec;
}

ulong Thermometer::getInterval()
{
    return m_intervalMSec;
}

bool Thermometer::isNewTemperatureAvailable()
{
    if (m_isNewTemperatureAvailable)
    {
        m_isNewTemperatureAvailable = false;
        return true;
    }
    else
    {
        return false;
    }
}

void Thermometer::setCalibration(float gain, float offset)
{
    m_gain = gain;
    m_offset = offset;
}

void Thermometer::simulateTemperature()
{
    // simulate raw temperature value such that it converts to 225F (107.2C)
    m_rawTemperature = 425; // 900 * 0.25 = 225F
    // Add simusoidal oscillation to the raw temperature with amplude such that it converts to 50F (10C) and period of 10 seconds
    m_rawTemperature += static_cast<int>(100 * sin((millis() / 200000.0) * TWO_PI)); // 50F = 10C
    // Convert the raw temperature to degrees Celsius
    m_temperatureC = static_cast<int>(round(m_rawTemperature * 0.25));
    // Convert the raw temperature to degrees Farenhite (0°C × 9/5) + 32 = 32°F
    m_temperatureF = static_cast<int>(m_gain * (round((m_rawTemperature * 0.25) * (9.0 / 5.0) + 32)) + m_offset);
}