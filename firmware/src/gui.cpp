#include "gui.h"
#include <math.h>

// ========================================== SETTINGS GETTERS & SETTERS ==============================

// TARGET TEMPERATURE =================================================================================
static String getTargetTemp(const Configuration &c) { return String(c.temperatureTarget) + " F"; }
void incTargetTemp(Configuration &c)
{
    if (c.temperatureTarget + GUI_SETTINGS_TEMP_STEP <= GUI_SETTINGS_TEMP_MAX)
        c.temperatureTarget += GUI_SETTINGS_TEMP_STEP;
}
void decTargetTemp(Configuration &c)
{
    if (c.temperatureTarget - GUI_SETTINGS_TEMP_STEP >= GUI_SETTINGS_TEMP_MIN)
        c.temperatureTarget -= GUI_SETTINGS_TEMP_STEP;
}

// TEMPERATURE INTERVAL ==============================================================================
static String getInterval(const Configuration &c) { return String(c.temperatureIntervalMSec / 1000) + " sec"; }
void incInterval(Configuration &c)
{
    if (c.temperatureIntervalMSec + GUI_SETTINGS_INTERVAL_STEP <= GUI_SETTINGS_INTERVAL_MAX)
        c.temperatureIntervalMSec += GUI_SETTINGS_INTERVAL_STEP;
}
void decInterval(Configuration &c)
{
    if (c.temperatureIntervalMSec - GUI_SETTINGS_INTERVAL_STEP >= GUI_SETTINGS_INTERVAL_MIN)
        c.temperatureIntervalMSec -= GUI_SETTINGS_INTERVAL_STEP;
}

// PID ENABLE SWITCH ==============================================================================
static String getPIDEnabled(const Configuration &c) { return c.isPIDEnabled ? "Yes" : "No"; }
void incPIDEnabled(Configuration &c) { c.isPIDEnabled = !c.isPIDEnabled; }
void decPIDEnabled(Configuration &c) { c.isPIDEnabled = !c.isPIDEnabled; }

// PID K_PROPORTIONAL =============================================================================
static String getKP(const Configuration &c) { return String(c.kP, GUI_SETTINGS_PID_K_DECIMAL_PLACES); }
void incKP(Configuration &c)
{
    if (c.kP + GUI_SETTINGS_PID_K_STEP <= GUI_SETTINGS_PID_K_MAX)
        c.kP += GUI_SETTINGS_PID_K_STEP;
    else
        c.kP = GUI_SETTINGS_PID_K_MAX;
}
void decKP(Configuration &c)
{
    if (c.kP - GUI_SETTINGS_PID_K_STEP >= GUI_SETTINGS_PID_K_MIN)
        c.kP -= GUI_SETTINGS_PID_K_STEP;
    else
        c.kP = GUI_SETTINGS_PID_K_MIN;
}

// PID K_INTEGRAL =================================================================================
static String getKI(const Configuration &c) { return String(c.kI, GUI_SETTINGS_PID_K_DECIMAL_PLACES); }
void incKI(Configuration &c)
{
    if (c.kI + GUI_SETTINGS_PID_K_STEP <= GUI_SETTINGS_PID_K_MAX)
        c.kI += GUI_SETTINGS_PID_K_STEP;
    else
        c.kI = GUI_SETTINGS_PID_K_MAX;
}
void decKI(Configuration &c)
{
    if (c.kI - GUI_SETTINGS_PID_K_STEP >= GUI_SETTINGS_PID_K_MIN)
        c.kI -= GUI_SETTINGS_PID_K_STEP;
    else
        c.kI = GUI_SETTINGS_PID_K_MIN;
}

// PID K_DERIVATIVE ===============================================================================
static String getKD(const Configuration &c) { return String(c.kD, GUI_SETTINGS_PID_K_DECIMAL_PLACES); }
void incKD(Configuration &c)
{
    if (c.kD + GUI_SETTINGS_PID_K_STEP <= GUI_SETTINGS_PID_K_MAX)
        c.kD += GUI_SETTINGS_PID_K_STEP;
    else
        c.kD = GUI_SETTINGS_PID_K_MAX;
}
void decKD(Configuration &c)
{
    if (c.kD - GUI_SETTINGS_PID_K_STEP >= GUI_SETTINGS_PID_K_MIN)
        c.kD -= GUI_SETTINGS_PID_K_STEP;
    else
        c.kD = GUI_SETTINGS_PID_K_MIN;
}

// BANG BANG TEMPERATURE THRESHOLDS ===============================================================
static String getBangBangBand(const Configuration &c) { return String(c.bangBangHighThreshold - c.bangBangLowThreshold) + " F"; }
void incBangBangBand(Configuration &c)
{
    int band = c.bangBangHighThreshold - c.bangBangLowThreshold;
    if (band + GUI_SETTINGS_TEMP_BB_BAND_STEP <= GUI_SETTINGS_TEMP_BB_BAND_MAX)
    {
        c.bangBangHighThreshold += GUI_SETTINGS_TEMP_BB_BAND_STEP;
        c.bangBangLowThreshold -= GUI_SETTINGS_TEMP_BB_BAND_STEP;
    }
}
void decBangBangBand(Configuration &c)
{
    int band = c.bangBangHighThreshold - c.bangBangLowThreshold;
    if (band - GUI_SETTINGS_TEMP_BB_BAND_STEP >= GUI_SETTINGS_TEMP_BB_BAND_MIN)
    {
        c.bangBangHighThreshold -= GUI_SETTINGS_TEMP_BB_BAND_STEP;
        c.bangBangLowThreshold += GUI_SETTINGS_TEMP_BB_BAND_STEP;
    }
}

