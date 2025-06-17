#include "main.h"
#include "Arduino.h"

// Timer variables
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

// Initalie the interface
SmokeMateGUI g_smokeMateGUI(g_tftDisplay, g_configuration);

// Device status
ControllerStatus g_controllerStatus; // Controller status
bool g_prevIsRunning = false;        // Previous running state for the controller

// Temperature Controller
TemperatureController g_temperatureController(g_controllerStatus, g_configuration, g_blowerMotor, g_door);

// Webserver
WebServer g_webServer = WebServer(WEB_SERVER_PORT, g_controllerStatus, g_configuration);
bool g_prevWiFiConnected = false;                      // Previous WiFi connected state for the controller
int g_wiFiConnectAttempts = MAX_WIFI_CONNECT_ATTEMPTS; // Maximum number of attempts to connect to WiFi

// Temperature profiling variables
int g_temperatureProfileStepIndex = -1;      // Current step index in the temperature profile, -1 means no active profile
ulong g_temperatureProfileStartTimeMSec = 0; // Start time of the current temperature profile step

void setup()
{

  // Blink LED to indicate startup
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH); // Turn on LED
  delay(500);

  // Set up SPI manually if needed (ESP32 hardware SPI)
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  // Initalize TFT display
  g_tftDisplay.init(240, 320);
  g_tftDisplay.setRotation(1);
  g_tftDisplay.fillScreen(ST77XX_BLACK);

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
  g_door.setBoundaries(g_configuration.doorClosePosition, g_configuration.doorOpenPosition);
  g_door.close();
  while (g_door.isMoving())
  {
  }

  // Initialize the blower motor
  g_blowerMotor.setPWM(0);

  // Initalize the interface
  g_smokeMateGUI.begin();

  // Initialize the thermometers
  g_thermometerSmoker.setSimulated(g_configuration.isThemometerSimulated);
  g_thermometerFood.setSimulated(g_configuration.isThemometerSimulated);

  if (g_configuration.isWiFiEnabled)
  {
    // If WiFi is enabled, attempt to connect
    DEBUG_PRINTLN("WiFi is enabled, attempting to connect...");
    connectToWiFi();
    if (g_controllerStatus.isWiFiConnected)
    {
      DEBUG_PRINTLN("WiFi connected successfully.");
      g_webServer.begin(); // Start the web server
    }
  }

  digitalWrite(PIN_LED, LOW); // Turn off LED - setup is now done
}

