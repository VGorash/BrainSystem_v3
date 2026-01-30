#ifndef DISPLAY_H
#define DISPLAY_H

#define I2C_SDA 32
#define I2C_SCL 25
#define RST_N_PIN 33
#define INT_N_PIN 39
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

#include "hardware_config.h"
#include "graphics.h"

#include <TFT_eSPI.h>
#include <FT6336U.h>
#include <OpenFontRender.h>
#include <EncButton.h>

#include "src/Framework/Core/Hal.h"

// custom button values
#ifdef USE_WIRELESS_LINK
  #define BUTTON_WIRELESS 0
#endif

class Display
{
public:
  Display();
  ~Display();

  void init();

  void tick();

  void sync(const DisplayState& state);
  vgs::ButtonState syncTouchscreen();

private:
  void initElements();

  TextElement* createTextElement(coordinates_t coordinates);
  BitmapElement* createBitmapElement(coordinates_t coordinates);

private:
  DisplayState m_state;

  TFT_eSPI m_tft;
  OpenFontRender m_fontRender;

  FT6336U m_touchscreen = FT6336U(I2C_SDA, I2C_SCL, RST_N_PIN, INT_N_PIN);
  int m_touchscreenState;
  VirtButton m_buttonEmulator;
  bool m_touched;
  int m_touchX;
  int m_touchY;

  Element** m_elements;
  int m_numElements;
};

#endif
