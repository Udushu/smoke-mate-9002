#include "door.h"

Door::Door(uint8_t pin, uint closedPos, uint openPos, ulong stepMoveTimeMSec)
    : m_pin(pin),
      m_closedPosition(closedPos),
      m_openPosition(openPos),
      m_currentPosition(0),
      m_demandedPosition(0),
      m_state(DOOR_IDLE),
      m_stepMoveTimeMSec(stepMoveTimeMSec) {}

void Door::begin()
{
    m_servo.attach(m_pin, 600, 2300); // Attach servo with min and max pulse width
    m_servo.write(0);
    m_state = DOOR_STOPPED; // Start in stopped state
    close();                // Default to closed
}

void Door::service(ulong currentTimeMSec)
{
    switch (m_state)
    {
    case DOOR_IDLE:
#ifdef DOOR_DEBUG
        DEBUG_PRINTLN("Door is idle");
#endif
        break;
    case DOOR_STOPPED:
        if (m_currentPosition != m_demandedPosition)
        {
            m_state = DOOR_MOVING;
            m_startMoveTimeMSec = currentTimeMSec;
            setPosition(m_demandedPosition);
        }
        break;
    case DOOR_MOVING:
        if (currentTimeMSec - m_startMoveTimeMSec >= m_traveTimeMsec)
        {
            // Movement is complete
            m_currentPosition = m_demandedPosition; // Update current position
            m_state = DOOR_STOPPED;                 // Set state to stopped
        }
        break;

    default:
        break;
    }
}

void Door::setPosition(uint pos)
{
    // Make sure that the door is not moving
    if (m_state == DOOR_MOVING)
    {
#ifdef DOOR_DEBUG
        DEBUG_PRINTLN("Door is moving, cannot set position");
#endif
        return;
    }

    // Make sure that the positoin is within the allowed range
    if (pos < m_closedPosition || pos > m_openPosition)
    {
#ifdef DOOR_DEBUG
        DEBUG_PRINTLN("Position out of range, must be between " + String(m_closedPosition) + " and " + String(m_openPosition));
#endif
        return;
    }

    // Make sure that the position is not the same as the current position
    if (pos == m_currentPosition)
    {
#ifdef DOOR_DEBUG
        DEBUG_PRINTLN("Position is already set to " + String(pos));
#endif
        return;
    }

    // Calcuate the travel time based on the step move time and the distance to travel
    if (m_currentPosition > m_demandedPosition)
    {
        m_traveTimeMsec = (m_currentPosition - pos) * m_stepMoveTimeMSec;
    }
    else
    {
        m_traveTimeMsec = (pos - m_currentPosition) * m_stepMoveTimeMSec;
    }

    m_demandedPosition = pos;
}

uint Door::getPosition()
{
    return m_currentPosition;
}

void Door::open()
{
    setPosition(m_openPosition);
}

void Door::close()
{
    setPosition(m_closedPosition);
}

bool Door::isMoving()
{
    return m_state == DOOR_MOVING;
}

bool Door::isOpen()
{
    return m_currentPosition == m_openPosition;
}

bool Door::isClosed()
{
    return m_currentPosition == m_closedPosition;
}