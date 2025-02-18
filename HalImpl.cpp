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

static const int playerButtonPins[NUM_PLAYERS] = {BUTTON_PLAYER_1, BUTTON_PLAYER_2, BUTTON_PLAYER_3, BUTTON_PLAYER_4};
static const int playerLedPins[NUM_PLAYERS] = {LED_PLAYER_1, LED_PLAYER_2, LED_PLAYER_3, LED_PLAYER_4};

HalImpl::HalImpl()
{
  m_blinkTimer.setTime(500);
  m_blinkTimer.setPeriodMode(true);
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

  for(int i=0; i<NUM_PLAYERS; i++)
  {
    m_playerButtons[i].tick();

    if(m_playerButtons[i].press())
    {
      m_displayState.button_state.player = i;
      return;
    }
  }
}

const ButtonState& HalImpl::getButtonState()
{
  return m_displayState.button_state;
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

void HalImpl::sound(int frequency, int duration)
{

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
