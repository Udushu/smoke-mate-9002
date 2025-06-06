#ifndef GUI_H
#define GUI_H
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "types.h"

#include <deque>

// CHART CONSTANTS ================================================================================
#define GUI_MAX_HISTORY_ENTRIES 1800                      // Maximum number of temperature history entries to keep
#define GUI_CHART_UPDATE_INTERVAL_MSEC 2 * 1000           // Interval to update the history in milliseconds
#define GUI_CHART_MIN2HOUR_SWITCH_MSEC 2 * 60 * 60 * 1000 // Switch to 2-hour chart mode after this time

// COLOR DEFINITIONS ==============================================================================
// Convert hex color to RGB565 format
// The hex color format is 0xRRGGBB, and RGB565 format is 0xRRRRRGGGGGGBBBBB
#define HEX_RGB565(hex) ((((hex) >> 16 & 0xF8) << 8) | (((hex) >> 8 & 0xFC) << 3) | ((hex) & 0xFF >> 3))

#define COLOR_BG HEX_RGB565(0x000000)              // Dark center background
#define COLOR_HEADER_PASSIVE HEX_RGB565(0xFF6600)  // Orange top/bottom
#define COLOR_HEADER_RUNNING HEX_RGB565(0x008610)  // Orange top/bottom
#define COLOR_HEADER_ACTIVE HEX_RGB565(0x0083FE)   // Blue for active state
#define COLOR_HEADER_SELECTED HEX_RGB565(0x003D77) // Dark Blue for active selected state
#define COLOR_TEXT HEX_RGB565(0xFFFFFF)
#define COLOR_CHART_SMOKER HEX_RGB565(0xFF6600)    // Red for smoker temperature
#define COLOR_CHART_FOOD HEX_RGB565(0x0083FE)      // Green for food temperature
#define COLOR_CHART_TARGET HEX_RGB565(0x00FFD0)    // Blue for target temperature
#define COLOR_CHART_GRIDLINES HEX_RGB565(0x202020) // Black for chart background

// GUI CONSTANTS ==================================================================================

#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320

#define GUI_HEADER_HEIGHT 40
#define GUI_HEADER_BLOCK_WIDTH 106 // Width of each header block

#define GUI_FOOTER_OFFSET 220 // Footer starts at this Y position
#define GUI_FOOTER_HEIGHT 20

#define GUI_STATUS_PANEL_Y_OFFSET GUI_HEADER_HEIGHT                     // Y offset for the status canvas
#define GUI_STATUS_PANEL_HEIGHT (GUI_FOOTER_OFFSET - GUI_HEADER_HEIGHT) // Height of the status canvas
#define GUI_STATUS_PANEL_BLOCK_WIDTH (SCREEN_WIDTH / 2)
#define GUI_STATUS_PANEL_BLOCK_COUNT 5
#define GUI_STATUS_PANEL_BLOCK_HEIGHT (GUI_STATUS_PANEL_HEIGHT / GUI_STATUS_PANEL_BLOCK_COUNT)

#define GUI_CHART_PANEL_Y_OFFSET GUI_HEADER_HEIGHT
#define GUI_CHART_PANEL_HEIGHT (GUI_FOOTER_OFFSET - GUI_HEADER_HEIGHT) // Height of the chart canvas

#define GUI_SETTINGS_PANEL_Y_OFFSET GUI_HEADER_HEIGHT                     // Y offset for the settings canvas
#define GUI_SETTINGS_PANEL_HEIGHT (GUI_FOOTER_OFFSET - GUI_HEADER_HEIGHT) // Height of the settings canvas
#define GUI_SETTINGS_BLOCK_COUNT 8
#define GUI_SETTINGS_BLOCK_HEIGHT (GUI_SETTINGS_PANEL_HEIGHT / GUI_SETTINGS_BLOCK_COUNT)
#define GUI_SETTINGS_VALUE_OFFSET 220 // Offset for the value in settings panel
#define GUI_SETTINGS_LABEL_OFFSET 10  // Offset for the label in settings panel

// --- Temperature settings edit constants
#define GUI_SETTINGS_TEMP_MIN 100
#define GUI_SETTINGS_TEMP_MAX 500
#define GUI_SETTINGS_TEMP_STEP 5
#define GUI_SETTINGS_TEMP_BB_BAND_STEP 1
#define GUI_SETTINGS_TEMP_BB_BAND_MIN 1
#define GUI_SETTINGS_TEMP_BB_BAND_MAX 50

