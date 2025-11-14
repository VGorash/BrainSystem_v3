#include "Display.h"
#include "binaryttf.h"
#include "icons.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
#define MAX_ELEMENTS 64

using namespace vgs;

constexpr int playerColors[NUM_PLAYERS] = {TFT_YELLOW, TFT_BLUE, TFT_GREEN, TFT_RED};

volatile bool touchAvailable = false;
void IRAM_ATTR touchISR() { touchAvailable = true;}

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
  m_buttonEmulator.setDebTimeout(0);
  m_touchscreen.begin();
  attachInterrupt(digitalPinToInterrupt(INT_N_PIN), touchISR, FALLING);
  m_tft.init();
  m_tft.setRotation(3);
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

BitmapElement* Display::createBitmapElement(coordinates_t coordinates)
{
  BitmapElement* e = new BitmapElement(&m_tft, &m_fontRender, coordinates);
  m_elements[m_numElements++] = e;
  return e;
}

void Display::tick()
{
  int touchscreenState = digitalRead(INT_N_PIN);

  if(touchAvailable)
  {
    touchAvailable = false;
    m_touched = (m_touchscreen.read_td_status() > 0);
    // swap x and y axis because it's different for display and touchscreen
    m_touchX = SCREEN_WIDTH - m_touchscreen.read_touch1_y();
    m_touchY = m_touchscreen.read_touch1_x();
  }

  m_buttonEmulator.tick(m_touched);

  if(m_state.dirty)
  {
    for(int i=0; i<m_numElements; i++)
    {
      m_elements[i]->update(m_state);
    }

    if(m_state.modeChanged)
    {
      m_tft.fillScreen(COMMON_BACKGROUND_COLOR);
    }

    for(int i=0; i<m_numElements; i++)
    {
      m_elements[i]->show(m_state.modeChanged);
    }

    m_state.modeChanged = false;
    m_state.dirty = false;
  }
}

void Display::sync(const DisplayState& state)
{
  bool modeChanged = m_state.mode != state.mode;
  m_state = state;
  m_state.dirty = true;
  m_state.modeChanged = modeChanged;
}

ButtonState Display::syncTouchscreen()
{
  m_state.button_state = ButtonState();

  if(m_buttonEmulator.press())
  {
    for(int i=0; i<m_numElements; i++)
    {
      m_elements[i]->onPress(m_state, m_touchX, m_touchY);
    }
    //m_tft.drawLine(m_touchX, 0, m_touchX, SCREEN_HEIGHT, TFT_RED);
    //m_tft.drawLine(0, m_touchY, SCREEN_WIDTH, m_touchY, TFT_RED);
  }
  if(m_buttonEmulator.hold())
  {
    for(int i=0; i<m_numElements; i++)
    {
      m_elements[i]->onHold(m_state, m_touchX, m_touchY);
    }
  }
  if(m_buttonEmulator.click())
  {
    for(int i=0; i<m_numElements; i++)
    {
      m_elements[i]->onClick(m_state, m_touchX, m_touchY);
    }
  }

  return m_state.button_state;
}

// GRAPHICS

void startStopButtonUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;
  if(state.mode != DisplayMode::Game)
  {
    e->setHidden(true);
    return;
  }
  if(String(state.game.name).indexOf("БРЕЙН-РИНГ") >= 0 && state.game.state == GameState::Press)
  {
    e->setHidden(true);
    return;
  }
  e->setHidden(false);
  if(state.game.state == GameState::Idle)
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
  if(state.game.state == GameState::Idle)
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

void brainStartStopButtonUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;
  if(state.mode != DisplayMode::Game)
  {
    e->setHidden(true);
    return;
  }
  if(String(state.game.name).indexOf("БРЕЙН-РИНГ") < 0 || state.game.state != GameState::Press)
  {
    e->setHidden(true);
    return;
  }
  e->setHidden(false);
}

void brainStartButtonOnClick(DisplayState& state, Element* e)
{
  state.button_state.start = true;
}

TextElement* setupBrainStartButton(TextElement* e)
{
  e->setTextColor(TFT_BLACK);
  e->setBorderRadius(0);
  e->setFontSize(30);
  e->setText("20 секунд");
  e->setBackgroundColor(TFT_GREEN);
  e->setBorderColor(TFT_GREEN);
  e->setUpdateCallback(brainStartStopButtonUpdate);
  e->setOnPressCallback(brainStartButtonOnClick);
  return e;
}

void brainStopButtonOnClick(DisplayState& state, Element* e)
{
  state.button_state.stop = true;
}

