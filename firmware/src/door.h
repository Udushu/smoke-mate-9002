#ifndef DOOR_H
#define DOOR_H
#include <Arduino.h>
#include <ESP32Servo.h>
#include "types.h"
#include "debug.h"

#define DOOR_DEBUG

enum DOOR_STATE
{
    DOOR_IDLE,
    DOOR_STOPPED,
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

private:
    DOOR_STATE m_state;
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