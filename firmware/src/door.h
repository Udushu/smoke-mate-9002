#ifndef DOOR_H
#define DOOR_H
#include <Arduino.h>
#include <ESP32Servo.h>
#include "types.h"
#include "debug.h"

#define DOOR_DEBUG
#define DOOR_STEP_MOVE_TIME_MSEC 5UL    // Default step move time in milliseconds (type: ulong)
#define DOOR_SERVO_MIN_PULSE_WIDTH 500  // Minimum pulse width for the servo (in microseconds)
#define DOOR_SERVO_MAX_PULSE_WIDTH 2500 // Maximum pulse width for the servo (in microseconds)

enum DoorState
{
    DOOR_IDLE,
    DOOR_STOPPED,
    DOOR_STARTING,
    DOOR_MOVING
};

class Door
{
public:
    Door(uint8_t pin, uint closedPos = 0, uint openPos = 90, ulong stepMoveTimeMSec = 50);

    void begin();
    void service(ulong currentTimeMSec);
    void setPosition(uint pos);
    uint getPosition();
    void open();
    void close();
    bool isMoving();
    bool isOpen();
    bool isClosed();
    void setBoundaries(uint closedPos, uint openPos);

private:
    DoorState m_state;
    DoorState m_prevState;
    ulong m_startMoveTimeMSec;
    ulong m_stepMoveTimeMSec;
    ulong m_traveTimeMsec;
    uint8_t m_pin;
    uint m_closedPosition;
    uint m_openPosition;
    uint m_currentPosition;
    uint m_demandedPosition;
    Servo m_servo;
};

#endif // DOOR_H