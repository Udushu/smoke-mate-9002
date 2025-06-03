#ifndef TEMPERATURE_CONTROLLER_H
#define TEMPERATURE_CONTROLLER_H

#include <Arduino.h>
#include "types.h"
#include "pid.h"
#include "bangbang.h"
#include "blower.h"
#include "door.h"

#define DEBUG_TEMPERATURE_CONTROLLER

enum ControlAlgorithm
{
    CONTROL_PID,
    CONTROL_BANGBANG
};

class TemperatureController
{
private:
    ControllerStatus &m_status;   // Reference to the controller status
    Configuration &m_config;      // Reference to the configuration
    Blower &m_blower;             // Reference to the blower motor
    Door &m_door;                 // Reference to the door servo
    ControlAlgorithm m_algorithm; // Current control algorithm
    PID m_pid;                    // PID controller instance
    BangBang m_bangBang;          // Bang-Bang controller instance
    ulong m_lastServiceTimeMSec;  // Last service time in milliseconds

    void servicePIDController(int currentTempF, ulong currentTimeMSec);
    void serviceBangBangController(int currentTempF, ulong currentTimeMSec);

public:
    TemperatureController(ControllerStatus &status, Configuration &config, Blower &blower, Door &door);
    void setControlAlgorithm(ControlAlgorithm algo);
    void service(int currentTempF, ulong currentTimeMSec);
};

#endif // TEMPERATURE_CONTROLLER_H
