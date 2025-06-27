#include "guitabpanel.h"
#include <memory>

const char *GUI_TAB_NAMES[GUI_TABPANEL_TAB_COUNT] = {
    "STATUS",
    "CHART",
    "CONFIG",
};

// Equality operator
bool operator==(const GuiTabState &lhs, const GuiTabState &rhs)
{
    return lhs.cursor == rhs.cursor &&
           lhs.isActive == rhs.isActive;
}

// Inequality operator
bool operator!=(const GuiTabState &lhs, const GuiTabState &rhs)
{
    return !(lhs == rhs);
}

GuiTabPanel::GuiTabPanel(Adafruit_ST7789 &displayRef, int x0, int y0, int width, int height)
    : m_tft(displayRef), m_x0(x0), m_y0(y0), m_panelWidth(width), m_panelHeight(height)
{
    m_currentTimeMsec = 0; // Initialize current time in milliseconds
    for (int i = 0; i < GUI_TABPANEL_TAB_COUNT; ++i)
    {
        // Calculate the x0, y0, width, and height for each tab
        int tabWidth = width / GUI_TABPANEL_TAB_COUNT; // Width of each tab
        int tabX0 = x0 + i * tabWidth;                 // X coordinate for the tab
        int tabY0 = y0;                                // Y coordinate is fixed at the top of the panel

        // Create a new GuiTab instance and store it in the m_tabs array
        m_tabs[i] = std::unique_ptr<GuiTab>(new GuiTab(displayRef, tabX0, tabY0, tabWidth, height, GUI_TAB_NAMES[i]));
    }
}

void GuiTabPanel::update(ulong currentTimeMsec)
{
    m_currentTimeMsec = currentTimeMsec; // Update the current time

    // if (m_prevState == m_state)
    // {
    //     return; // No change in state, nothing to update
    // }

    draw(); // Redraw the tab panel with the new state

    m_prevState = m_state; // Store the previous state for comparison
}

void GuiTabPanel::draw()
{
    // Iterate through each tab and draw it
    for (int i = 0; i < GUI_TABPANEL_TAB_COUNT; ++i)
    {
        m_tabs[i]->update(m_currentTimeMsec);
    }
}
