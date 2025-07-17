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

#define UART1_RX 27
#define UART1_TX 26
#define UART2_RX 16
#define UART2_TX 17

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

using namespace vgs;

static const int playerButtonPins[NUM_PLAYERS] = {BUTTON_PLAYER_1, BUTTON_PLAYER_2, BUTTON_PLAYER_3, BUTTON_PLAYER_4};
static const int playerLedPins[NUM_PLAYERS] = {LED_PLAYER_1, LED_PLAYER_2, LED_PLAYER_3, LED_PLAYER_4};

HalImpl::HalImpl()
{
  m_blinkTimer.setTime(500);
  m_blinkTimer.setPeriodMode(true);

  m_soundTimer.setPeriodMode(false);
  
  m_uartLinks[0] = new link::ArduinoUartLink(&Serial);
  m_uartLinks[1] = new link::ArduinoUartLink(&Serial1);
  m_uartLinks[2] = new link::ArduinoUartLink(&Serial2);
}

HalImpl::~HalImpl()
{
  for (int i=0; i<NUM_UART_LINKS; i++)
  {
    delete m_uartLinks[i];
  }
}

void HalImpl::init()
{
  EEPROM.begin(512);

  m_display.init();

  for(int i=0; i<NUM_PLAYERS; i++)
  {
    m_playerButtons[i].init(playerButtonPins[i], INPUT);
    pinMode(playerLedPins[i], OUTPUT);
  }

  pinMode(LED_SIGNAL, OUTPUT);

  pinMode(UART1_RX, INPUT_PULLUP);
  pinMode(UART2_RX, INPUT_PULLUP);

  Serial.begin(9600, SERIAL_8N1);
  Serial1.begin(9600, SERIAL_8N1, UART1_RX, UART1_TX);
  Serial2.begin(9600, SERIAL_8N1, UART2_RX, UART2_TX);
}

void HalImpl::tick()
{
  m_display.tick();

  if(m_blinkTimer.tick(*this))
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

  if(m_soundTimer.tick(*this))
  {
    ledcDetach(BUZZER);
  }

  for (int i=0; i<NUM_UART_LINKS; i++)
  {
    m_uartLinks[i]->tick();
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

  if(s.player < 0)
  {
    for (int i=0; i<NUM_UART_LINKS; i++)
    {
      if(m_uartLinks[i]->getCommand() == link::Command::PlayerButton)
      {
        s.player = m_uartLinks[i]->getData() + NUM_PLAYERS + UART_LINK_MAX_PLAYERS * i;
        break;
      }
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

  if(player < NUM_PLAYERS)
  {
    digitalWrite(playerLedPins[player], 1);
    return;
  }

  player -= NUM_PLAYERS;

  if(player < UART_LINK_MAX_PLAYERS * NUM_UART_LINKS)
  {
    sendLinkCommand(player / UART_LINK_MAX_PLAYERS, true, link::Command::PlayerLedOn, player % UART_LINK_MAX_PLAYERS);
  }

  for (int i=0; i<NUM_UART_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::DisplayPlayerLedOn, player % UART_LINK_MAX_PLAYERS);
  }
}

void HalImpl::playerLedBlink(int player)
{
  if (player < 0)
  {
    return;
  }
  if(player < NUM_PLAYERS)
  {
    playerLedOn(player);
    m_blinkingLeds[player] = true;

    if(!m_blinkTimer.isStarted())
    {
      m_blinkState = 1;
      m_blinkTimer.start(*this);
    }

    return;
  }

  player -= NUM_PLAYERS;

  if(player < UART_LINK_MAX_PLAYERS * NUM_UART_LINKS)
  {
    sendLinkCommand(player / UART_LINK_MAX_PLAYERS, true, link::Command::PlayerLedBlink, player % UART_LINK_MAX_PLAYERS);
  }

  for (int i=0; i<NUM_UART_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::DisplayPlayerLedBlink, player % UART_LINK_MAX_PLAYERS);
  }
}

void HalImpl::signalLedOn()
{
  if(m_signalLightEnabled)
  {
    digitalWrite(LED_SIGNAL, 1);
  }

  for (int i=0; i<NUM_UART_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::SignalLedOn);
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

  for (int i=0; i<NUM_UART_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::LedsOff);
  }
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
  m_soundTimer.start(*this);
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
  s.mode = DisplayMode::Game;
  s.game = info;
  m_display.sync(s);
}

void HalImpl::updateDisplay(const CustomDisplayInfo& info)
{
  if(info.type == DisplayInfoSettings)
  {
    DisplayState s;
    s.mode = DisplayMode::Settings;
    s.settings = *((SettingsDisplayInfo*)info.data);
    m_display.sync(s);
  }
}

unsigned long HalImpl::getTimeMillis()
{
  return millis();
}

void HalImpl::saveSettings(const settings::Settings& settings)
{
  int data[settings.size()];
  settings.dumpData(data);

  for(int i=0; i<settings.size(); i++)
  {
    EEPROM.put(i * sizeof(int), data[i]);
  }

  EEPROM.commit();
}

void HalImpl::loadSettings(settings::Settings& settings)
{
  int data[settings.size()];

  for(int i=0; i<settings.size(); i++)
  {
    EEPROM.get(i * sizeof(int), data[i]);
  }

  settings.loadData(data);
}

void HalImpl::setLinkVersion(vgs::link::UartLinkVersion version)
{
  for (int i=0; i<NUM_UART_LINKS; i++)
  {
    delete m_uartLinks[i];
  }

  m_uartLinks[0] = new link::ArduinoUartLink(&Serial, version);
  m_uartLinks[1] = new link::ArduinoUartLink(&Serial1, version);
  m_uartLinks[2] = new link::ArduinoUartLink(&Serial2, version);
}

void HalImpl::sendLinkCommand(int linkNumber, bool useLink, vgs::link::Command command, unsigned int data)
{
  if(!useLink || linkNumber >= NUM_UART_LINKS)
  {
    return;
  }

  m_uartLinks[linkNumber]->send(command, data);
}