// BANG BANG HYSTERESIS ===========================================================================
static String getBangBangHyst(const Configuration &c) { return String(c.bangBangHysteresis) + " F"; }
void incBangBangHyst(Configuration &c)
{
    if (c.bangBangHysteresis + GUI_SETTINGS_TEMP_BB_BAND_STEP <= GUI_SETTINGS_TEMP_BB_BAND_MAX)
        c.bangBangHysteresis += GUI_SETTINGS_TEMP_BB_BAND_STEP;
}
void decBangBangHyst(Configuration &c)
{
    if (c.bangBangHysteresis - GUI_SETTINGS_TEMP_BB_BAND_STEP >= GUI_SETTINGS_TEMP_BB_BAND_MIN)
        c.bangBangHysteresis -= GUI_SETTINGS_TEMP_BB_BAND_STEP;
}

// BANG BANG FAN SPEED ============================================================================
static String getBangBangFan(const Configuration &c) { return String(c.bangBangFanSpeed); }
void incBangBangFan(Configuration &c)
{
    if (c.bangBangFanSpeed + GUI_SETTINGS_PWM_STEP <= GUI_SETTINGS_PWM_MAX)
        c.bangBangFanSpeed += GUI_SETTINGS_PWM_STEP;
    else
        c.bangBangFanSpeed = GUI_SETTINGS_PWM_MAX;
}
void decBangBangFan(Configuration &c)
{
    if (c.bangBangFanSpeed - GUI_SETTINGS_PWM_STEP >= GUI_SETTINGS_PWM_MIN)
        c.bangBangFanSpeed -= GUI_SETTINGS_PWM_STEP;
    else
        c.bangBangFanSpeed = GUI_SETTINGS_PWM_MIN;
}

// DOOR OPEN POSITION =============================================================================
static String getDoorOpenPos(const Configuration &c) { return String(c.doorOpenPosition) + " deg"; }
void incDoorOpenPos(Configuration &c)
{
    if (c.doorOpenPosition < 180)
        c.doorOpenPosition += 1;
}
void decDoorOpenPos(Configuration &c)
{
    if (c.doorOpenPosition > 0)
        c.doorOpenPosition -= 1;
}

// DOOR CLOSE POSITION ============================================================================
static String getDoorClosePos(const Configuration &c) { return String(c.doorClosePosition) + " deg"; }
void incDoorClosePos(Configuration &c)
{
    if (c.doorClosePosition < 180)
        c.doorClosePosition += 1;
}
void decDoorClosePos(Configuration &c)
{
    if (c.doorClosePosition > 0)
        c.doorClosePosition -= 1;
}

// SMOKER THEMOMETER CALIBRATION - GAIN ===========================================================
static String getSmokerGain(const Configuration &c) { return String(c.themometerSmokerGain, 2); }
void incSmokerGain(Configuration &c) { c.themometerSmokerGain += 0.01f; }
void decSmokerGain(Configuration &c) { c.themometerSmokerGain -= 0.01f; }

// SMOKER THEMOMETER CALIBRATION - OFFSET =========================================================
static String getSmokerOffset(const Configuration &c) { return String(c.themometerSmokerOffset, 2); }
void incSmokerOffset(Configuration &c) { c.themometerSmokerOffset += 0.1f; }
void decSmokerOffset(Configuration &c) { c.themometerSmokerOffset -= 0.1f; }

// FOOD THEMOMETER CALIBRATION - GAIN ============================================================
static String getFoodGain(const Configuration &c) { return String(c.themometerFoodGain, 2); }
void incFoodGain(Configuration &c) { c.themometerFoodGain += 0.01f; }
void decFoodGain(Configuration &c) { c.themometerFoodGain -= 0.01f; }

// FOOD THEMOMETER CALIBRATION - OFFSET ==========================================================
static String getFoodOffset(const Configuration &c) { return String(c.themometerFoodOffset, 2); }
void incFoodOffset(Configuration &c) { c.themometerFoodOffset += 0.1f; }
void decFoodOffset(Configuration &c) { c.themometerFoodOffset -= 0.1f; }

// ENABLE MANUAL FAN CONTROL ======================================================================
static String getIsForcedFan(const Configuration &c) { return c.isForcedFanPWM ? "Yes" : "No"; }
void incIsForcedFan(Configuration &c) { c.isForcedFanPWM = !c.isForcedFanPWM; }
void decIsForcedFan(Configuration &c) { c.isForcedFanPWM = !c.isForcedFanPWM; }

