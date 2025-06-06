#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "types.h"
#include "knob.h"
#include "nvram.h"
#include "thermometer.h"
#include "door.h"
#include "blower.h"
#include "gui.h"
#include "temperaturecontroller.h"
#include "tftdebug.h"
#include "debug.h"
#include "webserver.h"

// ============================ COMPILATION FLAGS =========================
// #define DEBUG_MAIN
// #define NVRAM_DELETE_ON_STARTUP

// ============================ CONSTANTS =================================
#define WEB_SERVER_PORT 80 // Web server port
#define BAUD_RATE 115200
#define NVRAM_START_ADDRESS 0

#define WIFI_SCAN_TIMEOUT_MSEC 5000 // Timeout for WiFi scan in milliseconds
#define MAX_WIFI_NETWORKS 8         // Maximum number of WiFi networks to store
#define MAX_WIFI_CONNECT_ATTEMPTS 3 // Maximum number of attempts to connect to WiFi

// Default configuration values
#define DEFAULT_TEMPERATURE_TARGET 250
#define DEFAULT_TEMPERATURE_INTERVAL_MSEC 5000
#define DEFAULT_BANG_BANG_THRESHOLD_HIGH 260
#define DEFAULT_BANG_BANG_THRESHOLD_LOW 240
#define DEFAULT_BANG_BANG_HYSTERESIS 5
#define DEFAULT_BANG_BANG_FAN_SPEED 255
#define DEFAULT_DOOR_OPEN_POSITION 110
#define DEFAULT_DOOR_CLOSE_POSITION 4
#define DEFAULT_PID_KP 4.0
#define DEFAULT_PID_KI 0.0
#define DEFAULT_PID_KD 0.0
#define DEFAULT_THERMOMETER_SMOKER_GAIN 1.0
#define DEFAULT_THERMOMETER_SMOKER_OFFSET 0.0
#define DEFAULT_THERMOMETER_FOOD_GAIN 1.0
#define DEFAULT_THERMOMETER_FOOD_OFFSET 0.0

// ============================ PIN ASSIGNMENTS ===========================
#define PIN_LED 2

// Knob encoder pins
#define PIN_ENCODER_A 25
#define PIN_ENCODER_B 27
#define PIN_ENCODER_BTN 26

// Thermometer pins
#define PIN_THERMOMETER_SO 19        // Shared SO (data out)
#define PIN_THERMOMETER_CLK 15       // Shared Clock (separate from TFT)
#define PIN_THERMOMETER_FOOD_CS 32   // Thermocouple #1 Chip Select
#define PIN_THERMOMETER_SMOKER_CS 33 // Thermocouple #2 Chip Select

// TFT display pins
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 17
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_BL 22

// Door servo pins
#define PIN_DOOR_SERVO 21

// Blower motor pins
#define PIN_BLOWER_PWM 14
#define PIN_BLOWER_A 13
#define PIN_BLOWER_B 12
#define PIN_BLOWER_ENABLE 4

// ============================= TFT CONFIGURATION =========================

// TFT display configuration
#define ST7789_DRIVER
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define LOAD_GFXFF
#define SPI_FREQUENCY 27000000
#define SPI_READ_FREQUENCY 20000000

// ============================= FUNCTIONS ===================================

void updateGuiState(GuiState &guiState, const ControllerStatus &controllerStatus);
void loadDefaultConfiguration(Configuration *ptr_configuration);
void setupInitializeNVRAM();
void setupInitializeGuiState(GuiState &guiState);
void setupInitializeControllerStatus(ControllerStatus &controllerStatus);
void loopServiceKnobButtonEvents();
void loopUpdateControllerStatus();
void updateConfiguration();

#endif // MAIN_H