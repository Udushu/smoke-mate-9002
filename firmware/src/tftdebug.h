#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

class TFTDebug {

    Adafruit_ST7789* tft;
    int cursor_x, cursor_y;
    int line_height;
    int screen_width, screen_height;

  public:
  
    TFTDebug(Adafruit_ST7789* tft, int x, int y, int textSize);
    void println();
    void print(const String& msg);
    void print(int value);
    void print(unsigned int value);
    void print(long value);
    void print(unsigned long value);
    void println(const String& msg);
    void println(int value);
    void println(unsigned int value);
    void println(long value);
    void println(unsigned long value);
    void clear();

};