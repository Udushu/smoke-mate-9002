#include "tftdebug.h"



TFTDebug::TFTDebug(Adafruit_ST7789 *tft, int x, int y, int textSize)
    : tft(tft), cursor_x(x), cursor_y(y)
{
    tft->setTextSize(textSize);
    line_height = 8 * textSize; // 8 is default font height
    screen_width = tft->width();
    screen_height = tft->height();
}

void TFTDebug::println()
{
    tft->setCursor(cursor_x, cursor_y);
    tft->println();
    cursor_y += line_height;
    if (cursor_y + line_height > screen_height)
    {
        cursor_y = 10; // Reset to top
        tft->fillScreen(ST77XX_BLACK); // Clear screen when out of space
    }
}

void TFTDebug::print(const String &msg)
{
    tft->setCursor(cursor_x, cursor_y);
    tft->print(msg);
}

void TFTDebug::println(const String &msg)
{
    tft->setCursor(cursor_x, cursor_y);
    tft->print(msg);
    cursor_y += line_height;
    if (cursor_y + line_height > screen_height)
    {
        cursor_y = 10;
        tft->fillScreen(ST77XX_BLACK); // Clear screen when out of space
    }
}

void TFTDebug::print(int value)
{
    tft->setCursor(cursor_x, cursor_y);
    tft->print(value);
}

void TFTDebug::print(unsigned int value)
{
    tft->setCursor(cursor_x, cursor_y);
    tft->print(value);
}

void TFTDebug::print(long value)
{
    tft->setCursor(cursor_x, cursor_y);
    tft->print(value);
}

void TFTDebug::print(unsigned long value)
{
    tft->setCursor(cursor_x, cursor_y);
    tft->print(value);
}

// Overloads for println
void TFTDebug::println(int value)
{
    tft->setCursor(cursor_x, cursor_y);
    tft->print(value);
    cursor_y += line_height;
    if (cursor_y + line_height > screen_height)
    {
        cursor_y = 10;
        tft->fillScreen(ST77XX_BLACK);
    }
}

void TFTDebug::println(unsigned int value)
{
    tft->setCursor(cursor_x, cursor_y);
    tft->print(value);
    cursor_y += line_height;
    if (cursor_y + line_height > screen_height)
    {
        cursor_y = 10;
        tft->fillScreen(ST77XX_BLACK);
    }
}

void TFTDebug::println(long value)
{
    tft->setCursor(cursor_x, cursor_y);
    tft->print(value);
    cursor_y += line_height;
    if (cursor_y + line_height > screen_height)
    {
        cursor_y = 10;
        tft->fillScreen(ST77XX_BLACK);
    }
}

void TFTDebug::println(unsigned long value)
{
    tft->setCursor(cursor_x, cursor_y);
    tft->print(value);
    cursor_y += line_height;
    if (cursor_y + line_height > screen_height)
    {
        cursor_y = 10;
        tft->fillScreen(ST77XX_BLACK);
    }
}

void TFTDebug::clear()
{
    tft->fillScreen(ST77XX_BLACK);
    cursor_y = 10;
}