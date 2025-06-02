#ifndef GUI_H
#define GUI_H
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
// #include <Fonts/FreeSans9pt7b.h>
// #include <Fonts/FreeMono12pt7b.h>
// #include <Fonts/FreeMono9pt7b.h>

#include <deque>

#define GUI_MAX_HISTORY_ENTRIES 50              // Maximum number of temperature history entries to keep
#define GUI_CHART_UPDATE_INTERVAL_MSEC 1 * 1000 // Interval to update the history in milliseconds

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

enum GUI_STATE_ACTIVE_HEADER
{
    GUI_STATE_HEADER_STATUS,
    GUI_STATE_HEADER_CHART,
    GUI_STATE_HEADER_SETTINGS,
};

struct TemperatureHistoryEntry
{
    ulong timestampMSec;
    int smokerTempF;
    int foodTempF;
    int targetTempF;
};

struct GuiStateStatus
{
    int smokerTempF;
    int foodTempF;
    int targetTempF;
    uint8_t fanPercent;
    uint8_t doorPercent;
};

struct GuiState
{
    enum GUI_STATE_ACTIVE_HEADER headerState;
    bool isSelected;
    GuiStateStatus status;

    bool isControllerRunning;
    ulong controllerStartTimeMSec;

    std::deque<TemperatureHistoryEntry> history;
};

class SmokeMateGUI
{
public:
    SmokeMateGUI(Adafruit_ST7789 &displayRef);
    void begin();
    void service(GuiState &state, ulong currentTimeMSec);

    void commandMoveNext(GuiState &state);
    void commandMovePrevious(GuiState &state);
    void commandSelect(GuiState &state);

private:
    void drawHeader(const GuiState &state);
    void drawFooter(const GuiState &state, ulong controllerRunTimeMSec);
    void drawStausPanel(const GuiStateStatus &state);
    void drawChart(const std::deque<TemperatureHistoryEntry> &history);

    void drawHeaderBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t textOffset,
                         const char *text, bool isActive, bool isSelected);

    void drawStatusBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                         const char *text);
    void drawStatusLine(uint16_t n, const char *label, const char *value);

    Adafruit_ST7789 &tft;
    bool m_isCommandQueued = false;                                   // Flag to indicate if a command is queued
    bool m_isControllerRunning = false;                               // Flag to indicate if the controller is running
    ulong m_lastChartUpdateTimeMSec = 0;                              // Last time the chart was updated
    bool m_isChartUpdateNeeded = false;                               // Flag to indicate if chart update is needed
    ulong m_chartSampleIntervalMSec = GUI_CHART_UPDATE_INTERVAL_MSEC; // Current sampling interval
};

#endif // GUI_H