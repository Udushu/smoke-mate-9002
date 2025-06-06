#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

typedef unsigned long ulong;
typedef unsigned int uint;

struct RunningStatus
{
    bool isRunning;
    String uuid;
};

struct ControllerStatus
{
    bool isRunning;
    String uuid;
    ulong uptime;
    ulong controllerStartMSec;
    int temperatureSmoker;
    int temperatureFood;
    int temperatureTarget;
    int fanPWM;
    int doorPosition;
    int RSSI;
    int bars;
};

struct Configuration
{
    int temperatureTarget;
    int temperatureIntervalMSec;

    bool isPIDEnabled;
    float kP;
    float kI;
    float kD;

    int bangBangLowThreshold;
    int bangBangHighThreshold;
    int bangBangHysteresis;
    int bangBangFanSpeed;

    int doorOpenPosition;
    int doorClosePosition;

    float themometerSmokerGain;
    float themometerSmokerOffset;
    float themometerFoodGain;
    float themometerFoodOffset;

    bool isThemometerSimulated;

    bool isForcedFanPWM;
    int forcedFanPWM;

    bool isForcedDoorPosition;
    int forcedDoorPosition;

    char wifiSSID[65];     // WiFi SSID
    char wifiPassword[65]; // WiFi Password
};

#endif // TYPES_H