void loop()
{

  ArduinoOTA.handle();

  // Get the current time in milliseconds
  g_loopCurrentTimeMSec = millis();

  // ====== SERVICE ALL COMPONENTS THAT RUN AT THE CPU RATE

  // Service the knob
  g_knob.service(g_loopCurrentTimeMSec);

  // Service the thermometers
  g_thermometerSmoker.service(g_loopCurrentTimeMSec);
  g_thermometerFood.service(g_loopCurrentTimeMSec);

  // Service the door and blower
  g_door.service(g_loopCurrentTimeMSec);
  g_blowerMotor.service(g_loopCurrentTimeMSec);

  loopServiceKnobButtonEvents();
  loopUpdateControllerStatus();
  updateConfiguration();

  // Do actions on the controller status change
  if (g_controllerStatus.isRunning != g_prevIsRunning)
  {
    // if the controller just stopped close the door and stop the blower motor
    if (!g_controllerStatus.isRunning && g_prevIsRunning)
    {
      // Stop the blower motor
      g_blowerMotor.setPWM(0);
      // Close the door
      g_door.close();
    }
    else if (g_controllerStatus.isRunning && !g_prevIsRunning)
    {
      // If the controller just started, set the start time
      g_controllerStatus.controllerStartMSec = g_loopCurrentTimeMSec;
      // Reset the temperature profile step index
      g_temperatureProfileStepIndex = -1; // Reset the temperature profile step index
    }
    g_prevIsRunning = g_controllerStatus.isRunning; // Update the previous running state
  }

  // Service the temperature controller
  if (g_controllerStatus.isRunning && g_thermometerSmoker.isNewTemperatureAvailable())
  {
    // Update the target temperature
    g_controllerStatus.temperatureTarget = calculateTemperatureTarget();
    // If the controller is not running, we still want to update the temperature
    g_temperatureController.service(g_thermometerSmoker.getTemperatureF(), g_loopCurrentTimeMSec);
    g_controllerStatus.temperatureError = g_temperatureController.getLastOutput(); // Get the last output from the temperature controller
  }

  if (!g_controllerStatus.isRunning && g_configuration.isForcedDoorPosition)
  {
    g_door.setPosition(g_configuration.forcedDoorPosition); // Set the door position if forced
  }

  if (!g_controllerStatus.isRunning && g_configuration.isForcedFanPWM)
  {
    g_blowerMotor.setPWM(g_configuration.forcedFanPWM); // Set the blower motor PWM if forced
  }

  // Check nvram save request
  if (g_webServer.isNVRAMSaveRequired())
  {
    g_nvram.writeNVRAM(); // Write the configuration to NVRAM
  }

  // Check the 0.5 second timer
  if (g_loopCurrentTimeMSec - g_loopTimer500MSec >= 500)
  {
    g_loopTimer500MSec = g_loopCurrentTimeMSec;

    // Update GUI state
    g_smokeMateGUI.updateState(g_controllerStatus, g_configuration);
    g_smokeMateGUI.service(g_loopCurrentTimeMSec);
    // Check if the gui requested NVRAM save
    if (g_smokeMateGUI.isNVRAMSaveRequired())
    {
      g_nvram.writeNVRAM(); // Write the configuration to NVRAM
    }

    // Update the wifi status
    if (g_configuration.isWiFiEnabled)
    {
      g_controllerStatus.RSSI = WiFi.RSSI();
      g_controllerStatus.bars = WiFi.RSSI() / -20; // Convert RSSI to bars (0-5)

      // --- WiFi/server auto-reconnect logic ---
      if (g_prevWiFiConnected &&
          !g_controllerStatus.isWiFiConnected &&
          g_wiFiConnectAttempts < MAX_WIFI_CONNECT_ATTEMPTS)
      {
        // Lost connection, try to reconnect and restart server
        DEBUG_PRINTLN("WiFi connection lost, attempting to reconnect...");
        connectToWiFi();
        if (g_controllerStatus.isWiFiConnected)
        {
          DEBUG_PRINTLN("WiFi reconnected, restarting web server...");
          g_webServer.begin();
          g_wiFiConnectAttempts = 0; // Reset the connection attempts
        }
        else
        {
          g_wiFiConnectAttempts++; // Increment the connection attempts
          DEBUG_PRINTLN("WiFi reconnect attempt failed, retrying...");
        }
      }
      g_prevWiFiConnected = g_controllerStatus.isWiFiConnected;
    }
  }
}

void loopServiceKnobButtonEvents()
{
  if (g_knob.isShortButtonPressed())
  {
    g_smokeMateGUI.commandSelect(); // Handle short button press
  }

  // Check for the long press on the knob
  if (g_knob.isLongButtonPressed())
  {
    // Check on the current GUI state
    if (g_smokeMateGUI.getState().header.state == GUI_STATE_HEADER_STATUS ||
        g_smokeMateGUI.getState().header.state == GUI_STATE_HEADER_CHART ||
        g_smokeMateGUI.getState().header.state == GUI_STATE_HEADER_SETTINGS)
    {
      // Toggle the controller running state
      g_controllerStatus.isRunning = !g_controllerStatus.isRunning;
      if (g_controllerStatus.isRunning)
      {
        // Start the controller
        g_controllerStatus.controllerStartMSec = g_loopCurrentTimeMSec;
        // g_blowerMotor.start(); // Start the blower motor
      }
      else
      {
        // Stop the controller
        // g_blowerMotor.stop(); // Stop the blower motor
      }
    }
    else
    {
      g_smokeMateGUI.commandConfirm(); // Handle long button press in settings edit mode
    }
  }

  if (g_knob.isUltraLongButtonPressed())
  {
    // Reset ESP32 and NVRAM
    g_tftDisplay.fillScreen(ST77XX_BLACK); // Clear the display
    DEBUG_PRINTLN("Ultra long button press detected, clearing NVRAM resetting ESP32");
    g_nvram.clearNVRAM(); // Clear NVRAM
    delay(1000);          // Wait for a second to ensure NVRAM is cleared
    ESP.restart();        // Restart the ESP32
  }

  if (g_knob.isRotatingUp())
  {
    // Increase the target temperature
    g_smokeMateGUI.commandMoveNext();
  }

  if (g_knob.isRotatingDown())
  {
    // Decrease the target temperature
    g_smokeMateGUI.commandMovePrevious();
  }
}

