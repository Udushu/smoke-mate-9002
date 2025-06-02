#include "gui.h"
#include <math.h>

// ========================================== PUBLIC METHODS ==========================================

SmokeMateGUI::SmokeMateGUI(Adafruit_ST7789 &displayRef) : tft(displayRef)
{
    // Initialize the GUI state
    m_guiState.header.state = GUI_STATE_HEADER_STATUS; // Start with the status header
    m_guiState.header.isSelected = false;              // Start with no header selected
    m_guiState.status.targetTempF = 0;
    m_guiState.status.smokerTempF = 0;
    m_guiState.status.foodTempF = 0;
    m_guiState.isControllerRunning = false; // Start with controller not running
    m_guiState.status.fanPercent = 0;       // Start with fan off
    m_guiState.status.doorPercent = 0;      // Start with door closed
    m_guiState.controllerStartTimeMSec = 0; // Start with zero controller start time
    // Initialize the history
    m_guiState.history.clear();
}

void SmokeMateGUI::begin()
{
    tft.setRotation(1); // Landscape
    tft.fillScreen(COLOR_BG);
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
            drawChart(m_guiState.history);
        }
        break;

    case GUI_STATE_HEADER_SETTINGS:
        m_isChartUpdateNeeded = true;
        // do nothing for now, settings panel is not implemented
        // draw black rectangle to clear the area
        tft.fillRect(0, GUI_HEADER_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - GUI_HEADER_HEIGHT - GUI_FOOTER_HEIGHT, COLOR_BG);
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
    GuiStateHeader &header = m_guiState.header; // Reference to the current header state

    if (m_isCommandQueued)
    {
        return; // Ignore if a command is already queued
    }

    if (!header.isSelected)
    {

        if (header.state == GUI_STATE_HEADER_SETTINGS)
        {
            header.state = GUI_STATE_HEADER_STATUS;
        }
        else if (header.state == GUI_STATE_HEADER_CHART)
        {
            header.state = GUI_STATE_HEADER_SETTINGS;
        }
        else
        {
            header.state = static_cast<GUI_STATE_ACTIVE_HEADER>(header.state + 1);
        }
    }

    m_isCommandQueued = true; // Set the command as queued
}

void SmokeMateGUI::commandMovePrevious()
{
    GuiStateHeader &header = m_guiState.header; // Reference to the current header state
    if (m_isCommandQueued)
    {
        return; // Ignore if a command is already queued
    }

    if (!header.isSelected)
    {
        if (header.state == GUI_STATE_HEADER_STATUS)
        {
            header.state = GUI_STATE_HEADER_SETTINGS;
        }
        else if (header.state == GUI_STATE_HEADER_CHART)
        {
            header.state = GUI_STATE_HEADER_STATUS;
        }
        else
        {
            header.state = static_cast<GUI_STATE_ACTIVE_HEADER>(header.state - 1);
        }
    }

    m_isCommandQueued = true; // Set the command as queued
}

void SmokeMateGUI::commandSelect()
{
    GuiStateHeader &header = m_guiState.header; // Reference to the current header state
    if (m_isCommandQueued)
    {
        return; // Ignore if a command is already queued
    }

    m_isCommandQueued = true; // Set the command as queued
}

// ========================================== PRIVATE METHODS ==========================================

void SmokeMateGUI::drawHeaderBlock(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                                   uint16_t textOffset,
                                   const char *text,
                                   bool isActive, bool isSelected)
{
    uint16_t bgColor = isActive ? COLOR_HEADER_ACTIVE : COLOR_HEADER_PASSIVE;
    if (isActive && isSelected)
    {
        bgColor = COLOR_HEADER_SELECTED;
    }

    tft.fillRect(x, y, width, height, bgColor);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(x + textOffset, y + 13);
    tft.print(text);
}

void SmokeMateGUI::drawHeader(const GuiStateHeader &header)
{
    // Draw the header blocks
    drawHeaderBlock(0, 0,
                    GUI_HEADER_BLOCK_WIDTH, GUI_HEADER_HEIGHT, 19, "STATUS",
                    header.state == GUI_STATE_HEADER_STATUS,
                    header.isSelected && header.state == GUI_STATE_HEADER_STATUS);
    drawHeaderBlock(GUI_HEADER_BLOCK_WIDTH + 1, 0,
                    GUI_HEADER_BLOCK_WIDTH, GUI_HEADER_HEIGHT, 25, "CHART",
                    header.state == GUI_STATE_HEADER_CHART,
                    header.isSelected && header.state == GUI_STATE_HEADER_CHART);
    drawHeaderBlock(2 * GUI_HEADER_BLOCK_WIDTH + 2, 0,
                    GUI_HEADER_BLOCK_WIDTH, GUI_HEADER_HEIGHT, 7, "SETTINGS",
                    header.state == GUI_STATE_HEADER_SETTINGS,
                    header.isSelected && header.state == GUI_STATE_HEADER_SETTINGS);
}

