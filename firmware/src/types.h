#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

typedef unsigned long ulong;
typedef unsigned int uint;

enum TempProfileType
{
    TEMP_PROFILE_TYPE_DWELL,
    TEMP_PROFILE_TYPE_RAMP
};

struct TempProfileStep
{
    ulong timeMSec;        // Time in milliseconds for this step
    TempProfileType type;  // Type of the step (dwell or ramp)
    int temperatureStartF; // Temperature in degrees F for this step
    int temperatureEndF;   // Temperature in degrees F for this step
};

#define MAX_PROFILE_STEPS 10

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
    String ipAddress;
    bool isWiFiConnected;
    String networkName;
    int temperatureError;
    int isProfileRunning;                       // 0 - not running, 1 - running, 2 - finished
    int temperatureProfileStepIndex;            // Current step index in the temperature profile, -1 means no active profile
    int temperatureProfileStartTimeMSec;        // Start time of the current temperature profile step
    int temperatureProfileStepsCount;           // Number of steps in the temperature profile
    TempProfileType temperatureProfileStepType; // Type of the current temperature profile step
};

struct Configuration
{
    int temperatureTarget;
    int temperatureIntervalMSec;

    bool isTemperatureProfilingEnabled;
    TempProfileStep temperatureProfile[MAX_PROFILE_STEPS];
    int temperatureProfileStepsCount;

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

    bool isWiFiEnabled;    // Flag to indicate if WiFi is enabled
    char wifiSSID[65];     // WiFi SSID
    char wifiPassword[65]; // WiFi Password

    bool isTemperatureFilterEnabled; // Flag to indicate if the temperature filter is enabled
    float temperatureFilterCoeff;    // Coefficient for the temperature filter (0.0 - 1.0)

    // Add MQTT configuration
    bool isMqttEnabled;       // Enable/disable MQTT
    char mqttBroker[65];      // MQTT broker hostname/IP
    int mqttPort;             // MQTT broker port (default 1883)
    char mqttUsername[65];    // MQTT username (optional)
    char mqttPassword[65];    // MQTT password (optional)
    char mqttClientId[33];    // MQTT client ID
    char mqttTopicPrefix[65]; // Base topic prefix (e.g., "smoker/kitchen")
    int mqttPublishInterval;  // Publish interval in seconds
    bool isMqttRetain;        // Retain published messages
};

#endif // TYPES_H