// SET MANUAL FAN PWM VALUE =======================================================================
static String getForcedFanPWM(const Configuration &c) { return String(c.forcedFanPWM); }
void incForcedFanPWM(Configuration &c)
{
    if (c.forcedFanPWM + GUI_SETTINGS_PWM_STEP <= GUI_SETTINGS_PWM_MAX)
        c.forcedFanPWM += GUI_SETTINGS_PWM_STEP;
    else
        c.forcedFanPWM = GUI_SETTINGS_PWM_MAX;
}
void decForcedFanPWM(Configuration &c)
{
    if (c.forcedFanPWM - GUI_SETTINGS_PWM_STEP >= GUI_SETTINGS_PWM_MIN)
        c.forcedFanPWM -= GUI_SETTINGS_PWM_STEP;
    else
        c.forcedFanPWM = GUI_SETTINGS_PWM_MIN;
}

// ENABLE MANUAL DOOR CONTROL =====================================================================
static String getIsForcedDoor(const Configuration &c) { return c.isForcedDoorPosition ? "Yes" : "No"; }
void incIsForcedDoor(Configuration &c) { c.isForcedDoorPosition = !c.isForcedDoorPosition; }
void decIsForcedDoor(Configuration &c) { c.isForcedDoorPosition = !c.isForcedDoorPosition; }

// SET MANUAL DOOR POSITION VALUE =================================================================
static String getForcedDoorPos(const Configuration &c) { return String(c.forcedDoorPosition) + " deg"; }
void incForcedDoorPos(Configuration &c)
{
    if (c.forcedDoorPosition < 180)
        c.forcedDoorPosition += 1;
}
void decForcedDoorPos(Configuration &c)
{
    if (c.forcedDoorPosition > 0)
        c.forcedDoorPosition -= 1;
}

static const SettingItem SETTINGS_LIST[] = {
    {"Target Temp", getTargetTemp, incTargetTemp, decTargetTemp},
    {"Contol Interval", getInterval, incInterval, decInterval},

    {"PID Enabled", getPIDEnabled, incPIDEnabled, decPIDEnabled},
    {"PID kP", getKP, incKP, decKP},
    {"PID kI", getKI, incKI, decKI},
    {"PID kD", getKD, incKD, decKD},

    {"BangBang Band", getBangBangBand, incBangBangBand, decBangBangBand},
    {"BangBang Hyst", getBangBangHyst, incBangBangHyst, decBangBangHyst},
    {"BangBang Fan PWM", getBangBangFan, incBangBangFan, decBangBangFan},

    {"Door Open Pos", getDoorOpenPos, incDoorOpenPos, decDoorOpenPos},
    {"Door Close Pos", getDoorClosePos, incDoorClosePos, decDoorClosePos},

    {"T_smoker Gain", getSmokerGain, incSmokerGain, decSmokerGain},
    {"T_smoker Offset", getSmokerOffset, incSmokerOffset, decSmokerOffset},
    {"T_food Gain", getFoodGain, incFoodGain, decFoodGain},
    {"T_food Offset", getFoodOffset, incFoodOffset, decFoodOffset},

    {"Manual Fan Contol", getIsForcedFan, incIsForcedFan, decIsForcedFan},
    {"Forced Fan PWM", getForcedFanPWM, incForcedFanPWM, decForcedFanPWM},
    {"Manual Door Control", getIsForcedDoor, incIsForcedDoor, decIsForcedDoor},
    {"Forced Door Pos", getForcedDoorPos, incForcedDoorPos, decForcedDoorPos},

    {"Exit", nullptr, nullptr, nullptr}};
static constexpr int SETTINGS_COUNT = sizeof(SETTINGS_LIST) / sizeof(SETTINGS_LIST[0]);

// ========================================== PUBLIC METHODS ==========================================

SmokeMateGUI::SmokeMateGUI(Adafruit_ST7789 &displayRef, Configuration &config) : m_tft(displayRef),
                                                                                 m_config(config)
{
    // Initialize the GUI state
    m_guiState.header.state = GUI_STATE_HEADER_STATUS; // Start with the status header
    m_guiState.status.targetTempF = 0;
    m_guiState.status.smokerTempF = 0;
    m_guiState.status.foodTempF = 0;
    m_guiState.isControllerRunning = false; // Start with controller not running
    m_guiState.status.fanPercent = 0;       // Start with fan off
    m_guiState.status.doorPercent = 0;      // Start with door closed
    m_guiState.controllerStartTimeMSec = 0; // Start with zero controller start time
    m_guiState.settings.cursor = 0;         // Start with cursor at the first setting
    m_guiState.settings.scroll = 0;         // Start with scroll at the top
    m_guiState.settings.editingIndex = -1;  // Not editing any setting initially
    // Initialize the history
    m_guiState.history.clear();
}

void SmokeMateGUI::begin()
{
    m_tft.setRotation(1); // Landscape
    m_tft.fillScreen(COLOR_BG);
}

