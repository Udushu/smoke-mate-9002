#include "guitab.h"

GuiTab::GuiTab(Adafruit_ST7789 &displayRef, int x0, int y0, int width, int height, String name)
    : m_tft(displayRef), m_x0(x0), m_y0(y0), m_width(width), m_height(height), m_name(name)
{
    m_state = GUI_TAB_STATE_INACTIVE; // Initialize to inactive state
    m_prevState = GUI_TAB_STATE_NULL; // Previous state starts as inactive
}

void GuiTab::update(ulong currentTimeMsec)
{
    if (m_prevState == m_state)
    {
        return; // No change in state, nothing to update
    }

    m_currentTimeMsec = currentTimeMsec; // Update the current time in milliseconds

    draw(); // Redraw the tab with the new state

    m_prevState = m_state; // Store the previous state for comparison
}

void GuiTab::setActive()
{
    m_state = GUI_TAB_STATE_ACTIVE; // Set the tab to active state
}

void GuiTab::setSelected()
{
    m_state = GUI_TAB_STATE_SELECTED; // Set the tab to selected state
}

void GuiTab::setInactive()
{
    m_state = GUI_TAB_STATE_INACTIVE; // Set the tab to inactive state
}

GUI_TAB_STATE GuiTab::getState()
{
    return m_state; // Return the current state of the tab
}

void GuiTab::draw()
{
    // Determine the color based on the state of the tab
    uint16_t fillColor;

    switch (m_state)
    {
    case GUI_TAB_STATE_SELECTED:
        fillColor = COLOR_HEADER_SELECTED; // Selected tab color
        break;
    case GUI_TAB_STATE_ACTIVE:
        fillColor = COLOR_HEADER_ACTIVE; // Active tab color
        break;
    default:
        fillColor = COLOR_HEADER_PRIMARY; // Inactive tab color
        break;
    }

    int radius = 5;      // Radius for rounded corners
    int borderWidth = 2; // Width of the border

    // Make the tab one pixel smaller in all directions
    int delta = 10;
    int x = m_x0 + delta;
    int y = m_y0 + delta;
    int w = m_width - delta * 2;
    int h = m_height - delta * 2;

    // Draw filled rounded rectangle for the tab background, inside the border
    int fillInset = borderWidth;
    int fillRadius = radius - borderWidth + 1;
    if (fillRadius < 0)
        fillRadius = 0;
    m_tft.fillRoundRect(m_x0 + fillInset, m_y0 + fillInset, m_width - 2 * fillInset, m_height - 2 * fillInset, fillRadius, fillColor);

    // Draw border for the tab
    for (int i = 0; i < borderWidth; i++)
    {
        m_tft.drawRoundRect(m_x0 + i, m_y0 + i, m_width - 2 * i, m_height - 2 * i, radius - i, COLOR_BORDER);
    }
    // m_tft.drawRoundRect(m_x0, m_y0, m_width, m_height, radius, COLOR_BORDER);
    // m_tft.drawRoundRect(m_x0 + 1, m_y0 + 1, m_width - 2, m_height - 2, radius - 1, COLOR_BORDER);

    // Draw the tab name
    m_tft.setTextColor(COLOR_TEXT, fillColor);
    m_tft.setTextSize(GUI_TAB_TEXT_SIZE);
    m_tft.setCursor(m_x0 + GUI_TAB_TEXT_OFFSET_X, m_y0 + GUI_TAB_TEXT_OFFSET_Y);
    m_tft.print(m_name);
}