#include "SettingsApp.h"
#include "HalImpl.h"

#include "src/Framework/Game.h"
#include "src/Framework/JeopardyGame.h"
#include "src/Framework/BrainRingGame.h"
#include "src/Framework/QuestionsGame.h"

#include "src/Settings/ListSettingsItem.h"

using namespace vgs;

IApp* createGame(const GameConfig& config)
{
  return new Game(config);
}

IApp* createJeopardyGame(const GameConfig& config)
{
  return new JeopardyGame(config);
}

IApp* createBrainRingGame(const GameConfig& config, int primaryTime, int secondaryTime)
{
  GameConfig newConfig = config;
  newConfig.time.primary = primaryTime;
  newConfig.time.secondary = secondaryTime;
  return new BrainRingGame(newConfig);
}

IApp* createBrainRingGame60(const GameConfig& config)
{
  return createBrainRingGame(config, 60, 20);
}

IApp* createBrainRingGame40(const GameConfig& config)
{
  return createBrainRingGame(config, 40, 20);
}

IApp* createQuestionsGame(const GameConfig& config)
{
  return new QuestionsGame(config);
}

typedef IApp* (*GameConstructor)(const GameConfig&);

constexpr int gameCount = 5;
constexpr GameConstructor gameConstructors[gameCount] = {createGame, createJeopardyGame, createBrainRingGame60, createBrainRingGame40, createQuestionsGame};

constexpr const char* gameNames[gameCount] = {"БЕЗ ОТСЧЕТА", "СВОЯ ИГРА", "БРЕЙН-РИНГ 60+20", "БРЕЙН-РИНГ 40+20", "ЧГК"};
constexpr const char* modeNames[3] = {"БЕЗ ФАЛЬСТАРТОВ", "С ФАЛЬСТАРТАМИ", "ХАМСА"};
constexpr const char* onOffNames[2] = {"ВКЛ", "ВЫКЛ"};
constexpr const char* linkModes[2] = {"V1 (устаревший)", "V2 (обычный)"};

SettingsApp::SettingsApp(bool launchGame) : m_launchGame(launchGame)
{
  m_settings.addItem(new settings::ListSettingsItem("Тип игры", gameCount, gameNames));  
  m_settings.addItem(new settings::ListSettingsItem("Режим", 3, modeNames));
  m_settings.addItem(new settings::ListSettingsItem("Звук", 2, onOffNames));
  m_settings.addItem(new settings::ListSettingsItem("Свет", 2, onOffNames));
  m_settings.addItem(new settings::ListSettingsItem("Link", 2, linkModes));
}

void SettingsApp::init(IHal& hal)
{
  loadSettings(hal);

  if(m_launchGame)
  {
    m_launchGame = false;
    exit(hal);
  }
}

void SettingsApp::tick(IHal& hal)
{
  if(m_shouldClose)
  {
    return;
  }

  if(m_editMode)
  {
    processEdit(hal);
  }
  else
  {
    processIdle(hal);
  }

  if(m_displayDirty)
  {
    SettingsDisplayInfo info;
    info.settings = &m_settings;
    info.edit_mode = m_editMode;

    CustomDisplayInfo customInfo;
    customInfo.type = DisplayInfoSettings;
    customInfo.data = (void*)&info;

    hal.updateDisplay(customInfo);
    m_displayDirty = false;
  }
}

void SettingsApp::processIdle(IHal& hal)
{
  ButtonState buttonState = hal.getButtonState();

  if(buttonState.menu)
  {
    exit(hal);
    return;
  }
  if(buttonState.enter)
  {
    m_editMode = true;
    m_displayDirty = true;
    return;
  }
  if(buttonState.start)
  {
    m_settings.moveNext();
    m_displayDirty = true;
    return;
  }
  if(buttonState.stop)
  {
    m_settings.movePrevious();
    m_displayDirty = true;
    return;
  }
}

void SettingsApp::processEdit(IHal& hal)
{
  ButtonState buttonState = hal.getButtonState();

  if(buttonState.menu)
  {
    m_editMode = false;
    m_displayDirty = true;
    return;
  }
  if(buttonState.start)
  {
    m_settings.getCurrentItem().increment();
    m_displayDirty = true;
    m_settingsDirty = true;
    return;
  }
  if(buttonState.stop)
  {
    m_settings.getCurrentItem().decrement();
    m_displayDirty = true;
    m_settingsDirty = true;
    return;
  }
}

void SettingsApp::exit(IHal& hal)
{
  HalImpl* halImpl = (HalImpl*) &hal;

  if(m_settingsDirty)
  {
    saveSettings(hal);
    m_settingsDirty = false;
  }

  int settingsState[m_settings.size()];
  m_settings.dumpData(settingsState);
  int soundMode = settingsState[2];
  bool signalLightDisabled = settingsState[3];
  int linkVersion = settingsState[4];

  halImpl->setSoundMode(static_cast<HalSoundMode>(soundMode));
  halImpl->setSignalLightEnabled(!signalLightDisabled);
  halImpl->setUartLinkVersion(static_cast<vgs::link::UartLinkVersion>(linkVersion));

  m_shouldClose = true;
}

void SettingsApp::loadSettings(IHal& hal)
{
  HalImpl* halImpl = (HalImpl*) &hal;

  Preferences& preferences = halImpl->getPreferences();
  preferences.begin("settings", true);

  if(!preferences.isKey("values") || preferences.getBytesLength("values") != m_settings.size() * sizeof(int))
  {
    preferences.end();
    return;
  }

  int data[m_settings.size()];
  preferences.getBytes("values", data, m_settings.size() * sizeof(int));
  m_settings.loadData(data);

  preferences.end();
}

void SettingsApp::saveSettings(IHal& hal)
{
  HalImpl* halImpl = (HalImpl*) &hal;

  Preferences& preferences = halImpl->getPreferences();
  preferences.begin("settings", false);

  int data[m_settings.size()];
  m_settings.dumpData(data);
  preferences.putBytes("values", data, m_settings.size() * sizeof(int));

  preferences.end();
}

AppChangeType SettingsApp::appChangeNeeded()
{
  if(m_shouldClose)
  {
    m_displayDirty = true;
    m_shouldClose = false;
    return AppChangeType::Custom;
  }

  return AppChangeType::None;
}

IApp* SettingsApp::createCustomApp()
{
  int settingsState[m_settings.size()];
  m_settings.dumpData(settingsState);

  int gameNumber = settingsState[0];
  
  GameConfig config;
  config.displayed_name = gameNames[gameNumber];
  config.mode = static_cast<GameMode>(settingsState[1]);

  return gameConstructors[gameNumber](config);
}