void SmokeMateGUI::service(ulong currentTimeMSec)
{

    // Check if a transition from idle temperature controller to running has occurred
    if (m_guiState.isControllerRunning && !m_isControllerRunning)
    {
        // Reset the history when the controller starts
        m_guiState.history.clear();
        // Reset the last chart update time
        m_chartSampleIntervalMSec = GUI_CHART_UPDATE_INTERVAL_MSEC;
    }

    if (m_guiState.isControllerRunning &&
        (currentTimeMSec - m_lastChartUpdateTimeMSec >= m_chartSampleIntervalMSec))
    {
        m_lastChartUpdateTimeMSec = currentTimeMSec;
        // Push the current state to the history
        m_guiState.history.push_back({currentTimeMSec - m_guiState.controllerStartTimeMSec,
                                      m_guiState.status.smokerTempF,
                                      m_guiState.status.foodTempF,
                                      m_guiState.status.targetTempF});
        // Deal with the temperature history queue, check for overflow
        if (m_guiState.history.size() > GUI_MAX_HISTORY_ENTRIES)
        {
            // Remove the oldest entry if the queue is full
            // state.history.pop_front();

            // Remove every second data point (keep even indices)
            for (size_t i = 1; i < m_guiState.history.size();)
            {
                m_guiState.history.erase(m_guiState.history.begin() + i);
                ++i;
            }
            m_chartSampleIntervalMSec *= 2; // Double the interval
        }
        m_isChartUpdateNeeded = true; // Set the flag to indicate that chart update is needed
    }

    // Update the controller running state
    m_isControllerRunning = m_guiState.isControllerRunning;

    drawHeader(m_guiState.header);
    drawFooter(m_guiState, currentTimeMSec - m_guiState.controllerStartTimeMSec);

    // Draw the status panel or chart based on the header state
    switch (m_guiState.header.state)
    {
    case GUI_STATE_HEADER_STATUS:
        m_isChartUpdateNeeded = true;
        drawStausPanel(m_guiState.status);
        break;

    case GUI_STATE_HEADER_CHART:
        if (m_isChartUpdateNeeded)
        {
            m_isChartUpdateNeeded = false; // Reset the flag after drawing
            drawChartPanel(m_guiState.history);
        }
        break;

    case GUI_STATE_HEADER_SETTINGS:
        m_isChartUpdateNeeded = true;
        drawSettingsPanel(m_guiState);
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT:
        m_isChartUpdateNeeded = true;
        drawSettingsPanel(m_guiState);
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT_VALUE:
        m_isChartUpdateNeeded = true;
        drawSettingsPanel(m_guiState);
        break;
    }
    // drawChart(state.smokerHistory, state.foodHistory);

    m_isCommandQueued = false; // Reset the command queued flag after processing
}

void SmokeMateGUI::updateState(const ControllerStatus &controllerStatus)
{
    // Update the GUI state with the controller status
    m_guiState.isControllerRunning = controllerStatus.isRunning;
    m_guiState.status.smokerTempF = controllerStatus.temperatureSmoker;
    m_guiState.status.foodTempF = controllerStatus.temperatureFood;
    m_guiState.status.targetTempF = controllerStatus.temperatureTarget;
    m_guiState.status.fanPercent = map(controllerStatus.fanPWM, 0, 255, 0, 100);
    m_guiState.status.doorPercent = map(controllerStatus.doorPosition, 0, 180, 0, 100);
    m_guiState.controllerStartTimeMSec = controllerStatus.controllerStartMSec;
}

