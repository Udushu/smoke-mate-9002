#ifndef GUITABPANEL_H
#define GUITABPANEL_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "types.h"
#include "guitab.h"
#include "guicolors.h"
#include <memory>

struct GuiTabState
{
    uint cursor;
    bool isActive; // Indicates if the tab is active or not
};

// array of strings for tab names
#define GUI_TABPANEL_TAB_COUNT 3
extern const char *GUI_TAB_NAMES[GUI_TABPANEL_TAB_COUNT];

class GuiTabPanel
{
public:
    GuiTabPanel(Adafruit_ST7789 &displayRef, int x0, int y0, int width, int height);
    void update(ulong currentTimeMsec);
    void draw();

private:
    // Array of tabs
    std::unique_ptr<GuiTab> m_tabs[GUI_TABPANEL_TAB_COUNT];
    Adafruit_ST7789 &m_tft;  // Reference to the display object
    int m_x0;                // X coordinate of the top-left corner
    int m_y0;                // Y coordinate of the top-left corner
    int m_panelWidth;        // Width of the tab panel
    int m_panelHeight;       // Height of the tab panel
    GuiTabState m_state;     // Current state of the tab panel
    GuiTabState m_prevState; // Previous state for comparison
    ulong m_currentTimeMsec; // Current time in milliseconds
};

#endif