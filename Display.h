#ifndef DISPLAY_H
#define DISPLAY_H

#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

#include <TFT_eSPI.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <OpenFontRender.h>
#include <EncButton.h>

#include "graphics.h"
#include "Hal.h"

#define NUM_PLAYERS 4

class Display
{
public:
  Display();
  ~Display();

  void init();

  void tick();

  void sync(const DisplayState& state);
  ButtonState syncTouchscreen();

private:
  void initElements();

  TextElement* createTextElement(coordinates_t coordinates);
  BitmapElement* createBitmapElement(coordinates_t coordinates);

private:
  DisplayState m_state;

  TFT_eSPI m_tft;
  OpenFontRender m_fontRender;

  SPIClass m_touchscreenSpi = SPIClass(VSPI);
  XPT2046_Touchscreen m_touchscreen = XPT2046_Touchscreen(XPT2046_CS, XPT2046_IRQ);
  VirtButton m_buttonEmulator;
  int m_touchX;
  int m_touchY;

  Element** m_elements;
  int m_numElements;
};

#endif