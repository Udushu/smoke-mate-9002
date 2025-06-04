#include "door.h"

Door::Door(uint8_t pin, uint closedPos, uint openPos, ulong stepMoveTimeMSec)
    : m_pin(pin),
      m_closedPosition(closedPos),
      m_openPosition(openPos),
      m_currentPosition(0),
      m_demandedPosition(0),
      m_state(DOOR_IDLE),
      m_stepMoveTimeMSec(stepMoveTimeMSec)
{
    m_prevState = m_state;
}

void Door::begin()
{
    m_servo.attach(m_pin, DOOR_SERVO_MIN_PULSE_WIDTH, DOOR_SERVO_MAX_PULSE_WIDTH); // Attach servo with min and max pulse width
    m_servo.write(0);
    m_state = DOOR_STOPPED; // Start in stopped state
    delay(500);             // Allow servo to initialize
    m_servo.detach();       // Detach servo to allow it to initialize properly
}

void Door::service(ulong currentTimeMSec)
{

    switch (m_state)
    {
    case DOOR_IDLE:

        break;
    case DOOR_STOPPED:
        // Chill here until a position is set
        break;

    case DOOR_STARTING:
        m_startMoveTimeMSec = currentTimeMSec; // Record the start time
#ifdef DOOR_DEBUG
        DEBUG_PRINTLN("DOOR::service - Starting move to position: " + String(m_demandedPosition));
#endif
        m_servo.write(m_demandedPosition); // Set the servo to the demanded position
        m_state = DOOR_MOVING;             // Set state to moving
        break;

    case DOOR_MOVING:
        if (currentTimeMSec - m_startMoveTimeMSec >= m_traveTimeMsec)
        {
            // Movement is complete
            m_currentPosition = m_demandedPosition; // Update current position
            m_state = DOOR_STOPPED;                 // Set state to stopped
            m_servo.detach();                       // Detach servo to stop it from moving
        }
        break;

    default:
        break;
    }

#ifdef DOOR_DEBUG
    // Compare the prev state with the current state and report if it changed
    if (m_prevState != m_state)
    {
        auto stateToString = [](DoorState state) -> const char *
        {
            switch (state)
            {
            case DOOR_IDLE:
                return "IDLE";
            case DOOR_STOPPED:
                return "STOPPED";
            case DOOR_STARTING:
                return "STARTING";
            case DOOR_MOVING:
                return "MOVING";
            default:
                return "UNKNOWN";
            }
        };
        DEBUG_PRINTLN("DOOR::service - State changed from " + String(stateToString(m_prevState)) + " to " + String(stateToString(m_state)));
        m_prevState = m_state; // Update previous state
    }
#endif
}

void Door::setPosition(uint pos)
{
#ifdef DOOR_DEBUG
    DEBUG_PRINTLN("DOOR::setPosition to " + String(pos));
#endif
    // Make sure that the door is not moving
    if (m_state == DOOR_MOVING)
    {
#ifdef DOOR_DEBUG
        DEBUG_PRINTLN("DOOR:setPosition - Door is Moving");
#endif
        return;
    }

    // Make sure that the position is not the same as the current position
    if (pos == m_currentPosition)
    {
#ifdef DOOR_DEBUG
        DEBUG_PRINTLN("DOOR::setPosition - Allready There Position");
#endif
        return;
    }

    pos = constrain(pos, m_closedPosition, m_openPosition); // Constrain the position to the allowed range

    // Calcuate the travel time based on the step move time and the distance to travel
    m_traveTimeMsec = abs(static_cast<int>(m_currentPosition) - static_cast<int>(pos)) * DOOR_STEP_MOVE_TIME_MSEC;
    m_demandedPosition = pos;
    m_servo.attach(m_pin, DOOR_SERVO_MIN_PULSE_WIDTH, DOOR_SERVO_MAX_PULSE_WIDTH); // Attach servo with min and max pulse width
    m_state = DOOR_STARTING;                                                       // Set state to starting

#ifdef DOOR_DEBUG
    DEBUG_PRINTLN("DOOR::setPosition - Demanded Position: " + String(m_demandedPosition) + ", Travel Time: " + String(m_traveTimeMsec));
#endif
}

uint Door::getPosition()
{
    return m_currentPosition;
}

void Door::open()
{
#ifdef DOOR_DEBUG
    DEBUG_PRINTLN("DOOR::open - Opening Door");
#endif
    setPosition(m_openPosition);
}

void Door::close()
{
#ifdef DOOR_DEBUG
    DEBUG_PRINTLN("DOOR::close - Closing Door");
#endif
    setPosition(m_closedPosition);
}

bool Door::isMoving()
{
    return m_state == DOOR_MOVING;
}

bool Door::isOpen()
{
#ifdef DOOR_DEBUG
    DEBUG_PRINTLN("DOOR::isOpen - Current Position: " + String(m_currentPosition) + ", Open Position: " + String(m_openPosition));
#endif
    return m_currentPosition == m_openPosition;
}

bool Door::isClosed()
{
#ifdef DOOR_DEBUG
    DEBUG_PRINTLN("DOOR::isClosed - Current Position: " + String(m_currentPosition) + ", Closed Position: " + String(m_closedPosition));
#endif
    return m_currentPosition == m_closedPosition;
}

void Door::setBoundaries(uint closedPos, uint openPos)
{
#ifdef DOOR_DEBUG
    DEBUG_PRINTLN("DOOR::setBoundaries - Closed Position: " + String(closedPos) + ", Open Position: " + String(openPos));
#endif
    m_closedPosition = closedPos;
    m_openPosition = openPos;
}