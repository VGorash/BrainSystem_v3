#include "Display.h"
#include "binaryttf.h"
#include "graphics.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
#define MAX_ELEMENTS 64

static const int playerColors[NUM_PLAYERS] = {TFT_YELLOW, TFT_BLUE, TFT_GREEN, TFT_RED};

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

void Display::sync(const DisplayState& state)
{
  for(int i=0; i<m_numElements; i++)
  {
    m_elements[i]->update(state);
  }
  for(int i=0; i<m_numElements; i++)
  {
    m_elements[i]->show(false);
  }
}

void Display::syncTouchscreen(DisplayState& state)
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

void startStopButtonUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;
  if(state.mode != DisplayMode::GAME)
  {
    e->setHidden(true);
    return;
  }
  e->setHidden(false);
  if(state.game.state == GameState::IDLE)
  {
    e->setText("Запуск");
    e->setBackgroundColor(TFT_GREEN);
    e->setBorderColor(TFT_GREEN);
  }
  else
  {
    e->setText("Сброс");
    e->setBackgroundColor(TFT_RED);
    e->setBorderColor(TFT_RED);
  }
}

void startStopButtonOnClick(DisplayState& state, Element* e)
{
  if(state.game.state == GameState::IDLE)
  {
    state.button_state.start = true;
  }
  else
  {
    state.button_state.stop = true;
  }
}

TextElement* setupStartStopButton(TextElement* e)
{
  e->setTextColor(TFT_BLACK);
  e->setBorderRadius(0);
  e->setFontSize(30);
  e->setUpdateCallback(startStopButtonUpdate);
  e->setOnPressCallback(startStopButtonOnClick);
  return e;
}

void mainPanelUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;
  if(state.mode != DisplayMode::GAME)
  {
    e->setHidden(true);
    return;
  }
  e->setHidden(false);
  switch (state.game.state)
  {
    case GameState::IDLE:
    {
      e->setText("--");
      e->setTextColor(TFT_WHITE);
      break;
    }
    case GameState::COUNTDOWN:
    {
      if(state.game.gameTime >= 0)
      {
        e->setText(String(state.game.gameTime));
      }
      else
      {
        e->setText("--");
      }
      e->setTextColor(TFT_WHITE);
      break;
    }
    case GameState::PRESS:
    {
      e->setText(String("К") + String(state.game.player + 1));
      e->setTextColor(playerColors[state.game.player % 4]);
      break;
    }
    case GameState::FALSTART:
    {
      e->setText("ФС");
      e->setTextColor(playerColors[state.game.player % 4]);
      break;
    }
  }
}

TextElement* setupMainPanel(TextElement* e)
{
  e->setFontSize(110);
  e->setBackgroundColor(COMMON_BACKGROUND_COLOR);
  e->setUpdateCallback(mainPanelUpdate);
  return e;
}

void titlePanelUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;
  if(state.mode != DisplayMode::GAME)
  {
    e->setHidden(true);
    return;
  }
  e->setHidden(false);

  e->setText(state.game.name);
}

TextElement* setupTitlePanel(TextElement* e)
{
  e->setTextColor(TFT_WHITE);
  e->setBackgroundColor(COMMON_BACKGROUND_COLOR);
  e->setFontSize(30);
  e->setUpdateCallback(titlePanelUpdate);
  return e;
}


void Display::initElements()
{
  m_tft.fillScreen(COMMON_BACKGROUND_COLOR);
  TextElement* startStopButton = setupStartStopButton(createTextElement({0, 200, 480, 120}));
  TextElement* mainPanel = setupMainPanel(createTextElement({0, 70, 480, 110}));
  TextElement* titlePanel = setupTitlePanel(createTextElement({120, 5, 240, 30}));
}
