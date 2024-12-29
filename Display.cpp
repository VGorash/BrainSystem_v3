#include "Display.h"
#include "binaryttf.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
#define FONT_SIZE 4

void Display::init()
{
  m_buttonEmulator.setDebTimeout(10);
  m_touchscreenSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  m_touchscreen.begin(m_touchscreenSpi);
  m_touchscreen.setRotation(1);
  m_tft.init();
  m_tft.setRotation(1);
  m_fontRender.loadFont(binaryttf, sizeof(binaryttf));
  m_fontRender.setDrawer(m_tft);
}

void Display::showGreeting()
{
  m_tft.fillScreen(0x0006);
  m_fontRender.setFontColor(TFT_WHITE, TFT_WHITE);
  m_fontRender.printf("Hello World\n");
  m_fontRender.seekCursor(0, 10);

  m_fontRender.setFontSize(15);
  m_fontRender.setFontColor(TFT_WHITE, TFT_WHITE);
  m_fontRender.printf("СЪЕШЬ ЕЩЕ ЭТИХ МЯГКИХ ФРАНЦУЗСКИХ БУЛОК ДА ВЫПЕЙ ЧАЮ\n");
  m_fontRender.seekCursor(0, 10);

  m_fontRender.setFontSize(15);
  m_fontRender.setFontColor(TFT_WHITE, TFT_WHITE);
  m_fontRender.printf("съешь еще этих мягких французских булок да выпей чаю");
}

void Display::processTouchScreen()
{
  bool touched = m_touchscreen.tirqTouched() && m_touchscreen.touched();
  if(touched)
  {
    TS_Point p = m_touchscreen.getPoint();
    m_touchX = SCREEN_WIDTH - map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    m_touchY = SCREEN_HEIGHT - map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
  }
  m_buttonEmulator.tick(touched);
  if(m_buttonEmulator.press())
  {
    m_tft.fillScreen(0x0006);
    m_tft.drawLine(m_touchX, 0, m_touchX, SCREEN_HEIGHT, TFT_RED);
    m_tft.drawLine(0, m_touchY, SCREEN_WIDTH, m_touchY, TFT_RED);
  }
}