void SmokeMateGUI::commandMoveNext()
{
    GuiStateHeader &header = m_guiState.header;       // Reference to the current header state
    GuiStateSettings &settings = m_guiState.settings; // Reference to the settings state

    if (m_isCommandQueued)
    {
        return; // Ignore if a command is already queued
    }

    switch (header.state)
    {
    case GUI_STATE_HEADER_STATUS:
        header.state = GUI_STATE_HEADER_CHART; // Move to chart state
        break;
    case GUI_STATE_HEADER_CHART:
        header.state = GUI_STATE_HEADER_SETTINGS; // Move to settings state
        settings.cursor = 0;
        settings.scroll = 0;
        break;
    case GUI_STATE_HEADER_SETTINGS:
        header.state = GUI_STATE_HEADER_STATUS; // Move back to status state
        break;
    case GUI_STATE_HEADER_SETTINGS_EDIT:
        // Scroll settings in edit mode
        if (settings.cursor < SETTINGS_COUNT - 1)
        {
            settings.cursor++;
            if (settings.cursor >= settings.scroll + GUI_SETTINGS_BLOCK_COUNT)
                settings.scroll++;
        }
        else
        {
            // Roll to top
            settings.cursor = 0;
            settings.scroll = 0;
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT_VALUE:
        // Increment the value of the selected setting (not "Exit")
        if (settings.editingIndex >= 0 && settings.editingIndex < SETTINGS_COUNT - 1)
        {
            SETTINGS_LIST[settings.editingIndex].incFunc(m_config);
        }
        break;

    default:
        break;
    }

    m_isCommandQueued = true; // Set the command as queued
}

void SmokeMateGUI::commandMovePrevious()
{
    GuiStateHeader &header = m_guiState.header;       // Reference to the current header state
    GuiStateSettings &settings = m_guiState.settings; // Reference to the settings state

    if (m_isCommandQueued)
    {
        return; // Ignore if a command is already queued
    }

    switch (header.state)
    {
    case GUI_STATE_HEADER_STATUS:
        header.state = GUI_STATE_HEADER_SETTINGS; // Move to settings state
        settings.cursor = 0;
        settings.scroll = 0;
        break;
    case GUI_STATE_HEADER_CHART:
        header.state = GUI_STATE_HEADER_STATUS; // Move back to status state
        break;
    case GUI_STATE_HEADER_SETTINGS:
        header.state = GUI_STATE_HEADER_CHART; // Move to chart state
        break;
    case GUI_STATE_HEADER_SETTINGS_EDIT:
        // Scroll settings in edit mode
        if (settings.cursor > 0)
        {
            settings.cursor--;
            if (settings.cursor < settings.scroll)
                settings.scroll--;
        }
        else
        {
            // Roll to bottom
            settings.cursor = SETTINGS_COUNT - 1;
            settings.scroll = SETTINGS_COUNT > GUI_SETTINGS_BLOCK_COUNT
                                  ? SETTINGS_COUNT - GUI_SETTINGS_BLOCK_COUNT
                                  : 0;
        }
        break;
    case GUI_STATE_HEADER_SETTINGS_EDIT_VALUE:
        // Decrement the value of the selected setting (not "Exit")
        if (settings.editingIndex >= 0 && settings.editingIndex < SETTINGS_COUNT - 1)
        {
            SETTINGS_LIST[settings.editingIndex].decFunc(m_config);
        }
        break;
    default:
        break;
    }

    m_isCommandQueued = true; // Set the command as queued
}

void SmokeMateGUI::commandSelect()
{
    GuiStateHeader &header = m_guiState.header;       // Reference to the current header state
    GuiStateSettings &settings = m_guiState.settings; // Reference to the settings state

    if (m_isCommandQueued)
    {
        return; // Ignore if a command is already queued
    }

    switch (header.state)
    {
    case GUI_STATE_HEADER_STATUS:
        // Do nothing on command in the status state
        break;

    case GUI_STATE_HEADER_CHART:
        // Do nothing on command in the chart state
        break;

    case GUI_STATE_HEADER_SETTINGS:
        header.state = GUI_STATE_HEADER_SETTINGS_EDIT; // Move to settings edit state
        settings.cursor = 0;
        settings.scroll = 0;
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT:
        // WIP: Handle settings edit state
        // Here you would typically handle the settings editing logic
        // For now, we will just reset to the settings state
        // If "Exit" is selected, leave edit mode
        if (settings.cursor == SETTINGS_COUNT - 1)
        {
            header.state = GUI_STATE_HEADER_SETTINGS;
        }
        else
        {
            header.state = GUI_STATE_HEADER_SETTINGS_EDIT_VALUE; // Move to settings edit value state
            settings.editingIndex = settings.cursor;             // Set the index of the setting being edited
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT_VALUE:
        header.state = GUI_STATE_HEADER_SETTINGS_EDIT; // Move back to settings edit state
        settings.editingIndex = -1;                    // Reset the editing index
        break;

    default:
        // If we reach here, it means we are in an unexpected state
        break;
    }

    m_isCommandQueued = true; // Set the command as queued
}

// ========================================== PRIVATE METHODS ==========================================

void SmokeMateGUI::drawHeaderBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                                   uint16_t textOffset, const char *text, uint16_t color)
{

    m_tft.fillRect(x, y, width, height, color);
    m_tft.setTextSize(2);
    m_tft.setTextColor(COLOR_TEXT);
    m_tft.setCursor(x + textOffset, y + 13);
    m_tft.print(text);
}

void SmokeMateGUI::drawHeader(const GuiStateHeader &header)
{
    uint16_t settingsColor;
    if (header.state == GUI_STATE_HEADER_SETTINGS_EDIT || header.state == GUI_STATE_HEADER_SETTINGS_EDIT_VALUE)
    {
        settingsColor = COLOR_HEADER_SELECTED; // Highlight the settings block when editing
    }
    else
    {
        settingsColor = (header.state == GUI_STATE_HEADER_SETTINGS) ? COLOR_HEADER_ACTIVE : COLOR_HEADER_PASSIVE;
    }

    // Draw the header blocks
    drawHeaderBlock(0, 0,
                    GUI_HEADER_BLOCK_WIDTH, GUI_HEADER_HEIGHT, 19, "STATUS",
                    header.state == GUI_STATE_HEADER_STATUS ? COLOR_HEADER_ACTIVE : COLOR_HEADER_PASSIVE);
    drawHeaderBlock(GUI_HEADER_BLOCK_WIDTH + 1, 0,
                    GUI_HEADER_BLOCK_WIDTH, GUI_HEADER_HEIGHT, 25, "CHART",
                    header.state == GUI_STATE_HEADER_CHART ? COLOR_HEADER_ACTIVE : COLOR_HEADER_PASSIVE);
    drawHeaderBlock(2 * GUI_HEADER_BLOCK_WIDTH + 2, 0,
                    GUI_HEADER_BLOCK_WIDTH, GUI_HEADER_HEIGHT, 7, "SETTINGS",
                    settingsColor);
}

void SmokeMateGUI::drawFooter(const GuiState &state, ulong elapsedMillis)
{
    char timeStr[9];
    if (state.isControllerRunning)
    {

        m_tft.fillRect(0, GUI_FOOTER_OFFSET, SCREEN_WIDTH, GUI_FOOTER_HEIGHT, COLOR_HEADER_ACTIVE);
        m_tft.setTextSize(2);
        m_tft.setTextColor(COLOR_TEXT);

        m_tft.setCursor(6, GUI_FOOTER_OFFSET + 2);
        m_tft.print("Running");

        // Display the elapsed time in HH:MM:SS format
        m_tft.setCursor(SCREEN_WIDTH / 2 + 40, GUI_FOOTER_OFFSET + 2);

        unsigned long totalSeconds = elapsedMillis / 1000;
        unsigned long hours = totalSeconds / 3600;
        unsigned long minutes = (totalSeconds % 3600) / 60;
        unsigned long seconds = totalSeconds % 60;
        snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, seconds);
        m_tft.print(timeStr);
    }
    else
    {

        m_tft.fillRect(0, GUI_FOOTER_OFFSET, SCREEN_WIDTH, GUI_FOOTER_HEIGHT, COLOR_HEADER_PASSIVE);
        m_tft.setTextSize(2);
        m_tft.setTextColor(COLOR_TEXT);

        m_tft.setCursor(6, GUI_FOOTER_OFFSET + 2);
        m_tft.print("Idle");

        // Display the elapsed time in HH:MM:SS format
        m_tft.setCursor(SCREEN_WIDTH / 2 + 40, GUI_FOOTER_OFFSET + 2);

        snprintf(timeStr, sizeof(timeStr), "00:00:00");
        m_tft.print(timeStr);
    }

    // Additional footer information can be added here
}