void SmokeMateGUI::drawFooter(const GuiState &state, ulong elapsedMillis)
{
    char timeStr[9];
    if (state.isControllerRunning)
    {

        tft.fillRect(0, GUI_FOOTER_OFFSET, SCREEN_WIDTH, GUI_FOOTER_HEIGHT, COLOR_HEADER_ACTIVE);
        tft.setTextSize(2);
        tft.setTextColor(COLOR_TEXT);

        tft.setCursor(6, GUI_FOOTER_OFFSET + 2);
        tft.print("Running");

        // Display the elapsed time in HH:MM:SS format
        tft.setCursor(SCREEN_WIDTH / 2 + 40, GUI_FOOTER_OFFSET + 2);

        unsigned long totalSeconds = elapsedMillis / 1000;
        unsigned long hours = totalSeconds / 3600;
        unsigned long minutes = (totalSeconds % 3600) / 60;
        unsigned long seconds = totalSeconds % 60;
        snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, seconds);
        tft.print(timeStr);
    }
    else
    {

        tft.fillRect(0, GUI_FOOTER_OFFSET, SCREEN_WIDTH, GUI_FOOTER_HEIGHT, COLOR_HEADER_PASSIVE);
        tft.setTextSize(2);
        tft.setTextColor(COLOR_TEXT);

        tft.setCursor(6, GUI_FOOTER_OFFSET + 2);
        tft.print("Idle");

        // Display the elapsed time in HH:MM:SS format
        tft.setCursor(SCREEN_WIDTH / 2 + 40, GUI_FOOTER_OFFSET + 2);

        snprintf(timeStr, sizeof(timeStr), "00:00:00");
        tft.print(timeStr);
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
    tft.fillRect(x, y, width, height, COLOR_BG);
    tft.setTextSize(3);
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(x + 6, y + 6);
    tft.print(text);
}

void SmokeMateGUI::drawChart(const std::deque<TemperatureHistoryEntry> &history)
{
    const int chartX = 20;
    const int chartY = GUI_CHART_PANEL_Y_OFFSET + 10;
    const int width = SCREEN_WIDTH - chartX - 10;
    const int height = GUI_CHART_PANEL_HEIGHT - 30;

    // Clear the chart area as defined by the chart panel
    tft.fillRect(0, GUI_CHART_PANEL_Y_OFFSET, SCREEN_WIDTH, GUI_CHART_PANEL_HEIGHT, COLOR_BG);

    // Draw chart background and border
    tft.drawRect(chartX, chartY, width, height, ST77XX_WHITE);

    // Title
    tft.setCursor(chartX, chartY - 12);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);

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
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    int yTicks = 5;
    for (int i = 0; i <= yTicks; ++i)
    {
        int temp = minT + (maxT - minT) * i / yTicks;
        int y = chartY + height - ((height - 1) * i) / yTicks;
        tft.drawFastHLine(chartX, y, 4, ST77XX_WHITE); // Tick
        tft.setCursor(chartX - 20, y - 4);
        tft.printf("%3d", temp);
    }
    // Y-axis label
    tft.setCursor(chartX - 32, chartY + height / 2 - 8);
    tft.print("F");

    // --- Draw X axis (Time in minutes or hours) ---
    int xTicks = 5;
    ulong durationMSec = maxTime - minTime;
    bool showHours = (maxTime > GUI_CHART_MIN2HOUR_SWITCH_MSEC);

    for (int i = 0; i <= xTicks; ++i)
    {
        ulong t = minTime + (durationMSec * i) / xTicks;
        int x = chartX + ((width - 1) * i) / xTicks;
        tft.drawFastVLine(x, chartY + height - 4, 4, ST77XX_WHITE); // Tick

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
        tft.setCursor(x + cursorOffset, chartY + height + 2);
        if (showHours)
        {
            float hours = t / 3600000.0f;
            tft.printf("%.2f", hours);
        }
        else
        {
            float minutes = t / 60000.0f;
            tft.printf("%.1f", minutes);
        }
    }
    // X-axis label
    tft.setCursor(chartX + width - 24, chartY + height - 20);
    if (showHours)
    {
        tft.print("hrs");
    }
    else
    {
        tft.print("min");
    }

    // Y gridlines
    for (int i = 1; i < yTicks; ++i)
    {
        int y = chartY + height - ((height - 1) * i) / yTicks;
        tft.drawFastHLine(chartX + 1, y, width - 2, COLOR_CHART_GRIDLINES);
    }

    // X gridlines
    for (int i = 1; i < xTicks; ++i)
    {
        int x = chartX + ((width - 1) * i) / xTicks;
        tft.drawFastVLine(x, chartY + 1, height - 2, COLOR_CHART_GRIDLINES);
    }

    // --- Draw Legend (top right corner) ---
    int legendX = chartX + width - 50;
    int legendY = chartY + 8;
    tft.setTextSize(1);

    tft.setCursor(legendX, legendY);
    tft.setTextColor(COLOR_CHART_SMOKER);
    tft.print("Smoker");

    tft.setCursor(legendX, legendY + 12);
    tft.setTextColor(COLOR_CHART_FOOD);
    tft.print("Food");

    tft.setCursor(legendX, legendY + 24);
    tft.setTextColor(COLOR_CHART_TARGET);
    tft.print("Target");

    tft.setTextColor(ST77XX_WHITE); // Reset to default for other text

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

        tft.drawLine(x0, y0s, x1, y1s, COLOR_CHART_SMOKER); // Smoker temp
        tft.fillCircle(x, ySmoker, 2, COLOR_CHART_SMOKER);
        // tft.drawLine(x0, y0s - 1, x1, y1s - 1, COLOR_CHART_SMOKER); // Smoker temp
        // tft.drawLine(x0, y0s + 1, x1, y1s + 1, COLOR_CHART_SMOKER); // Smoker temp
        tft.drawLine(x0, y0f, x1, y1f, COLOR_CHART_FOOD); // Food temp
        tft.fillCircle(x, yFood, 2, COLOR_CHART_FOOD);
        // tft.drawLine(x0, y0f - 1, x1, y1f - 1, COLOR_CHART_FOOD);   // Food temp
        // tft.drawLine(x0, y0f + 1, x1, y1f + 1, COLOR_CHART_FOOD);   // Food temp
        tft.drawLine(x0, y0t, x1, y1t, COLOR_CHART_TARGET); // Target temp
    }
}
