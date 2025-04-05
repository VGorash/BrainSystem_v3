#include "HalImpl.h"

#include <EEPROM.h>

#define BUTTON_PLAYER_4 34
#define BUTTON_PLAYER_3 35
#define BUTTON_PLAYER_2 5
#define BUTTON_PLAYER_1 4

#define LED_PLAYER_4 23
#define LED_PLAYER_3 22
#define LED_PLAYER_2 19
#define LED_PLAYER_1 12
#define LED_SIGNAL 21

#define BUZZER 18

#define SOUND_TONE_START 2000
#define SOUND_TONE_PRESS 1000
#define SOUND_TONE_FALSTART 500
#define SOUND_TONE_TICK 1500
#define SOUND_TONE_SIGNAL 1500
#define SOUND_TONE_END 250

#define SOUND_DURATION_START 1000
#define SOUND_DURATION_PRESS 1000
#define SOUND_DURATION_FALSTART 500
#define SOUND_DURATION_TICK 250
#define SOUND_DURATION_SIGNAL 1000
#define SOUND_DURATION_END 1000

static const int playerButtonPins[NUM_PLAYERS] = {BUTTON_PLAYER_1, BUTTON_PLAYER_2, BUTTON_PLAYER_3, BUTTON_PLAYER_4};
static const int playerLedPins[NUM_PLAYERS] = {LED_PLAYER_1, LED_PLAYER_2, LED_PLAYER_3, LED_PLAYER_4};

HalImpl::HalImpl()
{
  m_blinkTimer.setTime(500);
  m_blinkTimer.setPeriodMode(true);

  m_soundTimer.setPeriodMode(false);
}

HalImpl::~HalImpl()
{

}

void HalImpl::init()
{
  m_display.init();

  for(int i=0; i<NUM_PLAYERS; i++)
  {
    m_playerButtons[i].init(playerButtonPins[i], INPUT);
    pinMode(playerLedPins[i], OUTPUT);
  }

  pinMode(LED_SIGNAL, OUTPUT);

}

void HalImpl::tick()
{
  m_display.tick();

  if(m_blinkTimer.tick(this))
  {
    m_blinkState = !m_blinkState;

    for(int i=0; i<NUM_PLAYERS; i++)
    {
      if(m_blinkingLeds[i])
      {
        digitalWrite(playerLedPins[i], m_blinkState);
      }
    }
  }

  for(int i=0; i<NUM_PLAYERS; i++)
  {
    m_playerButtons[i].tick();
  }

  if(m_soundTimer.tick(this))
  {
    ledcDetach(BUZZER);
  }
}


ButtonState HalImpl::getButtonState()
{
  ButtonState s = m_display.syncTouchscreen();

  for(int i=0; i<NUM_PLAYERS; i++)
  {
    if(m_playerButtons[i].press())
    {
      s.player = i;
      break;
    }
  }

  return s;
}

void HalImpl::playerLedOn(int player)
{
  if (player < 0)
  {
    return;
  }
  if(player < 4)
  {
    digitalWrite(playerLedPins[player], 1);
    return;
  }
}

void HalImpl::playerLedBlink(int player)
{
  if (player < 0)
  {
    return;
  }
  if(player < 4)
  {
    playerLedOn(player);
    m_blinkingLeds[player] = true;

    if(!m_blinkTimer.isStarted())
    {
      m_blinkState = 1;
      m_blinkTimer.start(this);
    }

    return;
  }
}

void HalImpl::signalLedOn()
{
  if(m_signalLightEnabled)
  {
    digitalWrite(LED_SIGNAL, 1);
  }
}

void HalImpl::ledsOff()
{
  digitalWrite(LED_SIGNAL, 0);

  for(int i=0; i<NUM_PLAYERS; i++)
  {
    digitalWrite(playerLedPins[i], 0);
    m_blinkingLeds[i] = false;
  }

  m_blinkTimer.stop();
}

void HalImpl::setSignalLightEnabled(bool enabled)
{
  m_signalLightEnabled = enabled;
}

void HalImpl::sound(HalSound soundType)
{
  if(m_soundMode == HalSoundMode::Disabled)
  {
    ledcDetach(BUZZER);
    return;
  }

  switch(soundType)
  {
    case HalSound::Start:
      sound(SOUND_TONE_START, SOUND_DURATION_START);
      break;
    case HalSound::Press:
      sound(SOUND_TONE_PRESS, SOUND_DURATION_PRESS);
      break;
    case HalSound::Falstart:
      sound(SOUND_TONE_FALSTART, SOUND_DURATION_FALSTART);
      break;
    case HalSound::Tick:
      sound(SOUND_TONE_TICK, SOUND_DURATION_TICK);
      break;
    case HalSound::Signal:
      sound(SOUND_TONE_SIGNAL, SOUND_DURATION_SIGNAL);
      break;
    case HalSound::End:
      sound(SOUND_TONE_END, SOUND_DURATION_END);
      break;
    case HalSound::None:
      ledcDetach(BUZZER);
      break;
  }
}

void HalImpl::sound(unsigned int frequency, unsigned int duration)
{
  m_soundTimer.setTime(duration);
  m_soundTimer.start(this);
  ledcAttach(BUZZER, 50, 10);
  ledcWriteTone(BUZZER, frequency);
}

void HalImpl::setSoundMode(HalSoundMode mode)
{
  m_soundMode = mode;
}

void HalImpl::updateDisplay(const GameDisplayInfo& info)
{
  DisplayState s;
  s.mode = DisplayMode::GAME;
  s.game = info;
  m_display.sync(s);
}

void HalImpl::updateDisplay(const CustomDisplayInfo& info)
{
  
}

void HalImpl::updateDisplay(const SettingsDisplayInfo& info)
{
  DisplayState s;
  s.mode = DisplayMode::SETTINGS;
  s.settings = info;
  m_display.sync(s);
}

unsigned long HalImpl::getTimeMillis()
{
  return millis();
}

void HalImpl::saveSettings(const Settings& settings)
{
  int data[settings.size()];
  settings.dumpData(data);

  for(int i=0; i<settings.size(); i++)
  {
    EEPROM.put(i * sizeof(int), data[i]);
  }

  EEPROM.commit();
}

void HalImpl::loadSettings(Settings& settings)
{
  int data[settings.size()];

  for(int i=0; i<settings.size(); i++)
  {
    EEPROM.get(i * sizeof(int), data[i]);
  }

  settings.loadData(data);
}
