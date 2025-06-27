#ifndef GUITABB_H
#define GUITABB_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "types.h"
#include "guicolors.h"

#define GUI_TAB_TEXT_OFFSET_X 10 // Offset for the tab text from the left edge
#define GUI_TAB_TEXT_OFFSET_Y 12 // Offset for the tab text from the left edge
#define GUI_TAB_TEXT_SIZE 2      // Text size for the tab name

enum GUI_TAB_STATE
{
    GUI_TAB_STATE_NULL, // Null state for uninitialized tabs
    GUI_TAB_STATE_INACTIVE,
    GUI_TAB_STATE_SELECTED,
    GUI_TAB_STATE_ACTIVE,
};

class GuiTab
{
public:
    GuiTab(Adafruit_ST7789 &displayRef, int x0, int y0, int width, int height, String name);
    void update(ulong currentTimeMsec);
    void setInactive();
    void setSelected();
    void setActive();
    GUI_TAB_STATE getState();

private:
    Adafruit_ST7789 &m_tft;    // Reference to the display object
    String m_name;             // Name of the tab
    int m_x0;                  // X coordinate of the top-left corner
    int m_y0;                  // Y coordinate of the top-left corner
    int m_width;               // Width of the tab
    int m_height;              // Height of the tab
    GUI_TAB_STATE m_state;     // Current state of the tab (inactive, selected, active
    GUI_TAB_STATE m_prevState; // Previous state for comparison
    ulong m_currentTimeMsec;   // Current time in milliseconds

    void draw(); // Draw the tab based on its current state
};

#endif // GUITABB_H