TextElement* setupBrainStopButton(TextElement* e)
{
  e->setTextColor(TFT_BLACK);
  e->setBorderRadius(0);
  e->setFontSize(30);
  e->setText("Сброс");
  e->setBackgroundColor(TFT_RED);
  e->setBorderColor(TFT_RED);
  e->setUpdateCallback(brainStartStopButtonUpdate);
  e->setOnPressCallback(brainStopButtonOnClick);
  return e;
}

void mainPanelUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;

  if(state.mode != DisplayMode::Game)
  {
    e->setHidden(true);
    return;
  }

  e->setHidden(false);

  switch (state.game.state)
  {
    case GameState::Idle:
    {
      e->setText("--");
      e->setTextColor(TFT_WHITE);
      break;
    }
    case GameState::Countdown:
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
    case GameState::Press:
    {
      if(state.game.player >= NUM_PLAYERS)
      {
        e->setText(String("С") + String(state.game.player / 16 + 1) + String("-К") + String(state.game.player % 16 - NUM_PLAYERS + 1));
      }
      else
      {
        e->setText(String("К") + String(state.game.player + 1));
      }
      e->setTextColor(playerColors[state.game.player % NUM_PLAYERS]);
      break;
    }
    case GameState::Falstart:
    {
      e->setText("ФС");
      e->setTextColor(playerColors[state.game.player % NUM_PLAYERS]);
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

void pressTimePanelUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;

  if(state.mode != DisplayMode::Game)
  {
    e->setHidden(true);
    return;
  }

  e->setHidden(false);

  if(state.game.mode != GameMode::Falstart || state.game.pressTime < 0)
  {
    e->setText("");
    return;
  }

  int t = state.game.pressTime;

  char c[6];
  sprintf(c, "%d", t / 1000);

  String text = String(c) + String(",");

  if (t > 1000)
  {
    sprintf(c, "%d", (t % 1000) / 100);
  }
  else
  {
    sprintf(c, "%02d", (t % 1000) / 10);
  }

  text = text + String(c);

  e->setText(text);
}

TextElement* setupPressTimePanel(TextElement* e)
{
  e->setTextColor(TFT_WHITE);
  e->setBackgroundColor(COMMON_BACKGROUND_COLOR);
  e->setFontSize(30);
  e->setUpdateCallback(pressTimePanelUpdate);
  return e;
}

void titlePanelOnClick(DisplayState& state, Element* e)
{
  if(String(state.game.name) == "ЧГК")
  {
    state.button_state.enter = true;
  } 
}

void titlePanelUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;
  e->setHidden(false);

  if(state.mode == DisplayMode::Game)
  {
    String title = state.game.name;

    if(title == "ЧГК")
    {
      if(state.game.customInt == 1)
      {
        title = title + " (ОБЫЧНЫЙ)";
      }
      else
      {
        title = title + " (БЛИЦ " + String(state.game.customInt) + ")";
      }
    }

    e->setText(title);
  }
  if(state.mode == DisplayMode::Settings)
  {
    if(state.settings.edit_mode)
    {
      String settingName = String(state.settings.settings->getCurrentItem().getName());
      settingName.toUpperCase();
      e->setText(settingName);
    }
    else
    {
      e->setText("НАСТРОЙКИ");
    }
  }
}

TextElement* setupTitlePanel(TextElement* e)
{
  e->setTextColor(TFT_WHITE);
  e->setBackgroundColor(COMMON_BACKGROUND_COLOR);
  e->setFontSize(30);
  e->setUpdateCallback(titlePanelUpdate);
  e->setOnClickCallback(titlePanelOnClick);
  return e;
}

void settingsIconOnClick(DisplayState& state, Element* e)
{
  state.button_state.menu = true;
}

void settingsIconUpdate(const DisplayState& state, Element* eRaw)
{
  BitmapElement* e = (BitmapElement*) eRaw;

  e->setHidden(false);

  if(state.mode == DisplayMode::Game && state.game.state == GameState::Idle)
  {
    e->setBitmap(bitmap_settings_30_30);
  }
  else if(state.mode == DisplayMode::Settings && state.settings.edit_mode)
  {
    e->setBitmap(bitmap_back_30_30);
  }
  else if(state.mode == DisplayMode::Settings)
  {
    e->setBitmap(bitmap_close_30_30);
  }
  else
  {
    e->setBitmap(nullptr);
  }
}

BitmapElement* setupSettingsIcon(BitmapElement* e)
{
  e->setBitmapColor(TFT_WHITE);
  e->setBackgroundColor(COMMON_BACKGROUND_COLOR);
  e->setUpdateCallback(settingsIconUpdate);
  e->setOnPressCallback(settingsIconOnClick);
  return e;
}

void modePanelUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;
  if(state.mode != DisplayMode::Game || String(state.game.name) == "ЧГК")
  {
    e->setHidden(true);
    return;
  }
  e->setHidden(false);

  constexpr const char* modeNames[3] = {"Б/Ф", "Ф/С", "ХС"};
  e->setText(modeNames[static_cast<int>(state.game.mode)]);
}