void SmokeMateGUI::drawStausPanel(const GuiStateStatus &state)
{
    char foodTempStr[16];
    char smokerTempStr[16];
    char targetTempStr[16];
    char fanSpeedStr[16];
    char doorPosition[16];

    // Serial debugging output
    Serial.print("Smoker Temp");
    Serial.println(state.smokerTempF);
    Serial.print("Food Temp");
    Serial.println(state.foodTempF);
    Serial.print("Target Temp");
    Serial.println(state.targetTempF);
    Serial.print("Fan Speed");
    Serial.println(state.fanPercent);
    Serial.print("Door Position");
    Serial.println(state.doorPercent);

    // SMOKER TEMPERATURE ===================================================
    snprintf(smokerTempStr, sizeof(smokerTempStr), "%d F", state.smokerTempF);
    drawStatusLine(1, "Smoker", smokerTempStr);

    // TARGET TEMPERATURE ====================================================
    snprintf(targetTempStr, sizeof(targetTempStr), "%d F", state.targetTempF);
    drawStatusLine(2, "Target", targetTempStr);

    // FOOD TEMPERATURE ====================================================
    snprintf(foodTempStr, sizeof(foodTempStr), "%d F", state.foodTempF);
    drawStatusLine(3, "Food", foodTempStr);

    // FAN SPEED ====================================================
    snprintf(fanSpeedStr, sizeof(fanSpeedStr), state.fanPercent <= 0 ? "OFF" : "%d %%", state.fanPercent);
    drawStatusLine(4, "Fan", fanSpeedStr);

    // DOOR POSITION ====================================================
    if (state.doorPercent > 0 || state.doorPercent < 100)
    {
        snprintf(doorPosition, sizeof(doorPosition), "%d %%", state.doorPercent);
    }
    else
    {
        snprintf(doorPosition, sizeof(doorPosition), state.doorPercent <= 0 ? "CLOSED" : "OPEN");
    }
    drawStatusLine(5, "Door", doorPosition);
}

void SmokeMateGUI::drawStatusLine(uint16_t n, const char *label, const char *value)
{
    drawStatusBlock(0, GUI_STATUS_PANEL_Y_OFFSET + (n - 1) * GUI_STATUS_PANEL_BLOCK_HEIGHT,
                    GUI_STATUS_PANEL_BLOCK_WIDTH, GUI_STATUS_PANEL_BLOCK_HEIGHT,
                    label);
    drawStatusBlock(GUI_STATUS_PANEL_BLOCK_WIDTH, GUI_STATUS_PANEL_Y_OFFSET + (n - 1) * GUI_STATUS_PANEL_BLOCK_HEIGHT,
                    GUI_STATUS_PANEL_BLOCK_WIDTH, GUI_STATUS_PANEL_BLOCK_HEIGHT,
                    value);
}

void SmokeMateGUI::drawStatusBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const char *text)
{
    m_tft.fillRect(x, y, width, height, COLOR_BG);
    m_tft.setTextSize(3);
    m_tft.setTextColor(COLOR_TEXT);
    m_tft.setCursor(x + 6, y + 6);
    m_tft.print(text);
}

