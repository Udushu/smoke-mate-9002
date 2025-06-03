#include "temperaturecontroller.h"

TemperatureController::TemperatureController(ControllerStatus &status, Configuration &config, Blower &blower, Door &door)
    : m_status(status), m_config(config), m_blower(blower), m_door(door),
      m_algorithm(CONTROL_PID), m_pid(config.kP, config.kI, config.kD, config.temperatureIntervalMSec),
      m_bangBang(config.bangBangLowThreshold, config.bangBangHighThreshold, config.bangBangHysteresis)
{
    m_lastServiceTimeMSec = 0;
}

void TemperatureController::service(int currentTempF, ulong currentTimeMSec)
{

    // Check if the controller needs to be serviced
    if (currentTimeMSec - m_lastServiceTimeMSec < m_config.temperatureIntervalMSec)
    {
        return; // Not enough time has passed since the last service
    }
    m_lastServiceTimeMSec = currentTimeMSec;

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

        BangBangState controlOutput = m_bangBang.service(currentTempF, currentTimeMSec);
        if (controlOutput == BANGBANG_STATE_HEAT)
        {
            // If the state is HEAT, start the blower and open the door
            m_blower.setPWM(m_config.bangBangFanSpeed);
            m_blower.start();
            m_door.open();
        }
        else if (controlOutput == BANGBANG_STATE_COOL)
        {
            // If the state is COOL, stop the blower and close the door
            m_blower.stop();
            m_door.close();
        }
        else
        {
            // If the state is IDLE, stop the blower and set door position
            m_blower.stop();
            m_door.open();
        }

        break;
    default:
        break;
    }
}

void TemperatureController::servicePIDController(int currentTempF, ulong currentTimeMSec)
{
    // Update the PID controller settings if they have changed
    if (m_pid.getKp() != m_config.kP || m_pid.getKi() != m_config.kI || m_pid.getKd() != m_config.kD)
    {
        m_pid.setKp(m_config.kP);
        m_pid.setKi(m_config.kI);
        m_pid.setKd(m_config.kD);
    }

    // Call the PID service to calculate the control output
    int controlOutput = m_pid.service(currentTempF, m_config.temperatureTarget, currentTimeMSec);

    // Check whether the blower or the door should be activated
    if (controlOutput > 0)
    {
        // Truncate the control output to the range of 0-255
        controlOutput = constrain(controlOutput, 0, 255);
        m_blower.setPWM(controlOutput);
        m_blower.setPWM(controlOutput);
        m_blower.start();
        m_door.open();
    }
    else
    {
        // Make sure that the control output is not negative
        controlOutput = abs(controlOutput);
        controlOutput = constrain(controlOutput, 0, 255);
        controlOutput = map(controlOutput, 0, 255, m_config.doorClosePosition, m_config.doorOpenPosition);
        m_blower.stop();
        m_door.setPosition(controlOutput);
    }
}