TextElement* setupModePanel(TextElement* e)
{
  e->setTextColor(TFT_WHITE);
  e->setBackgroundColor(COMMON_BACKGROUND_COLOR);
  e->setFontSize(30);
  e->setUpdateCallback(modePanelUpdate);
  return e;
}

void settingsPanelOnClick(DisplayState& state, Element* eRaw)
{
  if(state.settings.edit_mode)
  {
    state.button_state.menu = true;
  }
  else
  {
    state.button_state.enter = true;
  }
}

void settingsPanelUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;

  if(state.mode != DisplayMode::Settings)
  {
    e->setHidden(true);
    return;
  }
  e->setHidden(false);

  if(state.settings.edit_mode)
  {
    e->setText(state.settings.settings->getCurrentItem().getValueStr());
  }
  else
  {
    e->setText(state.settings.settings->getCurrentItem().getName());
  }
}

TextElement* setupSettingsPanel(TextElement* e)
{
  e->setTextColor(TFT_WHITE);
  e->setBackgroundColor(COMMON_BACKGROUND_COLOR);
  e->setFontSize(40);
  e->setUpdateCallback(settingsPanelUpdate);
  e->setOnPressCallback(settingsPanelOnClick);
  return e;
}

void settingsButtonUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;

  if(state.mode != DisplayMode::Settings)
  {
    e->setHidden(true);
    return;
  }
  e->setHidden(false);
}


void setupSettingsButton(BitmapElement* e)
{
  e->setBitmapColor(TFT_WHITE);
  e->setBackgroundColor(COMMON_BACKGROUND_COLOR);
  e->setUpdateCallback(settingsButtonUpdate);
  e->setBitmap(nullptr);
}

BitmapElement* setupSettingsIncrementButton(BitmapElement* e)
{
  setupSettingsButton(e);
  e->setOnPressCallback([](DisplayState& state, Element* eRaw){state.button_state.start = true;});
  e->setBitmap(bitmap_arrow_right_240_80);
  return e;
}

BitmapElement* setupSettingsDecrementButton(BitmapElement* e)
{
  setupSettingsButton(e);
  e->setOnPressCallback([](DisplayState& state, Element* eRaw){state.button_state.stop = true;});
  e->setBitmap(bitmap_arrow_left_240_80);
  return e;
}

void settingsDetailsPanelUpdate(const DisplayState& state, Element* eRaw)
{
  TextElement* e = (TextElement*) eRaw;

  if(state.mode != DisplayMode::Settings)
  {
    e->setHidden(true);
    return;
  }

  e->setHidden(false);

  if(state.settings.edit_mode)
  {
    e->setText("");
  }
  else
  {
    e->setText(state.settings.settings->getCurrentItem().getValueStr());
  }
}

TextElement* setupSettingsDetailsPanel(TextElement* e)
{
  e->setTextColor(TFT_WHITE);
  e->setBackgroundColor(COMMON_BACKGROUND_COLOR);
  e->setFontSize(20);
  e->setUpdateCallback(settingsDetailsPanelUpdate);
  return e;
}


void Display::initElements()
{
  m_tft.fillScreen(COMMON_BACKGROUND_COLOR);
  // GAME WINDOW
  TextElement* startStopButton = setupStartStopButton(createTextElement({0, 200, 480, 120}));
  TextElement* brainStopButton = setupBrainStopButton(createTextElement({0, 200, 240, 120}));
  TextElement* brainStartButton = setupBrainStartButton(createTextElement({240, 200, 240, 120}));
  TextElement* mainPanel = setupMainPanel(createTextElement({0, 70, 480, 110}));
  TextElement* pressTimePanel = setupPressTimePanel(createTextElement({0, 45, 480, 35}));

  // SETTINGS WINDOW
  TextElement* settingsPanel = setupSettingsPanel(createTextElement({40, 100, 400, 60}));
  TextElement* settingsDetailsPanel = setupSettingsDetailsPanel(createTextElement({40, 160, 400, 30}));
  BitmapElement* setingsDecrementButton= setupSettingsDecrementButton(createBitmapElement({0, 240, 240, 80}));
  BitmapElement* setingsIncrementButton = setupSettingsIncrementButton(createBitmapElement({240, 240, 240, 80}));

  // TOP PANEL
  TextElement* titlePanel = setupTitlePanel(createTextElement({80, 0, 320, 40}));
  TextElement* modePanel = setupModePanel(createTextElement({400, 0, 80, 40}));
  BitmapElement* setingsIcon = setupSettingsIcon(createBitmapElement({5, 5, 30, 30}));
}