void updateConfiguration()
{
  g_thermometerSmoker.setSimulated(g_configuration.isThemometerSimulated);
  g_thermometerFood.setSimulated(g_configuration.isThemometerSimulated);
}

void loopUpdateControllerStatus()
{
  // Update the status variable of the controller
  g_controllerStatus.temperatureSmoker = g_thermometerSmoker.getTemperatureF();
  g_controllerStatus.temperatureFood = g_thermometerFood.getTemperatureF();
  g_controllerStatus.fanPWM = g_blowerMotor.getPWM();
  g_controllerStatus.doorPosition = g_door.getPosition();
  g_controllerStatus.uptime = g_loopCurrentTimeMSec;
  // g_controllerStatus.RSSI = WiFi.RSSI();
  // g_controllerStatus.bars = WiFi.RSSI() / -20;             // Convert RSSI to bars (0-5)
  g_controllerStatus.isWiFiConnected = WiFi.isConnected(); // Update WiFi connection status
  if (!g_controllerStatus.isRunning)
  {
    if (g_configuration.isTemperatureProfilingEnabled && g_configuration.temperatureProfileStepsCount > 0)
    {
      g_controllerStatus.temperatureTarget = g_configuration.temperatureProfile[0].temperatureStartF; // Set target temperature to the first profile step
    }
    else
    {
      g_controllerStatus.temperatureTarget = g_configuration.temperatureTarget; // Set target temperature to the configured value
    }
  }

  int temperatureProfileisRunning = 0;
  if (g_temperatureProfileStepIndex == -1)
  {
    temperatureProfileisRunning = 0; // No active profile step, profile is not running
  }
  else if (g_temperatureProfileStepIndex >= 0 &&
           g_temperatureProfileStepIndex < g_configuration.temperatureProfileStepsCount)
  {
    temperatureProfileisRunning = 1; // Profile is running
  }
  else
  {
    temperatureProfileisRunning = 2; // Profile finished
  }
  g_controllerStatus.isProfileRunning = temperatureProfileisRunning;
  g_controllerStatus.temperatureProfileStepIndex = g_temperatureProfileStepIndex;                 // Current step index in the temperature profile
  g_controllerStatus.temperatureProfileStartTimeMSec = g_temperatureProfileStartTimeMSec;         // Start time of the current temperature profile step
  g_controllerStatus.temperatureProfileStepsCount = g_configuration.temperatureProfileStepsCount; // Number of steps in the temperature profile
  if (g_temperatureProfileStepIndex >= 0 && g_temperatureProfileStepIndex < g_configuration.temperatureProfileStepsCount)
  {
    g_controllerStatus.temperatureProfileStepType = g_configuration.temperatureProfile[g_temperatureProfileStepIndex].type; // Type of the current temperature profile step
  }
  else
  {
    g_controllerStatus.temperatureProfileStepType = TEMP_PROFILE_TYPE_DWELL; // Default to dwell type if no active profile step
  }
}

void setupInitializeControllerStatus(ControllerStatus &controllerStatus)
{
  controllerStatus.isRunning = false;                                     // Start with controller not running
  controllerStatus.uuid = "00000000-0000-0000-0000-000000000000";         // Default UUID
  controllerStatus.uptime = 0;                                            // Start with zero uptime
  controllerStatus.controllerStartMSec = 0;                               // Set controller start time
  controllerStatus.temperatureSmoker = 0;                                 // Start with zero smoker temperature
  controllerStatus.temperatureFood = 0;                                   // Start with zero food temperature
  controllerStatus.temperatureTarget = g_configuration.temperatureTarget; // Set target temperature from configuration
  controllerStatus.fanPWM = 0;                                            // Start with fan off
  controllerStatus.doorPosition = 0;                                      // Get initial door position
  controllerStatus.RSSI = 0;                                              // Start with zero RSSI
  controllerStatus.bars = 0;                                              // Start with zero bars
}

