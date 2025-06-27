#ifndef GUI_COLORS_H
#define GUI_COLORS_H

// Convert hex color to RGB565 format
// The hex color format is 0xRRGGBB, and RGB565 format is 0xRRRRRGGGGGGBBBBB
#define HEX_RGB565(hex) ((((hex) >> 16 & 0xF8) << 8) | (((hex) >> 8 & 0xFC) << 3) | ((hex) & 0xFF >> 3))

#define COLOR_BG HEX_RGB565(0x000000)              // Dark center background
#define COLOR_HEADER_PRIMARY HEX_RGB565(0xFF6600)  // Orange top/bottom
#define COLOR_HEADER_RUNNING HEX_RGB565(0x008610)  // Orange top/bottom
#define COLOR_HEADER_ACTIVE HEX_RGB565(0x0083FE)   // Blue for active state
#define COLOR_HEADER_SELECTED HEX_RGB565(0x003D77) // Dark Blue for active selected state
#define COLOR_TEXT HEX_RGB565(0xFFFFFF)
#define COLOR_BORDER HEX_RGB565(0x204090) // Gray border color

#define COLOR_CHART_SMOKER HEX_RGB565(0xFF6600)    // Red for smoker temperature
#define COLOR_CHART_FOOD HEX_RGB565(0x0083FE)      // Green for food temperature
#define COLOR_CHART_TARGET HEX_RGB565(0x00FFD0)    // Blue for target temperature
#define COLOR_CHART_GRIDLINES HEX_RGB565(0x202020) // Black for chart background

#endif // GUI_COLORS_H