// --- Time interval (step: 1 second, min: 1s, max: 60s) ---
#define GUI_SETTINGS_INTERVAL_MIN 1000
#define GUI_SETTINGS_INTERVAL_MAX 60000
#define GUI_SETTINGS_INTERVAL_STEP 1000

// --- PWM settings edit constants
#define GUI_SETTINGS_PWM_MIN 0
#define GUI_SETTINGS_PWM_MAX 255
#define GUI_SETTINGS_PWM_STEP 5

// PID settings edit constants
#define GUI_SETTINGS_PID_K_MIN 0.0f
#define GUI_SETTINGS_PID_K_MAX 10.0f
#define GUI_SETTINGS_PID_K_STEP 0.1f
#define GUI_SETTINGS_PID_K_DECIMAL_PLACES 2 // Decimal places for PID settings

enum GUI_STATE_ACTIVE_HEADER
{
    GUI_STATE_HEADER_STATUS,
    GUI_STATE_HEADER_CHART,
    GUI_STATE_HEADER_SETTINGS,
    GUI_STATE_HEADER_SETTINGS_EDIT,
    GUI_STATE_HEADER_SETTINGS_EDIT_VALUE,
};

struct TemperatureHistoryEntry
{
    ulong timestampMSec;
    int smokerTempF;
    int foodTempF;
    int targetTempF;
};

struct GuiStateHeader
{
    enum GUI_STATE_ACTIVE_HEADER state;
};

struct GuiStateStatus
{
    int smokerTempF;
    int foodTempF;
    int targetTempF;
    uint8_t fanPercent;
    uint8_t doorPercent;
};

struct GuiStateSettings
{
    int cursor;       // Current cursor position in settings
    int scroll;       // Current scroll position in settings
    int editingIndex; // Index of the setting being edited, -1 if not editing
};

struct GuiState
{
    GuiStateHeader header; // Current active header state
    GuiStateStatus status;
    GuiStateSettings settings; // Settings state

    bool isControllerRunning;
    ulong controllerStartTimeMSec;

    std::deque<TemperatureHistoryEntry> history;
};

// --- Settings metadata and helpers ---
typedef void (*SettingEditFunc)(Configuration &);
struct SettingItem
{
    const char *label;                         // Label for the setting
    String (*getValue)(const Configuration &); // Function to get the setting value as a string
    SettingEditFunc incFunc;                   // Function to increment the setting value
    SettingEditFunc decFunc;                   // Function to decrement the setting value
};

class SmokeMateGUI
{
public:
    SmokeMateGUI(Adafruit_ST7789 &displayRef, Configuration &config);
    void begin();
    void service(ulong currentTimeMSec);

    void updateState(const ControllerStatus &controllerStatus, const Configuration &config);
    GuiState getState();
    void commandMoveNext();
    void commandMovePrevious();
    void commandSelect();

    bool isNVRAMSaveRequired();

private:
    Adafruit_ST7789 &m_tft;  // Reference to the display object
    GuiState m_guiState;     // Current GUI state
    GuiState m_prevGuiState; // Previous GUI state for comparison
    Configuration &m_config; // Reference to the configuration object

    bool m_isCommandQueued = false;                                   // Flag to indicate if a command is queued
    bool m_isControllerRunning = false;                               // Flag to indicate if the controller is running
    ulong m_lastChartUpdateTimeMSec = 0;                              // Last time the chart was updated
    bool m_isChartUpdateNeeded = false;                               // Flag to indicate if chart update is needed
    ulong m_chartSampleIntervalMSec = GUI_CHART_UPDATE_INTERVAL_MSEC; // Current sampling interval
    bool m_isNVRAMSaveRequired = false;                               // Flag to indicate if NVRAM save is required
    bool m_isForcedGUIUpdate = false;                                 // Flag to force GUI update
    bool m_isFirstHeaderRender = true;                                // Flag to indicate if this is the first header render

    void drawHeader(const GuiStateHeader &state);
    void drawFooter(const GuiState &state, ulong controllerRunTimeMSec);
    void drawStausPanel(const GuiStateStatus &state);
    void drawChartPanel(const std::deque<TemperatureHistoryEntry> &history);
    void drawSettingsPanel(const GuiState &state);

    void drawHeaderBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t textOffset,
                         const char *text, uint16_t color);

    void drawStatusBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                         const char *text);
    void drawStatusLine(uint16_t n, const char *label, const char *value);
};

#endif // GUI_H