void loadDefaultConfiguration(Configuration *ptr_configuration)
{
  ptr_configuration->temperatureTarget = DEFAULT_TEMPERATURE_TARGET;
  ptr_configuration->temperatureIntervalMSec = DEFAULT_TEMPERATURE_INTERVAL_MSEC;

  ptr_configuration->isTemperatureProfilingEnabled = false; // Start with temperature profiling disabled
  ptr_configuration->temperatureProfileStepsCount = 0;      // Start with no temperature profile steps
  for (int i = 0; i < MAX_PROFILE_STEPS; i++)
  {
    ptr_configuration->temperatureProfile[i].timeMSec = 1000;
    ptr_configuration->temperatureProfile[i].type = TEMP_PROFILE_TYPE_DWELL;                 // Default to dwell type
    ptr_configuration->temperatureProfile[i].temperatureStartF = DEFAULT_TEMPERATURE_TARGET; // Default to target temperature
    ptr_configuration->temperatureProfile[i].temperatureEndF = DEFAULT_TEMPERATURE_TARGET;   // Default to target temperature
  }

  ptr_configuration->isPIDEnabled = true;
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

  ptr_configuration->isThemometerSimulated = false; // Start with real thermometers

  ptr_configuration->isForcedFanPWM = false;
  ptr_configuration->forcedFanPWM = DEFAULT_BANG_BANG_FAN_SPEED;
  ptr_configuration->isForcedDoorPosition = false;
  ptr_configuration->forcedDoorPosition = 0;

  ptr_configuration->isWiFiEnabled = false;                                                    // Start with WiFi disabled
  strncpy(ptr_configuration->wifiSSID, NETWORK_SSID, sizeof(ptr_configuration->wifiSSID) - 1); // Default SSID
  ptr_configuration->wifiSSID[sizeof(ptr_configuration->wifiSSID) - 1] = '\0';
  strncpy(ptr_configuration->wifiPassword, NETWORK_PASSWORD, sizeof(ptr_configuration->wifiPassword) - 1); // Default password
  ptr_configuration->wifiPassword[sizeof(ptr_configuration->wifiPassword) - 1] = '\0';
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

void connectToWiFi()
{
  // attempt to connect to WiFi with the stored credentials, repeating until successful or number of attempts is reached
  int attempts = 0;
  g_controllerStatus.isWiFiConnected = false;
  while (attempts < MAX_WIFI_CONNECT_ATTEMPTS)
  {

    // If NVRAM read was successful, try to connect to WiFi
    if (g_configuration.wifiSSID[0] != '\0' && g_configuration.wifiPassword[0] != '\0')
    {
      DEBUG_PRINTLN("Connecting to WiFi...");
      WiFi.begin(g_configuration.wifiSSID, g_configuration.wifiPassword);
      if (WiFi.waitForConnectResult() == WL_CONNECTED)
      {
        // Store the IP address in the controller status
        g_controllerStatus.ipAddress = WiFi.localIP().toString();
        g_controllerStatus.RSSI = WiFi.RSSI();

        if (g_controllerStatus.RSSI > -55)
          g_controllerStatus.bars = 4;
        else if (g_controllerStatus.RSSI > -70)
          g_controllerStatus.bars = 3;
        else if (g_controllerStatus.RSSI > -80)
          g_controllerStatus.bars = 2;
        else if (g_controllerStatus.RSSI > -90)
          g_controllerStatus.bars = 1;
        else
          g_controllerStatus.bars = 0;

        g_controllerStatus.isWiFiConnected = true; // Set WiFi connected status
        g_prevWiFiConnected = true;                // Update previous WiFi connected status
        DEBUG_PRINTLN("Connected to WiFi");
        // print the IP address
        DEBUG_PRINTLN("IP Address: ");
        DEBUG_PRINTLN(g_controllerStatus.ipAddress);

        break; // Exit loop on successful connection
      }
    }

    attempts++;
    delay(1000); // Wait before retrying
  }

  if (g_controllerStatus.isWiFiConnected)
  {
    // Set Network name
    g_controllerStatus.networkName = WiFi.SSID();

    // OTA setup
    ArduinoOTA.setHostname("smokemate9002");

    ArduinoOTA.onStart(
        []()
        {
          Serial.println("OTA Update Starting...");
          // Show message on TFT
          g_tftDisplay.fillRect(0, 260, 240, 60, ST77XX_BLACK); // Clear area
          g_tftDisplay.setCursor(10, 280);
          g_tftDisplay.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
          g_tftDisplay.setTextSize(2);
          g_tftDisplay.print("OTA Update...");
        });
    ArduinoOTA.onEnd(
        []()
        {
          // Print to serial
          Serial.println("OTA Update Complete!");
          // Show message on TFT
          g_tftDisplay.fillRect(0, 260, 240, 60, ST77XX_BLACK); // Clear area
          g_tftDisplay.setCursor(10, 280);
          g_tftDisplay.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
          g_tftDisplay.setTextSize(2);
          g_tftDisplay.print("OTA Done!");
        });
    ArduinoOTA.onProgress(
        [](unsigned int progress, unsigned int total)
        {
          // Calculate percentage
          int percent = (progress * 100) / total;

          // Print to serial
          Serial.printf("OTA Progress: %u%%\r\n", percent);

          // Show progress on TFT (simple example)
          g_tftDisplay.fillRect(10, 300, 220, 10, ST77XX_BLACK);                   // Clear previous bar
          g_tftDisplay.fillRect(10, 300, (220 * percent) / 100, 10, ST77XX_GREEN); // Draw progress bar
          g_tftDisplay.setCursor(10, 280);
          g_tftDisplay.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
          g_tftDisplay.setTextSize(2);
          g_tftDisplay.printf("OTA: %d%%", percent);
        });
    ArduinoOTA.onError([](ota_error_t error) {});

    ArduinoOTA.begin();
  }
}

int calculateTemperatureTarget()
{
  // Check if the temperature profiling is disabled or there are no configured steps
  // then just set the target based on the configuration
  if (!g_configuration.isTemperatureProfilingEnabled ||
      g_configuration.temperatureProfileStepsCount == 0)

  {
    // Temperature profiling is disabled or no steps are configured then just return the target temperature
    return g_configuration.temperatureTarget; // Return the target temperature from configuration
  }
  else if (g_configuration.isTemperatureProfilingEnabled && g_temperatureProfileStepIndex >= g_configuration.temperatureProfileStepsCount)
  {
    // If the temperature profiling is enabled but the current step index is out of bounds, return the last step's temperature
    // If the last step is a ramp step, return the end temperature of the last step
    TempProfileStep lastStep = g_configuration.temperatureProfile[g_configuration.temperatureProfileStepsCount - 1];
    if (lastStep.type == TEMP_PROFILE_TYPE_RAMP)
    {
      return lastStep.temperatureEndF; // Return the end temperature of the last step
    }
    else
    {
      return lastStep.temperatureStartF; // Return the start temperature of the last step
    }
  }
  else
  {
    // Check if the current call is the first call for the profile
    if (g_temperatureProfileStepIndex < 0)
    {
      // If the current step index is -1, it means we are starting the profile
      g_temperatureProfileStepIndex = 0;                         // Start from the first step
      g_temperatureProfileStartTimeMSec = g_loopCurrentTimeMSec; // Set the start time for the profile step
    }

    // Get the current step from the temperature profile
    TempProfileStep step = g_configuration.temperatureProfile[g_temperatureProfileStepIndex];

    // Check if the time has elapsed for the current step
    if (g_loopCurrentTimeMSec - g_temperatureProfileStartTimeMSec >= step.timeMSec)
    {
      // If the time has elapsed, move to the next step
      g_temperatureProfileStepIndex++;
      g_temperatureProfileStartTimeMSec = g_loopCurrentTimeMSec; // Reset the start time for the next step
    }
    else
    {
      // Check the type of the current step
      if (step.type == TEMP_PROFILE_TYPE_RAMP)
      {
        // If the step is a ramp, calculate the target temperature based on the elapsed time
        float elapsedTime = (g_loopCurrentTimeMSec - g_temperatureProfileStartTimeMSec) / 1000.0; // Convert to seconds
        float totalRampTime = step.timeMSec / 1000.0;                                             // Total ramp time in seconds
        float temperatureChange = step.temperatureEndF - step.temperatureStartF;                  // Temperature change for the ramp
        float temperaturePerSecond = temperatureChange / totalRampTime;                           // Temperature change per second

        // Calculate the target temperature based on the elapsed time
        return step.temperatureStartF + (temperaturePerSecond * elapsedTime);
      }
      else
      {
        // If the step is a dwell, return the start temperature of the dwell step
        return step.temperatureStartF; // Return the start temperature of the dwell step
      }
    }
  }

  return 0; // Fallback return value, should not be reached
}