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
  for (int i=0; i<NUM_LINKS; i++)
  {
    m_links[i] = nullptr;
  }
}

HalImpl::~HalImpl()
{
  for (int i=0; i<NUM_LINKS; i++)
  {
    delete m_links[i];
  }
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

  pinMode(UART1_RX, INPUT_PULLUP);
  pinMode(UART2_RX, INPUT_PULLUP);

  Serial.begin(9600, SERIAL_8N1);
  Serial1.begin(9600, SERIAL_8N1, UART1_RX, UART1_TX);
  Serial2.begin(9600, SERIAL_8N1, UART2_RX, UART2_TX);

  WirelessLink* wirelessLink = new WirelessLink();
  wirelessLink->init();

  m_links[0] = new link::ArduinoUartLink(&Serial);
  m_links[1] = new link::ArduinoUartLink(&Serial1);
  m_links[2] = new link::ArduinoUartLink(&Serial2);
  m_links[3] = wirelessLink;

  m_blinkTimer.setTime(500);
  m_blinkTimer.setPeriodMode(true);
  m_soundTimer.setPeriodMode(false);
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

  for (int i=0; i<NUM_LINKS; i++)
  {
    m_links[i]->tick();
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
    for (int i=0; i<NUM_LINKS; i++)
    {
      if(m_links[i]->getCommand() == link::Command::ButtonPressed)
      {
        s.player = m_links[i]->getData() + NUM_PLAYERS + link::Link::maxPlayers * i;
        break;
      }
    }
  }

  return s;
}

void HalImpl::correctPressSignal(int player)
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

  if(player < link::Link::maxPlayers * NUM_LINKS)
  {
    sendLinkCommand(player / link::Link::maxPlayers, true, link::Command::CorrectPressSignal, player % link::Link::maxPlayers);
  }

  for (int i=0; i<NUM_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::DisplayCorrectPressSignal, player % link::Link::maxPlayers);
  }
}

void HalImpl::falstartPressSignal(int player)
{
  if (player < 0)
  {
    return;
  }
  if(player < NUM_PLAYERS)
  {
    blinkLed(player);
    return;
  }

  player -= NUM_PLAYERS;

  if(player < link::Link::maxPlayers * NUM_LINKS)
  {
    sendLinkCommand(player / link::Link::maxPlayers, true, link::Command::FalstartPressSignal, player % link::Link::maxPlayers);
  }

  for (int i=0; i<NUM_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::DisplayFalstartPressSignal, player % link::Link::maxPlayers);
  }
}

void HalImpl::pendingPressSignal(int player)
{
  if (player < 0)
  {
    return;
  }
  if(player < NUM_PLAYERS)
  {
    blinkLed(player);
    return;
  }

  player -= NUM_PLAYERS;

  if(player < link::Link::maxPlayers * NUM_LINKS)
  {
    sendLinkCommand(player / link::Link::maxPlayers, true, link::Command::PendingPressSignal, player % link::Link::maxPlayers);
  }
}

void HalImpl::gameStartSignal()
{
  if(m_signalLightEnabled)
  {
    digitalWrite(LED_SIGNAL, 1);
  }

  for (int i=0; i<NUM_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::GameStartSignal);
  }
}

void HalImpl::clearSignals()
{
  digitalWrite(LED_SIGNAL, 0);

  for(int i=0; i<NUM_PLAYERS; i++)
  {
    digitalWrite(playerLedPins[i], 0);
    m_blinkingLeds[i] = false;
  }

  m_blinkTimer.stop();

  for (int i=0; i<NUM_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::Clear);
  }
}

void HalImpl::setSignalLightEnabled(bool enabled)
{
  m_signalLightEnabled = enabled;
}

void HalImpl::blinkLed(int player)
{
  digitalWrite(playerLedPins[player], 1);
  m_blinkingLeds[player] = true;

  if(!m_blinkTimer.isStarted())
  {
    m_blinkState = 1;
    m_blinkTimer.start(*this);
  }
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

void HalImpl::setUartLinkVersion(vgs::link::UartLinkVersion version)
{
  delete m_links[0];
  m_links[0] = new link::ArduinoUartLink(&Serial, version);
  delete m_links[1];
  m_links[1] = new link::ArduinoUartLink(&Serial1, version);
  delete m_links[2];
  m_links[2] = new link::ArduinoUartLink(&Serial2, version);
}

Preferences& HalImpl::getPreferences()
{
  return m_preferences;
}

void HalImpl::sendLinkCommand(int linkNumber, bool useLink, vgs::link::Command command, unsigned int data)
{
  if(!useLink || linkNumber >= NUM_LINKS)
  {
    return;
  }

  m_links[linkNumber]->send(command, data);
}
