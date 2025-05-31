#include "main.h"

// Timer variables
ulong g_loopTimer1000MSec = 0;
ulong g_loopTimer500MSec = 0;
ulong g_loopCurrentTimeMSec = 0;

// System configuration
Configuration g_configuration;

// NVRAM definition
NVRAM g_nvram = NVRAM(NVRAM_START_ADDRESS, (uint8_t *)&g_configuration, sizeof(Configuration));

// Knob definition
Knob g_knob = Knob(PIN_ENCODER_A, PIN_ENCODER_B, PIN_ENCODER_BTN, 1000, 50);

// TFT display definition
Adafruit_ST7789 g_tftDisplay = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
TFTDebug g_tftDebug(&g_tftDisplay, 10, 10, 2);

// Thermometers
Thermometer g_thermometerSmoker(DEFAULT_TEMPERATURE_INTERVAL_MSEC, PIN_THERMOMETER_SMOKER_CS, PIN_THERMOMETER_CLK, PIN_THERMOMETER_SO);
Thermometer g_thermometerFood(DEFAULT_TEMPERATURE_INTERVAL_MSEC, PIN_THERMOMETER_FOOD_CS, PIN_THERMOMETER_CLK, PIN_THERMOMETER_SO);

// Door definition
Door g_door(PIN_DOOR_SERVO, DEFAULT_DOOR_CLOSE_POSITION, DEFAULT_DOOR_OPEN_POSITION);

// Blower motor definition
Blower g_blowerMotor(PIN_BLOWER_PWM, PIN_BLOWER_A, PIN_BLOWER_B, PIN_BLOWER_ENABLE);

int pos = 0;

void setup()
{

  // Set up SPI manually if needed (ESP32 hardware SPI)
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  // Initalize TFT display
  g_tftDisplay.init(240, 320); // Init ST7789 240x320
  g_tftDisplay.setRotation(1); // Landscape
  g_tftDisplay.fillScreen(ST77XX_BLACK);
  g_tftDisplay.setTextColor(ST77XX_WHITE);
  g_tftDisplay.setTextSize(2);
  g_tftDisplay.setCursor(10, 10);

  // Initialize debug output on TFT
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // Initialize debug output
  g_tftDebug.clear();
  Serial.begin(BAUD_RATE);

  // Initialize NVRAM
  setupInitializeNVRAM();

  // Initialize the door
  g_door.begin();
  g_door.close(); // Start with the door closed
  delay(2000);    // Wait for servo to stabilize

  // Initialize the blower motor
  g_blowerMotor.setID("Blower");

  // Blink LED to indicate startup
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH); // Turn off LED
  delay(1000);                 // Wait for a second
  digitalWrite(PIN_LED, LOW);  // Turn on LED
}

void loop()
{

  // Get the current time in milliseconds
  g_loopCurrentTimeMSec = millis();

  // Service the knob
  g_knob.service(g_loopCurrentTimeMSec);

  // Service the thermometers
  g_thermometerSmoker.service(g_loopCurrentTimeMSec);
  g_thermometerFood.service(g_loopCurrentTimeMSec);

  // Service the door
  g_door.service(g_loopCurrentTimeMSec);
}

void loadDefaultConfiguration(Configuration *ptr_configuration)
{
  ptr_configuration->temperatureTarget = DEFAULT_TEMPERATURE_TARGET;
  ptr_configuration->temperatureIntervalMSec = DEFAULT_TEMPERATURE_INTERVAL_MSEC;
  ptr_configuration->forcedFanPWM = DEFAULT_BANG_BANG_FAN_SPEED;
  ptr_configuration->isForcedFanPWM = false;
  ptr_configuration->forcedDoorPosition = 0;
  ptr_configuration->isForcedDoorPosition = false;
  ptr_configuration->isPIDEnabled = false;
  ptr_configuration->kP = DEFAULT_PID_KP;
  ptr_configuration->kI = DEFAULT_PID_KI;
  ptr_configuration->kD = DEFAULT_PID_KD;
  ptr_configuration->bangBangLowThreshold = DEFAULT_BANG_BANG_THRESHOLD_LOW;
  ptr_configuration->bangBangHighThreshold = DEFAULT_BANG_BANG_THRESHOLD_HIGH;
  ptr_configuration->bangBangHysteresis = DEFAULT_BANG_BANG_HYSTERESIS;
  ptr_configuration->bangBangFanSpeed = DEFAULT_BANG_BANG_FAN_SPEED;
  ptr_configuration->doorOpenPosition = DEFAULT_DOOR_OPEN_POSITION;
  ptr_configuration->doorClosePosition = DEFAULT_DOOR_CLOSE_POSITION;
  ptr_configuration->themometerSmokerGain = DEFAULT_THERMOMETER_SMOKER_GAIN;
  ptr_configuration->themometerSmokerOffset = DEFAULT_THERMOMETER_SMOKER_OFFSET;
  ptr_configuration->themometerFoodGain = DEFAULT_THERMOMETER_FOOD_GAIN;
  ptr_configuration->themometerFoodOffset = DEFAULT_THERMOMETER_FOOD_OFFSET;
}

void setupInitializeNVRAM()
{
  bool result;
  result = g_nvram.initNVRAM(); // Initialize NVRAM
#ifdef DEBUG_MAIN
  if (!result)
  {
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("Failed to INIT NVRAM");
  }
  else
  {
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("NVRAM INIT success");
  }
#endif
#ifdef NVRAM_DELETE_ON_STARTUP
  g_nvram.clearNVRAM(); // Clear NVRAM on startup
#ifdef DEBUG_MAIN
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("NVRAM cleared on startup");
#endif
  loadDefaultConfiguration(&g_configuration); // Load default configuration
#ifdef DEBUG_MAIN
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("Dflt config loaded");
#endif
#endif
  result = g_nvram.readNVRAM(); // Read NVRAM
  if (!result)
  {

#ifdef DEBUG_MAIN
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("Failed to READ NVRAM");
    DEBUG_PRINTLN("Loading DFLT config");
#endif
    loadDefaultConfiguration(&g_configuration); // Load default configuration
#ifdef DEBUG_MAIN
    DEBUG_PRINTLN();
    DEBUG_PRINTLN("WRT DFLT config to NVRAM");
#endif
    delay(1000);
    result = g_nvram.writeNVRAM();
#ifdef DEBUG_MAIN
    DEBUG_PRINTLN(result ? "SUCCESS WRT to NVRAM" : "FAILED WRT to NVRAM");
    delay(1000);
#endif
  }
  else
  {
#ifdef DEBUG_MAIN
    DEBUG_PRINTLN("NVRAM READ success");
#endif
  }
}