void SmokeMateGUI::drawChartPanel(const std::deque<TemperatureHistoryEntry> &history)
{
    const int chartX = 20;
    const int chartY = GUI_CHART_PANEL_Y_OFFSET + 10;
    const int width = SCREEN_WIDTH - chartX - 10;
    const int height = GUI_CHART_PANEL_HEIGHT - 30;

    // Clear the chart area as defined by the chart panel
    m_tft.fillRect(0, GUI_CHART_PANEL_Y_OFFSET, SCREEN_WIDTH, GUI_CHART_PANEL_HEIGHT, COLOR_BG);

    // Draw chart background and border
    m_tft.drawRect(chartX, chartY, width, height, ST77XX_WHITE);

    // Title
    m_tft.setCursor(chartX, chartY - 12);
    m_tft.setTextColor(ST77XX_WHITE);
    m_tft.setTextSize(2);

    if (history.size() < 2)
        return;

    // Find min/max for scaling
    int minT = 999, maxT = -999;
    ulong minTime = history.front().timestampMSec;
    ulong maxTime = history.back().timestampMSec;

    for (const auto &entry : history)
    {
        minT = std::min({minT, entry.smokerTempF, entry.foodTempF, entry.targetTempF});
        maxT = std::max({maxT, entry.smokerTempF, entry.foodTempF, entry.targetTempF});
    }

    // Pad by 5°F and round to nearest floor/ceiling ending in 5
    minT = ((minT - 5) / 5) * 5;     // Floor to nearest 5
    maxT = ((maxT + 5 + 4) / 5) * 5; // Ceil to nearest 5

    for (const auto &entry : history)
    {
        minT = std::min({minT, entry.smokerTempF, entry.foodTempF, entry.targetTempF});
        maxT = std::max({maxT, entry.smokerTempF, entry.foodTempF, entry.targetTempF});
    }

    if (maxT - minT < 10)
    {
        minT -= 5;
        maxT += 5;
    }

    // --- Draw Y axis (Temperature) ---
    m_tft.setTextSize(1);
    m_tft.setTextColor(ST77XX_WHITE);
    int yTicks = 5;
    for (int i = 0; i <= yTicks; ++i)
    {
        int temp = minT + (maxT - minT) * i / yTicks;
        int y = chartY + height - ((height - 1) * i) / yTicks;
        m_tft.drawFastHLine(chartX, y, 4, ST77XX_WHITE); // Tick
        m_tft.setCursor(chartX - 20, y - 4);
        m_tft.printf("%3d", temp);
    }
    // Y-axis label
    m_tft.setCursor(chartX - 32, chartY + height / 2 - 8);
    m_tft.print("F");

    // --- Draw X axis (Time in minutes or hours) ---
    int xTicks = 5;
    ulong durationMSec = maxTime - minTime;
    bool showHours = (maxTime > GUI_CHART_MIN2HOUR_SWITCH_MSEC);

    for (int i = 0; i <= xTicks; ++i)
    {
        ulong t = minTime + (durationMSec * i) / xTicks;
        int x = chartX + ((width - 1) * i) / xTicks;
        m_tft.drawFastVLine(x, chartY + height - 4, 4, ST77XX_WHITE); // Tick

        int cursorOffset = 0;
        if (i == 0)
        {
            cursorOffset = 0;
        }
        else if (i == xTicks - 1)
        {
            cursorOffset = -20;
        }
        else
        {
            cursorOffset = -10;
        }
        m_tft.setCursor(x + cursorOffset, chartY + height + 2);
        if (showHours)
        {
            float hours = t / 3600000.0f;
            m_tft.printf("%.2f", hours);
        }
        else
        {
            float minutes = t / 60000.0f;
            m_tft.printf("%.1f", minutes);
        }
    }
    // X-axis label
    m_tft.setCursor(chartX + width - 24, chartY + height - 20);
    if (showHours)
    {
        m_tft.print("hrs");
    }
    else
    {
        m_tft.print("min");
    }

    // Y gridlines
    for (int i = 1; i < yTicks; ++i)
    {
        int y = chartY + height - ((height - 1) * i) / yTicks;
        m_tft.drawFastHLine(chartX + 1, y, width - 2, COLOR_CHART_GRIDLINES);
    }

    // X gridlines
    for (int i = 1; i < xTicks; ++i)
    {
        int x = chartX + ((width - 1) * i) / xTicks;
        m_tft.drawFastVLine(x, chartY + 1, height - 2, COLOR_CHART_GRIDLINES);
    }

    // --- Draw Legend (top right corner) ---
    int legendX = chartX + width - 50;
    int legendY = chartY + 8;
    m_tft.setTextSize(1);

    m_tft.setCursor(legendX, legendY);
    m_tft.setTextColor(COLOR_CHART_SMOKER);
    m_tft.print("Smoker");

    m_tft.setCursor(legendX, legendY + 12);
    m_tft.setTextColor(COLOR_CHART_FOOD);
    m_tft.print("Food");

    m_tft.setCursor(legendX, legendY + 24);
    m_tft.setTextColor(COLOR_CHART_TARGET);
    m_tft.print("Target");

    m_tft.setTextColor(ST77XX_WHITE); // Reset to default for other text

    // Draw lines for smoker, food, and target temps
    for (size_t i = 1; i < history.size(); ++i)
    {
        // X scaling
        int x = chartX + ((history[i].timestampMSec - minTime) * (width - 1)) / (maxTime - minTime ? maxTime - minTime : 1);
        int x0 = chartX + ((history[i - 1].timestampMSec - minTime) * (width - 1)) / (maxTime - minTime ? maxTime - minTime : 1);
        int x1 = chartX + ((history[i].timestampMSec - minTime) * (width - 1)) / (maxTime - minTime ? maxTime - minTime : 1);

        // Y scaling (invert so higher temps are higher up)
        int y0s = chartY + height - 1 - ((history[i - 1].smokerTempF - minT) * (height - 1)) / (maxT - minT);
        int y1s = chartY + height - 1 - ((history[i].smokerTempF - minT) * (height - 1)) / (maxT - minT);

        int y0f = chartY + height - 1 - ((history[i - 1].foodTempF - minT) * (height - 1)) / (maxT - minT);
        int y1f = chartY + height - 1 - ((history[i].foodTempF - minT) * (height - 1)) / (maxT - minT);

        int y0t = chartY + height - 1 - ((history[i - 1].targetTempF - minT) * (height - 1)) / (maxT - minT);
        int y1t = chartY + height - 1 - ((history[i].targetTempF - minT) * (height - 1)) / (maxT - minT);

        int ySmoker = chartY + height - 1 - ((history[i].smokerTempF - minT) * (height - 1)) / (maxT - minT);
        int yFood = chartY + height - 1 - ((history[i].foodTempF - minT) * (height - 1)) / (maxT - minT);
        // int yTarget = chartY + height - 1 - ((history[i].targetTempF - minT) * (height - 1)) / (maxT - minT);

        m_tft.drawLine(x0, y0s, x1, y1s, COLOR_CHART_SMOKER); // Smoker temp
        m_tft.fillCircle(x, ySmoker, 2, COLOR_CHART_SMOKER);
        // tft.drawLine(x0, y0s - 1, x1, y1s - 1, COLOR_CHART_SMOKER); // Smoker temp
        // tft.drawLine(x0, y0s + 1, x1, y1s + 1, COLOR_CHART_SMOKER); // Smoker temp
        m_tft.drawLine(x0, y0f, x1, y1f, COLOR_CHART_FOOD); // Food temp
        m_tft.fillCircle(x, yFood, 2, COLOR_CHART_FOOD);
        // tft.drawLine(x0, y0f - 1, x1, y1f - 1, COLOR_CHART_FOOD);   // Food temp
        // tft.drawLine(x0, y0f + 1, x1, y1f + 1, COLOR_CHART_FOOD);   // Food temp
        m_tft.drawLine(x0, y0t, x1, y1t, COLOR_CHART_TARGET); // Target temp
    }
}

