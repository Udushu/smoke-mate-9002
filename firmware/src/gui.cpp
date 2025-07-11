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

// TEMPERATURE PROFILING ENABLE SWITCH ============================================================
static String getIsTemperatureProfilingEnabled(const Configuration &c) { return c.isTemperatureProfilingEnabled ? "Yes" : "No"; }
void incIsTemperatureProfilingEnabled(Configuration &c) { c.isTemperatureProfilingEnabled = !c.isTemperatureProfilingEnabled; }
void decIsTemperatureProfilingEnabled(Configuration &c) { c.isTemperatureProfilingEnabled = !c.isTemperatureProfilingEnabled; }
// TEMPERATURE PROFILE STEPS COUNT ================================================================
static String getTemperatureProfileStepsCount(const Configuration &c)
{
    return String(c.temperatureProfileStepsCount) + " steps";
}
void incTemperatureProfileStepsCount(Configuration &c)
{
    if (c.temperatureProfileStepsCount < MAX_PROFILE_STEPS)
        c.temperatureProfileStepsCount++;
}
void decTemperatureProfileStepsCount(Configuration &c)
{
    if (c.temperatureProfileStepsCount > 0)
        c.temperatureProfileStepsCount--;
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

// TEMPERATURE FILTER ENABLE SWITCH ============================================================
static String getIsTemperatureFilterEnabled(const Configuration &c) { return c.isTemperatureFilterEnabled ? "Yes" : "No"; }
void incIsTemperatureFilterEnabled(Configuration &c) { c.isTemperatureFilterEnabled = !c.isTemperatureFilterEnabled; }
void decIsTemperatureFilterEnabled(Configuration &c) { c.isTemperatureFilterEnabled = !c.isTemperatureFilterEnabled; }

// TEMPERATURE FILTER COEFFICIENT ================================================================
static String getTemperatureFilterCoeff(const Configuration &c)
{
    return String(c.temperatureFilterCoeff, 2); // Display with 2 decimal places
}
void incTemperatureFilterCoeff(Configuration &c)
{
    if (c.temperatureFilterCoeff + 0.01f <= 1.0f)
        c.temperatureFilterCoeff += 0.05f;
    else
        c.temperatureFilterCoeff = 1.0f; // Cap at 1.0
}
void decTemperatureFilterCoeff(Configuration &c)
{
    if (c.temperatureFilterCoeff - 0.01f >= 0.0f)
        c.temperatureFilterCoeff -= 0.05f;
    else
        c.temperatureFilterCoeff = 0.0f; // Cap at 0.0
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

static String getIsThemometerSimulated(const Configuration &c) { return c.isThemometerSimulated ? "Yes" : "No"; }
void incIsThemometerSimulated(Configuration &c) { c.isThemometerSimulated = !c.isThemometerSimulated; }
void decIsThemometerSimulated(Configuration &c) { c.isThemometerSimulated = !c.isThemometerSimulated; }

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

// ENABLE MANUAL DOOR CONTROL =====================================================================
static String getIsWifiEnabled(const Configuration &c) { return c.isWiFiEnabled ? "Yes" : "No"; }
void incIsWifiEnabled(Configuration &c) { c.isWiFiEnabled = !c.isWiFiEnabled; }
void decIsWifiEnabled(Configuration &c) { c.isWiFiEnabled = !c.isWiFiEnabled; }

const char *WIFI_CHAR_SET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+";
const int wifiCharSetLen = strlen(WIFI_CHAR_SET);

static const SettingItem SETTINGS_LIST[] = {
    {"Target Temp", getTargetTemp, incTargetTemp, decTargetTemp},
    {"Contol Interval", getInterval, incInterval, decInterval},
    {"Temp Profiling", getIsTemperatureProfilingEnabled, incIsTemperatureProfilingEnabled, decIsTemperatureProfilingEnabled},
    {"Profile Steps", getTemperatureProfileStepsCount, incTemperatureProfileStepsCount, decTemperatureProfileStepsCount},

    {"Edit Temp Profiles", nullptr, nullptr, nullptr},

    {"PID Enabled", getPIDEnabled, incPIDEnabled, decPIDEnabled},
    {"PID kP", getKP, incKP, decKP},
    {"PID kI", getKI, incKI, decKI},
    {"PID kD", getKD, incKD, decKD},

    {"BangBang Band", getBangBangBand, incBangBangBand, decBangBangBand},
    {"BangBang Hyst", getBangBangHyst, incBangBangHyst, decBangBangHyst},
    {"BangBang Fan PWM", getBangBangFan, incBangBangFan, decBangBangFan},

    {"Door Open Pos", getDoorOpenPos, incDoorOpenPos, decDoorOpenPos},
    {"Door Close Pos", getDoorClosePos, incDoorClosePos, decDoorClosePos},

    {"Temp Filter", getIsTemperatureFilterEnabled, incIsTemperatureFilterEnabled, decIsTemperatureFilterEnabled},
    {"Temp Filter Coeff", getTemperatureFilterCoeff, incTemperatureFilterCoeff, decTemperatureFilterCoeff},

    {"T_smoker Gain", getSmokerGain, incSmokerGain, decSmokerGain},
    {"T_smoker Offset", getSmokerOffset, incSmokerOffset, decSmokerOffset},
    {"T_food Gain", getFoodGain, incFoodGain, decFoodGain},
    {"T_food Offset", getFoodOffset, incFoodOffset, decFoodOffset},

    {"Sim. Themometer", getIsThemometerSimulated, incIsThemometerSimulated, decIsThemometerSimulated},

    {"Manual Fan", getIsForcedFan, incIsForcedFan, decIsForcedFan},
    {"Forced Fan PWM", getForcedFanPWM, incForcedFanPWM, decForcedFanPWM},
    {"Manual Door", getIsForcedDoor, incIsForcedDoor, decIsForcedDoor},
    {"Forced Door Pos", getForcedDoorPos, incForcedDoorPos, decForcedDoorPos},

    {"Enable WiFi", getIsWifiEnabled, incIsWifiEnabled, decIsWifiEnabled},
    {"WiFi SSID", nullptr, nullptr, nullptr},
    {"WiFi Password", nullptr, nullptr, nullptr},

    {"Reboot", nullptr, nullptr, nullptr},

    {"Exit", nullptr, nullptr, nullptr}};

static constexpr int SETTINGS_COUNT = sizeof(SETTINGS_LIST) / sizeof(SETTINGS_LIST[0]);
static constexpr int SETTINGS_TEMP_PROFILE_START_INDEX = 4;             // Index of the first temperature profile setting
static constexpr int SETTINGS_WIFI_SSID_INDEX = SETTINGS_COUNT - 4;     // Index of WiFi SSID setting
static constexpr int SETTINGS_WIFI_PASSWORD_INDEX = SETTINGS_COUNT - 3; // Index of WiFi Password setting
static constexpr int SETTINGS_REBOOT_INDEX = SETTINGS_COUNT - 2;        // Index of WiFi Password setting
static constexpr int SETTINGS_EXIT_INDEX = SETTINGS_COUNT - 1;          // Index of the last editable setting

// ========================================== TEMP PROFILE SETTINGS LIST ==============================
static String getTempProfileStepType(const TempProfileStep &step)
{
    return step.type == TEMP_PROFILE_TYPE_RAMP ? "Ramp" : "Dwell";
}
void incTempProfileStepType(TempProfileStep &step)
{
    step.type = (step.type == TEMP_PROFILE_TYPE_RAMP) ? TEMP_PROFILE_TYPE_DWELL : TEMP_PROFILE_TYPE_RAMP;
}
void decTempProfileStepType(TempProfileStep &step)
{
    step.type = (step.type == TEMP_PROFILE_TYPE_RAMP) ? TEMP_PROFILE_TYPE_DWELL : TEMP_PROFILE_TYPE_RAMP;
}

static String getTempProfileDuration(const TempProfileStep &step)
{
    return String(step.timeMSec / (60 * 1000)) + " min";
}
void incTempProfileDuration(TempProfileStep &step)
{
    if (step.timeMSec + GUI_SETTINGS_TEMP_PROFILE_DURATION_STEP <= GUI_SETTINGS_TEMP_PROFILE_DURATION_MAX)
    {
        int dt = step.timeMSec >= 10 * GUI_SETTINGS_TEMP_PROFILE_DURATION_STEP
                     ? GUI_SETTINGS_TEMP_PROFILE_DURATION_STEP * 10
                     : GUI_SETTINGS_TEMP_PROFILE_DURATION_STEP;
        step.timeMSec += dt;
    }
    else
        step.timeMSec = GUI_SETTINGS_TEMP_PROFILE_DURATION_MAX;
}
void decTempProfileDuration(TempProfileStep &step)
{
    if (step.timeMSec - GUI_SETTINGS_TEMP_PROFILE_DURATION_STEP >= GUI_SETTINGS_TEMP_PROFILE_DURATION_MIN)
    {
        int dt = step.timeMSec > 10 * GUI_SETTINGS_TEMP_PROFILE_DURATION_STEP
                     ? GUI_SETTINGS_TEMP_PROFILE_DURATION_STEP * 10
                     : GUI_SETTINGS_TEMP_PROFILE_DURATION_STEP;
        step.timeMSec -= dt;
    }
    else
        step.timeMSec = GUI_SETTINGS_TEMP_PROFILE_DURATION_MIN;
}
static String getTempProfileT1(const TempProfileStep &step)
{
    return String(step.temperatureStartF) + " F";
}
void incTempProfileT1(TempProfileStep &step)
{
    if (step.temperatureStartF + GUI_SETTINGS_TEMP_STEP <= GUI_SETTINGS_TEMP_MAX)
        step.temperatureStartF += GUI_SETTINGS_TEMP_STEP;
    else
        step.temperatureStartF = GUI_SETTINGS_TEMP_MAX;
}
void decTempProfileT1(TempProfileStep &step)
{
    if (step.temperatureStartF - GUI_SETTINGS_TEMP_STEP >= GUI_SETTINGS_TEMP_MIN)
        step.temperatureStartF -= GUI_SETTINGS_TEMP_STEP;
    else
        step.temperatureStartF = GUI_SETTINGS_TEMP_MIN;
}
static String getTempProfileT2(const TempProfileStep &step)
{
    return String(step.temperatureEndF) + " F";
}
void incTempProfileT2(TempProfileStep &step)
{
    if (step.temperatureEndF + GUI_SETTINGS_TEMP_STEP <= GUI_SETTINGS_TEMP_MAX)
        step.temperatureEndF += GUI_SETTINGS_TEMP_STEP;
    else
        step.temperatureEndF = GUI_SETTINGS_TEMP_MAX;
}
void decTempProfileT2(TempProfileStep &step)
{
    if (step.temperatureEndF - GUI_SETTINGS_TEMP_STEP >= GUI_SETTINGS_TEMP_MIN)
        step.temperatureEndF -= GUI_SETTINGS_TEMP_STEP;
    else
        step.temperatureEndF = GUI_SETTINGS_TEMP_MIN;
}

static const TempProfileItem TEMPPROFILE_SETTINGS_LIST[] = {
    {"Type", getTempProfileStepType, incTempProfileStepType, decTempProfileStepType},
    {"Duration", getTempProfileDuration, incTempProfileDuration, decTempProfileDuration},
    {"Start Temp", getTempProfileT1, incTempProfileT1, decTempProfileT1},
    {"End Temp", getTempProfileT2, incTempProfileT2, decTempProfileT2}

};
static constexpr int TEMPPROFILE_SETTINGS_COUNT = sizeof(TEMPPROFILE_SETTINGS_LIST) / sizeof(TEMPPROFILE_SETTINGS_LIST[0]);

// ========================================== INTERNAL HELPER METHODS ====================================
bool operator==(const GuiStateHeader &a, const GuiStateHeader &b)
{
    return a.state == b.state;
}

bool operator!=(const GuiStateHeader &a, const GuiStateHeader &b)
{
    return !(a == b);
}

bool operator==(const GuiStateStatus &a, const GuiStateStatus &b)
{
    return a.smokerTempF == b.smokerTempF &&
           a.foodTempF == b.foodTempF &&
           a.targetTempF == b.targetTempF &&
           a.fanPercent == b.fanPercent &&
           a.doorPercent == b.doorPercent;
}

bool operator!=(const GuiStateStatus &a, const GuiStateStatus &b)
{
    return !(a == b);
}

bool operator==(const GuiStateSettings &a, const GuiStateSettings &b)
{
    return a.cursor == b.cursor &&
           a.scroll == b.scroll &&
           a.editingIndex == b.editingIndex &&
           a.wifiSSIDIndex == b.wifiSSIDIndex &&
           a.wifiPasswordCharPos == b.wifiPasswordCharPos &&
           a.wifiPasswordCharIdx == b.wifiPasswordCharIdx;
}

bool operator!=(const GuiStateSettings &a, const GuiStateSettings &b)
{
    return !(a == b);
}

bool operator==(const GuiStateTempProfile &a, const GuiStateTempProfile &b)
{
    return a.cursor == b.cursor &&
           a.scroll == b.scroll &&
           a.editingIndex == b.editingIndex;
}

bool operator!=(const GuiStateTempProfile &a, const GuiStateTempProfile &b)
{
    return !(a == b);
}

bool operator==(const GuiStateTempProfileEdit &a, const GuiStateTempProfileEdit &b)
{
    return a.stepIndex == b.stepIndex &&
           a.field == b.field &&
           a.editing == b.editing;
}

bool operator!=(const GuiStateTempProfileEdit &a, const GuiStateTempProfileEdit &b)
{
    return !(a == b);
}

// ========================================== DEBUG METHODS ==========================================
inline const char *guiStateHeaderToString(GUI_STATE_ACTIVE_HEADER state)
{
    switch (state)
    {
    case GUI_STATE_HEADER_STATUS:
        return "STATUS";
    case GUI_STATE_HEADER_CHART:
        return "CHART";
    case GUI_STATE_HEADER_SETTINGS:
        return "SETTINGS";
    case GUI_STATE_HEADER_SETTINGS_EDIT:
        return "SETTINGS_EDIT";
    case GUI_STATE_HEADER_SETTINGS_EDIT_VALUE:
        return "SETTINGS_EDIT_VALUE";
    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_SSID:
        return "SETTINGS_EDIT_WIFI_SSID";
    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_PASSWORD:
        return "SETTINGS_EDIT_WIFI_PASSWORD";
    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE:
        return "TEMP_PROFILE";
    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP:
        return "TEMP_PROFILE_STEP";
    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP_EDIT:
        return "TEMP_PROFILE_STEP_EDIT";
    default:
        return "UNKNOWN";
    }
}

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

    m_prevGuiState = m_guiState; // Initialize previous state to current state

    m_wifiNetworkSSIDs.clear(); // Clear the WiFi SSID list
    m_wifiNetworkRSSIs.clear(); // Clear the WiFi RSSI list
}

void SmokeMateGUI::begin()
{
    m_tft.setRotation(1); // Landscape
    m_tft.fillScreen(COLOR_BG);
}

void SmokeMateGUI::service(ulong currentTimeMSec)
{

    manageTempChartState(currentTimeMSec);

    // Update the controller running state
    m_isControllerRunning = m_guiState.isControllerRunning;

    if (m_guiState.header != m_prevGuiState.header || m_isFirstHeaderRender)
    {
        drawHeader(m_guiState.header);
        m_isFirstHeaderRender = false; // Reset the first render flag after drawing the header
    }

    drawFooter(m_guiState, currentTimeMSec - m_guiState.controllerStartTimeMSec);

    // Draw the status panel or chart based on the header state
    switch (m_guiState.header.state)
    {
    case GUI_STATE_HEADER_STATUS:
        m_isChartUpdateNeeded = true;
        if ((m_guiState.status != m_prevGuiState.status) || m_isForcedGUIUpdate)
        {
            m_isForcedGUIUpdate = false; // Reset the forced update flag
            drawStausPanel(m_guiState.status);
        }
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
        if ((m_guiState.settings != m_prevGuiState.settings) || m_isForcedGUIUpdate)
        {
            m_isForcedGUIUpdate = false; // Reset the forced update flag
            drawSettingsPanel(m_guiState);
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT:
        m_isChartUpdateNeeded = true;
        if ((m_guiState.settings != m_prevGuiState.settings) || m_isForcedGUIUpdate)
        {
            m_isForcedGUIUpdate = false; // Reset the forced update flag
            drawSettingsPanel(m_guiState);
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT_VALUE:
        m_isChartUpdateNeeded = true;
        if ((m_guiState.settings != m_prevGuiState.settings) || m_isForcedGUIUpdate)
        {
            m_isForcedGUIUpdate = false; // Reset the forced update flag
            drawSettingsPanel(m_guiState);
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_SSID:
        // Draw WiFi SSID selection panel
        m_isChartUpdateNeeded = true;
        if ((m_guiState.settings != m_prevGuiState.settings) || m_isForcedGUIUpdate)
        {
            m_isForcedGUIUpdate = false; // Reset the forced update flag
            drawWiFiSelectPanel();
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_PASSWORD:
        // Draw WiFi password entry panel
        m_isChartUpdateNeeded = true;
        if ((m_guiState.settings != m_prevGuiState.settings) || m_isForcedGUIUpdate)
        {
            m_isForcedGUIUpdate = false; // Reset the forced update flag
            drawWiFiPasswordPanel();
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE:
        // Draw temperature profile settings panel
        m_isChartUpdateNeeded = true;
        if ((m_guiState.tempProfile != m_prevGuiState.tempProfile) || m_isForcedGUIUpdate)
        {
            m_isForcedGUIUpdate = false; // Reset the forced update flag
            drawTempProfilePanel(m_guiState.tempProfile);
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP:
        // Draw temperature profile step edit panel
        m_isChartUpdateNeeded = true;
        if ((m_guiState.tempProfileEdit != m_prevGuiState.tempProfileEdit) || m_isForcedGUIUpdate)
        {
            m_isForcedGUIUpdate = false; // Reset the forced update flag
            drawTempProfileStepPanel(m_guiState.tempProfile, m_guiState.tempProfileEdit);
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP_EDIT:
        // Draw temperature profile step edit panel
        m_isChartUpdateNeeded = true;
        if ((m_guiState.tempProfileEdit != m_prevGuiState.tempProfileEdit) || m_isForcedGUIUpdate)
        {
            m_isForcedGUIUpdate = false; // Reset the forced update flag
            drawTempProfileStepPanel(m_guiState.tempProfile, m_guiState.tempProfileEdit);
        }
        break;
    }
    // drawChart(state.smokerHistory, state.foodHistory);

    m_isCommandQueued = false;   // Reset the command queued flag after processing
    m_prevGuiState = m_guiState; // Update the previous state to the current state

#ifdef GUI_DEBUG
    DEBUG_PRINTLN(guiStateHeaderToString(m_guiState.header.state));
#endif
}

void SmokeMateGUI::manageTempChartState(ulong currentTimeMSec)
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
}

void SmokeMateGUI::updateState(const ControllerStatus &controllerStatus, const Configuration &config)
{
    // Update the GUI state with the controller status
    m_guiState.isControllerRunning = controllerStatus.isRunning;
    m_guiState.status.smokerTempF = controllerStatus.temperatureSmoker;
    m_guiState.status.foodTempF = controllerStatus.temperatureFood;
    m_guiState.status.targetTempF = controllerStatus.temperatureTarget;
    m_guiState.status.fanPercent = map(controllerStatus.fanPWM, 0, 255, 0, 100);
    m_guiState.status.doorPercent = map(controllerStatus.doorPosition, config.doorClosePosition, config.doorOpenPosition, 0, 100);
    m_guiState.controllerStartTimeMSec = controllerStatus.controllerStartMSec;
    m_guiState.footer.isWiFiConnected = controllerStatus.isWiFiConnected;
    m_guiState.footer.RSSI = controllerStatus.RSSI;
    m_guiState.footer.bars = controllerStatus.bars;
    m_guiState.footer.ipAddress = controllerStatus.ipAddress;
    m_guiState.footer.isControllerRunning = controllerStatus.isRunning;
    m_guiState.footer.controllerRunTimeMSec = controllerStatus.controllerStartMSec > 0
                                                  ? (millis() - controllerStatus.controllerStartMSec)
                                                  : 0;
}

GuiState SmokeMateGUI::getState()
{
    // Return the current GUI state
    return m_guiState;
}

void SmokeMateGUI::commandMoveNext()
{
    GuiStateHeader &header = m_guiState.header;                   // Reference to the current header state
    GuiStateSettings &settings = m_guiState.settings;             // Reference to the settings state
    int stepIdx = m_guiState.tempProfile.editingIndex;            // Index for temperature profile steps
    TempProfileStep &step = m_config.temperatureProfile[stepIdx]; // Reference to the current temperature profile step

    if (m_isCommandQueued)
    {
        return; // Ignore if a command is already queued
    }

    m_isForcedGUIUpdate = true; // Force GUI update

    switch (header.state)
    {
    case GUI_STATE_HEADER_STATUS:
        header.state = GUI_STATE_HEADER_CHART; // Move to chart state
        m_isForcedGUIUpdate = true;            // Force GUI update
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

    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_SSID:
        // Scroll through available WiFi networks
        if (settings.wifiSSIDIndex < m_wifiNetworkSSIDs.size() - 1)
        {
            settings.wifiSSIDIndex++;
        }
        else
        {
            // Roll to top
            settings.wifiSSIDIndex = 0;
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_PASSWORD:
        // Increment the character index in the WiFi password
        if (settings.wifiPasswordCharIdx < strlen(WIFI_CHAR_SET) - 1)
        {
            settings.wifiPasswordCharIdx++;
        }
        else
        {
            settings.wifiPasswordCharIdx = 0;
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE:
        // Scroll through temperature profile settings
        if (m_guiState.tempProfile.cursor < m_config.temperatureProfileStepsCount - 1)
        {
            m_guiState.tempProfile.cursor++;
            if (m_guiState.tempProfile.cursor >= m_guiState.tempProfile.scroll + GUI_SETTINGS_BLOCK_COUNT)
                m_guiState.tempProfile.scroll++;
        }
        else
        {
            // Roll to top
            m_guiState.tempProfile.cursor = 0;
            m_guiState.tempProfile.scroll = 0;
        }

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP:

        // Move to next field
        if (m_guiState.tempProfileEdit.stepIndex < TEMPPROFILE_SETTINGS_COUNT - 1)
            m_guiState.tempProfileEdit.stepIndex++;
        else
            m_guiState.tempProfileEdit.stepIndex = 0;

        // Keep cursor in sync for highlight
        m_guiState.tempProfile.cursor = m_guiState.tempProfileEdit.stepIndex;
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP_EDIT:

        TEMPPROFILE_SETTINGS_LIST[m_guiState.tempProfileEdit.field].incFunc(
            m_config.temperatureProfile[m_guiState.tempProfile.editingIndex]);
        m_isCommandQueued = true;
        break;

    default:
        break;
    }

    m_isCommandQueued = true; // Set the command as queued
}

void SmokeMateGUI::commandMovePrevious()
{
    GuiStateHeader &header = m_guiState.header;                   // Reference to the current header state
    GuiStateSettings &settings = m_guiState.settings;             // Reference to the settings state
    int stepIdx = m_guiState.tempProfile.editingIndex;            // Index for temperature profile steps
    TempProfileStep &step = m_config.temperatureProfile[stepIdx]; // Reference to the current temperature profile step

    if (m_isCommandQueued)
    {
        return; // Ignore if a command is already queued
    }

    m_isForcedGUIUpdate = true; // Force GUI update

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

    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_SSID:
        // Scroll through available WiFi networks
        if (settings.wifiSSIDIndex > 0)
        {
            settings.wifiSSIDIndex--;
        }
        else
        {
            // Roll to bottom
            settings.wifiSSIDIndex = m_wifiNetworkSSIDs.size() - 1;
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_PASSWORD:
        // Decrement the character index in the WiFi password
        if (settings.wifiPasswordCharIdx > 0)
        {
            settings.wifiPasswordCharIdx--;
        }
        else
        {
            settings.wifiPasswordCharIdx = strlen(WIFI_CHAR_SET) - 1; // Wrap around to the last character
        }
        break;
    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE:
        // Scroll through temperature profile settings
        if (m_guiState.tempProfile.cursor > 0)
        {
            m_guiState.tempProfile.cursor--;
            if (m_guiState.tempProfile.cursor < m_guiState.tempProfile.scroll)
                m_guiState.tempProfile.scroll--;
        }
        else
        {
            // Roll to bottom
            m_guiState.tempProfile.cursor = m_config.temperatureProfileStepsCount - 1;
            m_guiState.tempProfile.scroll = m_config.temperatureProfileStepsCount > GUI_SETTINGS_BLOCK_COUNT
                                                ? m_config.temperatureProfileStepsCount - GUI_SETTINGS_BLOCK_COUNT
                                                : 0;
        }
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP:
        // Move to previous field
        if (m_guiState.tempProfileEdit.stepIndex > 0)
            m_guiState.tempProfileEdit.stepIndex--;
        else
            m_guiState.tempProfileEdit.stepIndex = TEMPPROFILE_SETTINGS_COUNT - 1;

        // Keep cursor in sync for highlight
        m_guiState.tempProfile.cursor = m_guiState.tempProfileEdit.stepIndex;
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP_EDIT:

        TEMPPROFILE_SETTINGS_LIST[m_guiState.tempProfileEdit.field].decFunc(
            m_config.temperatureProfile[m_guiState.tempProfile.editingIndex]);
        m_isCommandQueued = true;
        break;

    // Handle other states
    default:
        break;
    }

    m_isCommandQueued = true; // Set the command as queued
}

void SmokeMateGUI::commandSelect()
{
    GuiStateHeader &header = m_guiState.header;       // Reference to the current header state
    GuiStateSettings &settings = m_guiState.settings; // Reference to the settings state
    int len;                                          // Length of the WiFi password buffer

    if (m_isCommandQueued)
    {
        return; // Ignore if a command is already queued
    }

    m_isForcedGUIUpdate = true; // Force GUI update

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
        if (settings.cursor == SETTINGS_EXIT_INDEX)
        {
            header.state = GUI_STATE_HEADER_SETTINGS;
        }
        else if (settings.cursor == SETTINGS_REBOOT_INDEX)
        {
            // Reboot the esp32 hadware commnand
            ESP.restart();
        }
        else if (settings.cursor == SETTINGS_WIFI_SSID_INDEX)
        {
            header.state = GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_SSID; // Move to WiFi SSID edit state
            startWiFiScan();                                         // Start WiFi scan to populate available networks
            settings.wifiSSIDIndex = 0;                              // Reset the WiFi SSID index
        }
        else if (settings.cursor == SETTINGS_WIFI_PASSWORD_INDEX)
        {
            header.state = GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_PASSWORD; // Move to WiFi Password edit state
            // Reset the WiFi password character index
            settings.wifiPasswordCharPos = 0; // Reset the character position in the password
            settings.wifiPasswordCharIdx = 0; // Reset the character index in the password
            // Clear the WiFi password buffer
            memset(m_wifiPasswordBuffer, 0, sizeof(m_wifiPasswordBuffer)); // Clear the password buffer
        }
        else if (settings.cursor == SETTINGS_TEMP_PROFILE_START_INDEX)
        {
            header.state = GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE; // Move to temperature profiling state
            m_guiState.tempProfile.cursor = 0;                     // Reset the cursor for temperature profiling
            m_guiState.tempProfile.scroll = 0;                     // Reset the scroll for temperature profiling
            m_guiState.tempProfile.editingIndex = -1;              // Reset the editing index for temperature profiling
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
        m_isNVRAMSaveRequired = true;                  // Set the flag to indicate that NVRAM save is required
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_SSID:

        // Save selected SSID
        strncpy(m_config.wifiSSID, m_wifiNetworkSSIDs[m_guiState.settings.wifiSSIDIndex].c_str(), sizeof(m_config.wifiSSID));
        m_config.wifiSSID[sizeof(m_config.wifiSSID) - 1] = '\0'; // Ensure null termination
        // Exit WiFi SSID selection
        header.state = GUI_STATE_HEADER_SETTINGS_EDIT; // Move back to settings edit state
        m_isNVRAMSaveRequired = true;                  // Set the flag to indicate that NVRAM save is required
        // clear the wifi password for the next SSID
        m_config.wifiPassword[0] = '\0';
        break;

    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_PASSWORD:
        // Add selected character to password buffer
        len = strlen(m_wifiPasswordBuffer);
        if (len < sizeof(m_wifiPasswordBuffer) - 1)
        {
            m_wifiPasswordBuffer[len] = WIFI_CHAR_SET[m_guiState.settings.wifiPasswordCharIdx];
            m_wifiPasswordBuffer[len + 1] = '\0';
        }
        m_isForcedGUIUpdate = true;
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE:
        // Proceed to temperature profile step state
        header.state = GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP;          // Move to temperature profile step state
        m_guiState.tempProfile.editingIndex = m_guiState.tempProfile.cursor; // Set the editing index to the selected profile step
        m_guiState.tempProfileEdit.stepIndex = 0;                            // Start editing the first field of the selected profile step
        m_guiState.tempProfileEdit.editing = false;                          // Set editing to false initially
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP:
        header.state = GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP_EDIT;         // Move to temperature profile step edit state
        m_guiState.tempProfileEdit.editing = true;                               // Set editing to true
        m_guiState.tempProfileEdit.field = m_guiState.tempProfileEdit.stepIndex; // Set field to current stepIndex
        m_isCommandQueued = true;
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP_EDIT:
        header.state = GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP;
        m_isForcedGUIUpdate = true;                 // Force GUI update
        m_guiState.tempProfileEdit.editing = false; // Set editing to false
        m_isNVRAMSaveRequired = true;               // Set the flag to indicate that NVRAM save is required
        break;

    default:
        // If we reach here, it means we are in an unexpected state
        break;
    }

    m_isCommandQueued = true; // Set the command as queued
}

void SmokeMateGUI::commandConfirm()
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
        // Do nothing on command in the settings state
        break;
    case GUI_STATE_HEADER_SETTINGS_EDIT:
        // Leave settings edit mode
        header.state = GUI_STATE_HEADER_SETTINGS; // Move back to settings state
        settings.editingIndex = -1;               // Reset the editing index
        m_isForcedGUIUpdate = true;               // Force GUI update
        break;
    case GUI_STATE_HEADER_SETTINGS_EDIT_VALUE:
        // do nothing on command in the settings edit value state
        break;
    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_SSID:
        // Do nothing on command in the WiFi SSID edit state
        break;
    case GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_PASSWORD:
        // Save the WiFi password
        strncpy(m_config.wifiPassword, m_wifiPasswordBuffer, sizeof(m_config.wifiPassword));
        m_config.wifiPassword[sizeof(m_config.wifiPassword) - 1] = '\0';
        m_wifiPasswordBuffer[0] = '\0'; // Clear buffer for next time
        header.state = GUI_STATE_HEADER_SETTINGS_EDIT;
        m_isNVRAMSaveRequired = true;
        m_isForcedGUIUpdate = true;
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE:
        // Leave temperature profile settings
        header.state = GUI_STATE_HEADER_SETTINGS_EDIT; // Move back to settings editing state
        m_guiState.tempProfile.editingIndex = -1;      // Reset the editing index for temperature profile
        m_isForcedGUIUpdate = true;                    // Force GUI update
        break;

    case GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP:
        header.state = GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE; // Move back to temperature profile settings state
        m_guiState.tempProfileEdit.editing = false;
        m_isNVRAMSaveRequired = true;
        m_isForcedGUIUpdate = true;
        break;

    default:
        // If we reach here, it means we are in an unexpected state
        break;

        m_isCommandQueued = true; // Set the command as queued
    }
}

bool SmokeMateGUI::isNVRAMSaveRequired()
{
    // Check if NVRAM save is required
    if (m_isNVRAMSaveRequired)
    {
        m_isNVRAMSaveRequired = false; // Reset the flag after checking
        return true;                   // Indicate that NVRAM save is required
    }
    return false; // No NVRAM save required
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
    if (header.state == GUI_STATE_HEADER_SETTINGS_EDIT ||
        header.state == GUI_STATE_HEADER_SETTINGS_EDIT_VALUE ||
        header.state == GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_SSID ||
        header.state == GUI_STATE_HEADER_SETTINGS_EDIT_WIFI_PASSWORD ||
        header.state == GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE ||
        header.state == GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP ||
        header.state == GUI_STATE_HEADER_SETTINGS_TEMP_PROFILE_STEP_EDIT)
    {
        settingsColor = COLOR_HEADER_SELECTED; // Highlight the settings block when editing
    }
    else
    {
        settingsColor = (header.state == GUI_STATE_HEADER_SETTINGS) ? COLOR_HEADER_ACTIVE : COLOR_HEADER_PRIMARY;
    }

    // Draw the header blocks
    drawHeaderBlock(0, 0,
                    GUI_HEADER_BLOCK_WIDTH, GUI_HEADER_HEIGHT, 19, "STATUS",
                    header.state == GUI_STATE_HEADER_STATUS ? COLOR_HEADER_ACTIVE : COLOR_HEADER_PRIMARY);
    drawHeaderBlock(GUI_HEADER_BLOCK_WIDTH + 1, 0,
                    GUI_HEADER_BLOCK_WIDTH, GUI_HEADER_HEIGHT, 25, "CHART",
                    header.state == GUI_STATE_HEADER_CHART ? COLOR_HEADER_ACTIVE : COLOR_HEADER_PRIMARY);
    drawHeaderBlock(2 * GUI_HEADER_BLOCK_WIDTH + 2, 0,
                    GUI_HEADER_BLOCK_WIDTH, GUI_HEADER_HEIGHT, 7, "SETTINGS",
                    settingsColor);
}

void SmokeMateGUI::drawFooter(const GuiState &state, ulong elapsedMillis)
{
    char timeStr[9];
    m_tft.setTextSize(2);
    m_tft.setTextColor(COLOR_TEXT);

    String statusText = state.isControllerRunning ? "ON" : "OFF";

    if (state.isControllerRunning)
    {
        // Draw the footer background
        m_tft.fillRect(0, GUI_FOOTER_Y_OFFSET, SCREEN_WIDTH, GUI_FOOTER_HEIGHT, COLOR_HEADER_ACTIVE);

        unsigned long totalSeconds = elapsedMillis / 1000;
        unsigned long hours = totalSeconds / 3600;
        unsigned long minutes = (totalSeconds % 3600) / 60;
        unsigned long seconds = totalSeconds % 60;
        snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, seconds);
    }
    else
    {
        // Draw the footer background for idle state
        m_tft.fillRect(0, GUI_FOOTER_Y_OFFSET, SCREEN_WIDTH, GUI_FOOTER_HEIGHT, COLOR_HEADER_PRIMARY);
        snprintf(timeStr, sizeof(timeStr), "00:00:00");
    }

    if (m_guiState.footer.isWiFiConnected)
    {
        // Write the IP address in the footer
        m_tft.setCursor(GUI_FOOTER_IP_X_OFFSET, GUI_FOOTER_Y_OFFSET + 8);
        // Switch to a smaller text size for the IP address
        m_tft.setTextSize(1);
        m_tft.print(state.footer.ipAddress);
        // Reset text size for the rest of the footer
        m_tft.setTextSize(2);
        // Draw the WiFi icon with the current RSSI and bars
        drawWiFiIcon(GUI_FOOTER_WIFI_X_OFFSET, GUI_FOOTER_Y_OFFSET, m_guiState.footer.bars, m_guiState.footer.isWiFiConnected);
    }

    m_tft.setCursor(6, GUI_FOOTER_Y_OFFSET + 2);
    m_tft.print(statusText);
    m_tft.setCursor(GUI_FOOTER_CLOCK_X_OFFSET, GUI_FOOTER_Y_OFFSET + 2);
    m_tft.print(timeStr);

    // Additional footer information can be added here
}

void SmokeMateGUI::drawStausPanel(const GuiStateStatus &state)
{
    char foodTempStr[16];
    char smokerTempStr[16];
    char targetTempStr[16];
    char fanSpeedStr[16];
    char doorPosition[16];

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
                m_tft.fillRect(0, blockY + 1, SCREEN_WIDTH, GUI_SETTINGS_BLOCK_HEIGHT - 1, COLOR_HEADER_PRIMARY);
            }
        }

        m_tft.setCursor(GUI_SETTINGS_LABEL_OFFSET, blockY + 2);
        m_tft.setTextColor(COLOR_TEXT);

        if (i == SETTINGS_WIFI_SSID_INDEX)
        {
            // append the SSID label to the settings list item from the config
            String ssidDisplay = m_config.wifiSSID;
            if (ssidDisplay.length() > MAX_WIFI_SSID_LENGTH)
            {
                ssidDisplay = ssidDisplay.substring(0, MAX_WIFI_SSID_LENGTH);
            }
            m_tft.print(String(SETTINGS_LIST[i].label) + ": " + ssidDisplay);
        }
        else
        {
            m_tft.print(SETTINGS_LIST[i].label);
        }

        if (SETTINGS_LIST[i].getValue)
        {
            m_tft.setCursor(GUI_SETTINGS_VALUE_OFFSET, blockY + 2);
            m_tft.print(SETTINGS_LIST[i].getValue(m_config));
        }
    }
}

void SmokeMateGUI::drawWiFiSelectPanel()
{
    m_tft.fillRect(0, GUI_SETTINGS_PANEL_Y_OFFSET, SCREEN_WIDTH, GUI_SETTINGS_PANEL_HEIGHT, COLOR_BG);
    m_tft.setTextColor(COLOR_TEXT);
    m_tft.setTextSize(2);

    int count = m_wifiNetworkSSIDs.size();
    for (int i = 0; (i < min(count, MAX_WIFI_NETWORKS)); ++i)
    {
        int blockY = GUI_SETTINGS_PANEL_Y_OFFSET + i * GUI_SETTINGS_BLOCK_HEIGHT;
        if (i == m_guiState.settings.wifiSSIDIndex)
            m_tft.fillRect(0, blockY + 1, SCREEN_WIDTH, GUI_SETTINGS_BLOCK_HEIGHT - 1, COLOR_HEADER_PRIMARY);
        m_tft.setCursor(GUI_SETTINGS_LABEL_OFFSET, blockY + 2);
        // Print the SSID and (RSSI) in parentheses
        String ssidDisplay = m_wifiNetworkSSIDs[i];
        if (ssidDisplay.length() > MAX_WIFI_SSID_LENGTH)
        {
            ssidDisplay = ssidDisplay.substring(0, MAX_WIFI_SSID_LENGTH);
        }
        String wifiInfo = ssidDisplay + " (" + String(m_wifiNetworkRSSIs[i]) + ")";
        m_tft.print(wifiInfo);
    }
}

void SmokeMateGUI::drawWiFiPasswordPanel()
{
    m_tft.fillRect(0, GUI_SETTINGS_PANEL_Y_OFFSET, SCREEN_WIDTH, GUI_SETTINGS_PANEL_HEIGHT, COLOR_BG);
    m_tft.setTextColor(COLOR_TEXT);
    m_tft.setTextSize(2);
    m_tft.setCursor(GUI_SETTINGS_LABEL_OFFSET, GUI_SETTINGS_PANEL_Y_OFFSET + 10);
    m_tft.print("Enter WiFi Password:");
    m_tft.setCursor(GUI_SETTINGS_LABEL_OFFSET, GUI_SETTINGS_PANEL_Y_OFFSET + 40);
    m_tft.print(m_wifiPasswordBuffer);
    // Show current char selection
    m_tft.setCursor(GUI_SETTINGS_LABEL_OFFSET, GUI_SETTINGS_PANEL_Y_OFFSET + 80);
    m_tft.print(WIFI_CHAR_SET[m_guiState.settings.wifiPasswordCharIdx]);
}

void SmokeMateGUI::startWiFiScan()
{
    m_wifiNetworkSSIDs.clear();
    m_wifiNetworkRSSIs.clear();

    int n = WiFi.scanNetworks();
    for (int i = 0; i < min(n, MAX_WIFI_NETWORKS); ++i)
    {
        m_wifiNetworkSSIDs.push_back(WiFi.SSID(i));
        m_wifiNetworkRSSIs.push_back(WiFi.RSSI(i));
    }
    m_guiState.settings.wifiSSIDIndex = 0;
    m_isForcedGUIUpdate = true;
}

void SmokeMateGUI::drawWiFiIcon(int x, int y, int bars, bool connected)
{
    // Icon fits in 16x16 pixels, y is the top of the icon area (should be GUI_FOOTER_Y_OFFSET + 2)
    uint16_t color = connected ? COLOR_TEXT : ST77XX_RED;
    int barWidth = 4;
    int barSpacing = 2;
    int baseHeight = 4; // Smallest bar
    int maxHeight = 13; // Tallest bar (leaves room for dot at bottom)

    // Draw up to 4 bars, left to right
    for (int i = 0; i < 4; ++i)
    {
        int barHeight = baseHeight + i * 3; // 4, 7, 10, 13
        int barX = x + i * (barWidth + barSpacing);
        int barY = y + (16 - barHeight); // Align bottom of icon to y+16
        if (bars > i)
            m_tft.fillRect(barX, barY, barWidth, barHeight, color);
        else
            m_tft.drawRect(barX, barY, barWidth, barHeight, color);
    }
}

void SmokeMateGUI::drawTempProfilePanel(const GuiStateTempProfile &tempProfile)
{
    // Clear the temperature profile panel area
    m_tft.fillRect(0, GUI_SETTINGS_PANEL_Y_OFFSET, SCREEN_WIDTH, GUI_SETTINGS_PANEL_HEIGHT, COLOR_BG);

    m_tft.setTextColor(COLOR_TEXT);
    m_tft.setTextSize(2);

    // Iterate through the temperature profile steps and draw them
    for (int i = 0; i < m_config.temperatureProfileStepsCount; ++i)
    {
        TempProfileStep &step = m_config.temperatureProfile[i];
        bool isSelected = (i == tempProfile.cursor);
        drawTempProfileStepLine(i, step, isSelected);
    }
}

void SmokeMateGUI::drawTempProfileStepLine(int n, TempProfileStep &step, bool selected)
{
    // Calculate Y position for this line
    int y = GUI_SETTINGS_PANEL_Y_OFFSET + n * GUI_SETTINGS_BLOCK_HEIGHT;

    m_tft.setTextColor(COLOR_TEXT); // Set text color
    m_tft.setTextSize(2);

    // Draw the background for the block with the primary color if not selected and active color if selected
    m_tft.fillRect(0, y, SCREEN_WIDTH, GUI_SETTINGS_BLOCK_HEIGHT, selected ? COLOR_HEADER_PRIMARY : COLOR_BG);

    // Compose the label with the strings depending on the content of the step:
    // Step index (1 based)
    // Step type: Dwell or Ramp (TempProfileType)
    // Step duration in minutes
    // Step starting temperature in F
    // Step end temperature in F (if applicable, only for Ramp steps)
    String text = String(n + 1) + " " + (step.type == TEMP_PROFILE_TYPE_DWELL ? "Dwell" : "Ramp") +
                  " " + String(step.timeMSec / (60 * 1000)) + " min " + step.temperatureStartF;
    if (step.type == TEMP_PROFILE_TYPE_RAMP)
    {
        text += "-" + String(step.temperatureEndF);
    }
    text += " F";

    // Draw the label
    m_tft.setCursor(GUI_SETTINGS_LABEL_OFFSET, y + 4);
    m_tft.print(text);
}

void SmokeMateGUI::drawTempProfileStepPanel(const GuiStateTempProfile &tempProfile,
                                            GuiStateTempProfileEdit &tempProfileEdit)
{
    int stepIdx = tempProfile.editingIndex;
    if (stepIdx < 0 || stepIdx >= m_config.temperatureProfileStepsCount)
        return;
    TempProfileStep &step = m_config.temperatureProfile[stepIdx];

    // Clear the panel area
    m_tft.fillRect(0, GUI_SETTINGS_PANEL_Y_OFFSET, SCREEN_WIDTH, GUI_SETTINGS_PANEL_HEIGHT, COLOR_BG);
    m_tft.setTextColor(COLOR_TEXT);
    m_tft.setTextSize(2);

    // Draw step header
    int y = GUI_SETTINGS_PANEL_Y_OFFSET;
    m_tft.setCursor(GUI_SETTINGS_LABEL_OFFSET, y + 4);
    m_tft.print("Step ");
    m_tft.print(stepIdx + 1);

    // Draw each editable field using the settings list
    for (int i = 0; i < TEMPPROFILE_SETTINGS_COUNT; ++i)
    {
        y = GUI_SETTINGS_PANEL_Y_OFFSET + (i + 1) * GUI_SETTINGS_BLOCK_HEIGHT;

        // If this is the step being edited, highlight it with the primary color
        bool selected = i == tempProfileEdit.stepIndex;
        bool editing = tempProfileEdit.editing;
        // Set the color
        uint16_t color = selected ? (editing ? COLOR_HEADER_SELECTED : COLOR_HEADER_PRIMARY) : COLOR_BG;

        m_tft.fillRect(0, y, SCREEN_WIDTH, GUI_SETTINGS_BLOCK_HEIGHT, color);

        m_tft.setCursor(GUI_SETTINGS_LABEL_OFFSET, y + 4);
        m_tft.print(TEMPPROFILE_SETTINGS_LIST[i].label);
        m_tft.setCursor(GUI_SETTINGS_VALUE_OFFSET, y + 4);
        m_tft.print(TEMPPROFILE_SETTINGS_LIST[i].getValue(step));
    }
}