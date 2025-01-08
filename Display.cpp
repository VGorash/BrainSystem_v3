#include "Display.h"
#include "binaryttf.h"
#include "graphics.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
#define MAX_ELEMENTS 64

Display::Display()
{
  m_numElements = 0;
  m_elements = new Element*[MAX_ELEMENTS];
}

Display::~Display()
{
  for(int i=0; i<m_numElements; i++)
  {
    delete m_elements[i];
  }
  delete[] m_elements;
}

void Display::init()
{
  m_buttonEmulator.setDebTimeout(50);
  m_touchscreenSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  m_touchscreen.begin(m_touchscreenSpi);
  m_touchscreen.setRotation(1);
  m_tft.init();
  m_tft.setRotation(1);
  m_fontRender.loadFont(binaryttf, sizeof(binaryttf));
  m_fontRender.setDrawer(m_tft);
  m_fontRender.setBackgroundFillMethod(BgFillMethod::None);
  initElements();
}

TextElement* Display::createTextElement(coordinates_t coordinates)
{
  TextElement* e = new TextElement(&m_tft, &m_fontRender, coordinates);
  m_elements[m_numElements++] = e;
  return e;
}

void Display::sync(State& state)
{
  processTouchscreen(state);
  for(int i=0; i<m_numElements; i++)
  {
    m_elements[i]->update(state);
  }
  for(int i=0; i<m_numElements; i++)
  {
    m_elements[i]->show(false);
  }
}

void Display::showGreeting()
{
  m_tft.fillScreen(COMMON_BACKGROUND_COLOR);
}

void Display::processTouchscreen(State& state)
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
    for(int i=0; i<m_numElements; i++)
    {
      m_elements[i]->onPress(state, m_touchX, m_touchY);
    }
  }
  if(m_buttonEmulator.hold())
  {
    for(int i=0; i<m_numElements; i++)
    {
      m_elements[i]->onHold(state, m_touchX, m_touchY);
    }
  }
  if(m_buttonEmulator.click())
  {
    for(int i=0; i<m_numElements; i++)
    {
      m_elements[i]->onClick(state, m_touchX, m_touchY);
    }
  }
}

// GRAPHICS

void startStopButtonUpdate(const State& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;
  if(state.game.started)
  {
    e->setText("Сброс");
    e->setBackgroundColor(TFT_RED);
    e->setBorderColor(TFT_RED);
  }
  else
  {
    e->setText("Запуск");
    e->setBackgroundColor(TFT_GREEN);
    e->setBorderColor(TFT_GREEN);
  }
}

void startStopButtonOnClick(State& state, Element* e)
{
  state.game.started = !state.game.started;
}

TextElement* setupStartStopButton(TextElement* e)
{
  e->setTextColor(TFT_BLACK);
  e->setBorderRadius(5);
  e->setFontSize(30);
  e->setUpdateCallback(startStopButtonUpdate);
  e->setOnPressCallback(startStopButtonOnClick);
  return e;
}

void Display::initElements()
{
  TextElement* startStopButton = setupStartStopButton(createTextElement({20, 200, 440, 100}));
}
