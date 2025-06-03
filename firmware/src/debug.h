#ifndef DEBUG_H
#define DEBUG_H
#include <Arduino.h>

// #define ENBALE_TFT_DEBUG

#ifdef ENBALE_TFT_DEBUG
#include "tftdebug.h"
extern TFTDebug g_tftDebug;
#define DEBUG_PRINT(x) g_tftDebug.print(x)
#define DEBUG_PRINTLN(x) g_tftDebug.println(x)
#else
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#endif

#endif