void SmokeMateGUI::drawSettingsPanel(const GuiState &state)
{

    m_tft.fillRect(0, GUI_SETTINGS_PANEL_Y_OFFSET, SCREEN_WIDTH, GUI_SETTINGS_PANEL_HEIGHT, COLOR_BG);

    // Set text color and size
    m_tft.setTextColor(COLOR_TEXT);
    m_tft.setTextSize(2);

    int startIdx = state.settings.scroll;
    int endIdx = min(startIdx + GUI_SETTINGS_BLOCK_COUNT, SETTINGS_COUNT);

    for (int i = startIdx; i < endIdx; ++i)
    {
        int blockY = GUI_SETTINGS_PANEL_Y_OFFSET + (i - startIdx) * GUI_SETTINGS_BLOCK_HEIGHT;

        // Decide highlight color based on mode
        bool isSelected = (i == state.settings.cursor);
        bool isEditing = (state.header.state == GUI_STATE_HEADER_SETTINGS_EDIT ||
                          state.header.state == GUI_STATE_HEADER_SETTINGS_EDIT_VALUE);

        if (isSelected && isEditing)
        {
            if (state.header.state == GUI_STATE_HEADER_SETTINGS_EDIT_VALUE)
            {
                // Edit mode: highlight with SELECTED color
                m_tft.fillRect(0, blockY + 1, SCREEN_WIDTH, GUI_SETTINGS_BLOCK_HEIGHT - 1, COLOR_HEADER_SELECTED);
            }
            else
            {
                // Navigation mode: highlight with PASSIVE color
                m_tft.fillRect(0, blockY + 1, SCREEN_WIDTH, GUI_SETTINGS_BLOCK_HEIGHT - 1, COLOR_HEADER_PASSIVE);
            }
        }

        m_tft.setCursor(GUI_SETTINGS_LABEL_OFFSET, blockY + 2);
        m_tft.setTextColor(COLOR_TEXT);
        m_tft.print(SETTINGS_LIST[i].label);

        if (SETTINGS_LIST[i].getValue)
        {
            m_tft.setCursor(GUI_SETTINGS_VALUE_OFFSET, blockY + 2);
            m_tft.print(SETTINGS_LIST[i].getValue(m_config));
        }
    }
}