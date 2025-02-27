#include "HalImpl.h"

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
#define SOUND_TONE_END 250

#define SOUND_DURATION_START 1000
#define SOUND_DURATION_PRESS 1000
#define SOUND_DURATION_FALSTART 500
#define SOUND_DURATION_TICK 250
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
  m_displayState.button_state = ButtonState();

  m_display.syncTouchscreen(m_displayState);

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

  if(m_soundTimer.tick(this))
  {
    ledcDetach(BUZZER);
  }

  for(int i=0; i<NUM_PLAYERS; i++)
  {
    m_playerButtons[i].tick();

    if(m_playerButtons[i].press())
    {
      m_displayState.button_state.player = i;
    }
  }
}


ButtonState HalImpl::getButtonState()
{
  ButtonState s = m_displayState.button_state;
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
  digitalWrite(LED_SIGNAL, 1);
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

void HalImpl::sound(HalSound soundType)
{
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
  return;
  m_soundTimer.setTime(duration);
  m_soundTimer.start(this);
  ledcAttach(BUZZER, 50, 10);
  ledcWriteTone(BUZZER, frequency);
}

void HalImpl::updateDisplay(const GameDisplayInfo& info)
{
  m_displayState.mode = DisplayMode::GAME;
  m_displayState.game = info;
  m_display.sync(m_displayState);
}

void HalImpl::updateDisplay(const SettingsDisplayInfo& info)
{

}

void HalImpl::updateDisplay(const CustomDisplayInfo& info)
{

}

unsigned long HalImpl::getTimeMillis()
{
  return millis();
}
