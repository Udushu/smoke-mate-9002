#include "temperaturecontroller.h"

TemperatureController::TemperatureController(ControllerStatus &status, Configuration &config, Blower &blower, Door &door)
    : m_status(status), m_config(config), m_blower(blower), m_door(door),
      m_algorithm(CONTROL_PID), m_pid(config.kP, config.kI, config.kD, config.temperatureIntervalMSec),
      m_bangBang(config.bangBangLowThreshold, config.bangBangHighThreshold, config.bangBangHysteresis)
{
    m_lastServiceTimeMSec = 0;
    m_pid.enable(); // Enable PID controller by default
    m_lastOutput = 0;
}

void TemperatureController::service(int currentTempF, ulong currentTimeMSec)
{

    BangBangState controlOutput;

#ifdef DEBUG_TEMPERATURE_CONTROLLER
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("TC::service() - Entry");
#endif

    // Check if the controller needs to be serviced
    if (currentTimeMSec - m_lastServiceTimeMSec < m_config.temperatureIntervalMSec)
    {
        return; // Not enough time has passed since the last service
    }
    m_lastServiceTimeMSec = currentTimeMSec;

    // Check for updated configuration values
    m_bangBang.setThresholds(m_config.bangBangLowThreshold, m_config.bangBangHighThreshold);
    m_bangBang.setHysteresis(m_config.bangBangHysteresis);
    m_pid.setKp(m_config.kP);
    m_pid.setKi(m_config.kI);
    m_pid.setKd(m_config.kD);

    // Figure out which control algorithm to use - may get more complicated later
    if (m_config.isPIDEnabled)
    {
        m_algorithm = CONTROL_PID;
    }
    else
    {
        m_algorithm = CONTROL_BANGBANG;
    }

    switch (m_algorithm)
    {
    case CONTROL_PID:

        servicePIDController(currentTempF, currentTimeMSec);
        break;

    case CONTROL_BANGBANG:

        serviceBangBangController(currentTempF, currentTimeMSec);

        break;

    default:
        break;
    }

#ifdef DEBUG_TEMPERATURE_CONTROLLER
    DEBUG_PRINTLN("TC::service() - Exit");
#endif
}

int TemperatureController::getLastOutput()
{
    return m_lastOutput;
}

void TemperatureController::serviceBangBangController(int currentTempF, ulong currentTimeMSec)
{
    BangBangState controlOutput = m_bangBang.service(currentTempF, currentTimeMSec);
    m_lastOutput = static_cast<int>(controlOutput); // Store the last output for reference
    switch (controlOutput)
    {
    case BANGBANG_STATE_IDLE:
        // If the state is IDLE, stop the blower and set door position
        m_blower.setPWM(0);
        m_blower.stop();
        m_door.open();
        break;
    case BANGBANG_STATE_HEAT:
        // If the state is HEAT, start the blower and open the door
        m_blower.setPWM(m_config.bangBangFanSpeed);
        m_blower.start();
        m_door.open();
        break;
    case BANGBANG_STATE_COOL:
        // If the state is COOL, stop the blower and close the door
        m_blower.setPWM(0);
        m_blower.stop();
        m_door.close();
        break;
    default:
        break;
    }
}

void TemperatureController::servicePIDController(int currentTempF, ulong currentTimeMSec)
{

    // Call the PID service to calculate the control output
    int controlOutput = m_pid.service(currentTempF, m_config.temperatureTarget, currentTimeMSec);
    m_lastOutput = controlOutput; // Store the last output for reference
#ifdef DEBUG_TEMPERATURE_CONTROLLER
    DEBUG_PRINTLN("TC::PID - CONTROL: " + String(controlOutput));
#endif

    // Check whether the blower or the door should be activated
    if (controlOutput > 0)
    {
#ifdef DEBUG_TEMPERATURE_CONTROLLER
        DEBUG_PRINTLN("TC::PID - HEATING");
#endif
        // Truncate the control output to the range of 0-255
        controlOutput = constrain(controlOutput, 0, 255);
        m_blower.setPWM(controlOutput);
        m_blower.start();
        m_door.open();
    }
    else
    {
#ifdef DEBUG_TEMPERATURE_CONTROLLER
        DEBUG_PRINTLN("TC::PID - COOLING");
#endif
        // Make sure that the control output is not negative
        // controlOutput = abs(controlOutput);
        // controlOutput = constrain(controlOutput, 0, 100);
        // controlOutput = map(controlOutput, 0, 100, m_config.doorOpenPosition, m_config.doorClosePosition);
        // controlOutput = constrain(controlOutput, m_config.doorClosePosition, m_config.doorOpenPosition);
        // m_blower.stop();
        // m_door.setPosition(controlOutput);
        m_blower.setPWM(0);
        m_blower.stop();
        m_door.close();
    }
}
