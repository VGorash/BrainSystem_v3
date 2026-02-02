#include "HalImpl.h"
#include "src/Framework/sounds.h"

using namespace vgs;

HalImpl::HalImpl()
{
#ifdef USE_LINKS
  for (int i=0; i<NUM_LINKS; i++)
  {
    m_links[i] = nullptr;
  }
#endif

#ifdef USE_LED_STRIP
  m_ledStrip = new Adafruit_NeoPixel(LED_STRIP_LEDS_COUNT, LED_STRIP_PIN, LED_STRIP_COLOR_ORDER + LED_STRIP_FREQUENCY);
#endif
}

HalImpl::~HalImpl()
{
#ifdef USE_LINKS
  for (int i=0; i<NUM_LINKS; i++)
  {
    delete m_links[i];
  }
#endif

#ifdef USE_LED_STRIP
  delete m_ledStrip;
#endif
}

void HalImpl::init()
{
  m_display.init();

#ifdef USE_WIRED_BUTTONS
  for(int i=0; i<NUM_WIRED_BUTTONS; i++)
  {
    m_wiredButtons[i].init(wiredButtonPins[i], INPUT);
  }
#endif

#ifdef USE_BUTTON_LEDS
  for(int i=0; i<NUM_BUTTON_LEDS; i++)
  {
    pinMode(buttonLedPins[i], OUTPUT);
  }
#endif

#ifdef USE_SIGNAL_LED
  pinMode(SIGNAL_LED_PIN, OUTPUT);
#endif

#ifdef USE_LED_STRIP
  m_ledStrip->begin();
  m_ledStrip->setBrightness(LED_STRIP_BRIGHNTESS);
  m_ledStrip->clear();
  m_ledStrip->show();
#endif

#ifdef USE_UART_LINKS
  for(int i=0; i<NUM_UART_LINKS; i++)
  {
    pinMode(uartRxPins[i], INPUT_PULLUP);
    uartSerials[i]->begin(9600, SERIAL_8N1, uartRxPins[i], uartTxPins[i]);
    m_links[i] = new link::ArduinoUartLink(uartSerials[i]);
  }
#endif

#ifdef USE_WIRELESS_LINK
  WirelessLink* wirelessLink = new WirelessLink();
  wirelessLink->init();
  m_links[NUM_LINKS - 1] = wirelessLink; // wireless link is always last
  loadWirelessButtonsData();
#endif

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

#ifdef USE_BUTTON_LEDS
    for(int i=0; i<NUM_BUTTON_LEDS; i++)
    {
      if(m_blinkingLeds[i])
      {
        digitalWrite(buttonLedPins[i], m_blinkState);
      }
    }
#endif

#ifdef USE_LED_STRIP 
    if(m_blinkState)
    {
      setLedStripColor(m_blinkingStripColor);
    }
    else
    {
      m_ledStrip->clear();
      m_ledStrip->show();
    }
#endif
  }

#ifdef USE_WIRED_BUTTONS
  for(int i=0; i<NUM_WIRED_BUTTONS; i++)
  {
    m_wiredButtons[i].tick();
  }
#endif

  if(m_soundTimer.tick(*this))
  {
    ledcDetach(BUZZER_PIN);
  }

#ifdef USE_LINKS
  for (int i=0; i<NUM_LINKS; i++)
  {
    m_links[i]->tick();
  }
#endif
}

ButtonState HalImpl::getButtonState()
{
  ButtonState s = m_display.syncTouchscreen();

#ifdef USE_WIRED_BUTTONS
  for(int i=0; i<NUM_WIRED_BUTTONS; i++)
  {
    if(m_wiredButtons[i].press())
    {
      s.player = i;
      break;
    }
  }
#endif

  if(s.player < 0)
  {
#ifdef USE_LINKS
    for (int i=0; i<NUM_LINKS; i++)
    {
      if(m_links[i]->getCommand() == link::Command::ButtonPressed)
      {
        s.player = m_links[i]->getData() + NUM_WIRED_BUTTONS + link::Link::maxPlayers * i;
        break;
      }
    }
#endif
  }

  return s;
}

// return link number and player number in link, if link = -1, player is local
void HalImpl::convertPlayerNumber(int player, int& outPlayerNumber, int& outLinkNumber)
{
  if (player < 0)
  {
    outLinkNumber = -1;
    outPlayerNumber = -1; // incorrect player
    return;
  }

  if(player < NUM_WIRED_BUTTONS) // local player
  {
    outLinkNumber = -1; // local player
    outPlayerNumber = player;
    return;
  }
  
  // remote player
  player -= NUM_WIRED_BUTTONS;

  if(player >= link::Link::maxPlayers * NUM_LINKS) // incorrect player number (link limit)
  {
    outLinkNumber = -1;
    outPlayerNumber = -1;  // incorrect player
    return;
  }

  outLinkNumber = player / link::Link::maxPlayers;
  outPlayerNumber = player % link::Link::maxPlayers;
}

void HalImpl::correctPressSignal(int player)
{
  int linkNumber, playerNumber;
  convertPlayerNumber(player, playerNumber, linkNumber);

  if (playerNumber < 0) // incorrect player
  {
    return;
  }

#ifdef USE_LINKS
  for (int i=0; i<NUM_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::DisplayCorrectPressSignal, playerNumber);
  }
#endif

#ifdef USE_LED_STRIP
  setLedStripColor(colorFromPlayerNumber(playerNumber));
#endif

  if(linkNumber < 0) // localPlayer
  {
#ifdef USE_BUTTON_LEDS
    digitalWrite(buttonLedPins[playerNumber], 1);
#endif
    return;
  }

#ifdef USE_LINKS
  sendLinkCommand(linkNumber, true, link::Command::CorrectPressSignal, playerNumber);
#endif
}

void HalImpl::falstartPressSignal(int player)
{
  int linkNumber, playerNumber;
  convertPlayerNumber(player, playerNumber, linkNumber);

  if (playerNumber < 0) // incorrect player
  {
    return;
  }

#ifdef USE_LINKS
  for (int i=0; i<NUM_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::DisplayFalstartPressSignal, playerNumber);
  }
#endif

#ifdef USE_LED_STRIP
  blinkLedStrip(playerNumber);
#endif

  if(linkNumber < 0) // localPlayer
  {
#ifdef USE_BUTTON_LEDS
    blinkLed(playerNumber);
#endif
    return;
  }

#ifdef USE_LINKS
  sendLinkCommand(linkNumber, true, link::Command::FalstartPressSignal, playerNumber);
#endif
}

void HalImpl::pendingPressSignal(int player)
{
  int linkNumber, playerNumber;
  convertPlayerNumber(player, playerNumber, linkNumber);

  if (playerNumber < 0) // incorrect player
  {
    return;
  }

  if(linkNumber < 0) // localPlayer
  {
#ifdef USE_BUTTON_LEDS
    blinkLed(playerNumber);
#endif
    return;
  }

#ifdef USE_LINKS
  sendLinkCommand(linkNumber, true, link::Command::PendingPressSignal, playerNumber);
#endif
}

void HalImpl::gameStartSignal()
{
  if(m_signalLightEnabled)
  {
#ifdef USE_SIGNAL_LED
    digitalWrite(SIGNAL_LED_PIN, 1);
#endif

#ifdef USE_LED_STRIP
    setLedStripColor({255, 255, 255}); // white
#endif
  }

#ifdef USE_LINKS
  for (int i=0; i<NUM_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::GameStartSignal);
  }
#endif
}

void HalImpl::clearSignals()
{
#ifdef USE_SIGNAL_LED
  digitalWrite(SIGNAL_LED_PIN, 0);
#endif

#ifdef USE_LED_STRIP
    m_ledStrip->clear();
    m_ledStrip->show();
#endif

#ifdef USE_BUTTON_LEDS
  for(int i=0; i<NUM_BUTTON_LEDS; i++)
  {
    digitalWrite(buttonLedPins[i], 0);
    m_blinkingLeds[i] = false;
  }
#endif

  m_blinkTimer.stop();

#ifdef USE_LINKS
  for (int i=0; i<NUM_LINKS; i++)
  {
    sendLinkCommand(i, true, link::Command::Clear);
  }
#endif
}

void HalImpl::setSignalLightEnabled(bool enabled)
{
  m_signalLightEnabled = enabled;
}

#ifdef USE_BUTTON_LEDS
void HalImpl::blinkLed(int player)
{
  startBlinkTimer();
  digitalWrite(buttonLedPins[player], m_blinkState);
  m_blinkingLeds[player] = true;
}
#endif

#ifdef USE_LED_STRIP
void HalImpl::blinkLedStrip(int player)
{
  startBlinkTimer();
  m_blinkingStripColor = colorFromPlayerNumber(player);
  setLedStripColor(m_blinkingStripColor);
}
#endif

void HalImpl::startBlinkTimer()
{
  if(!m_blinkTimer.isStarted())
  {
    m_blinkState = 1;
    m_blinkTimer.start(*this);
  }
}

void HalImpl::sound(HalSound soundType)
{
  if(m_soundMode == HalSoundMode::Disabled || soundType == HalSound::None)
  {
    ledcDetach(BUZZER_PIN);
    return;
  }

  sound(toneFromSound(soundType), durationFromSound(soundType));
}

void HalImpl::sound(unsigned int frequency, unsigned int duration)
{
  m_soundTimer.setTime(duration);
  m_soundTimer.start(*this);
  ledcAttach(BUZZER_PIN, 50, 10);
  ledcWriteTone(BUZZER_PIN, frequency);
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
  
#ifdef USE_WIRELESS_LINK
  else if(info.type == DisplayInfoWireless)
  {
    DisplayState s;
    s.mode = DisplayMode::Wireless;
    s.wireless = *((WirelessDisplayInfo*)info.data);
    m_display.sync(s);
  }
#endif
}

unsigned long HalImpl::getTimeMillis()
{
  return millis();
}

#ifdef USE_UART_LINKS
void HalImpl::setUartLinkVersion(vgs::link::UartLinkVersion version)
{
  for(int i=0; i<NUM_UART_LINKS; i++)
  {
    delete m_links[i];
    m_links[i] = new link::ArduinoUartLink(uartSerials[i], version);
  }
}
#endif

#ifdef USE_WIRELESS_LINK

WirelessLink& HalImpl::getWirelessLink()
{
  return *(WirelessLink*)m_links[NUM_LINKS - 1];
}

constexpr const char* wirelessNamespaceKey = "wireless_link";
constexpr const char* numWirelessButtonsKey = "num_buttons";
constexpr const char* wirelessButtonsDataKey = "buttons_data";

void HalImpl::loadWirelessButtonsData()
{
  WirelessLink& link = getWirelessLink();
  Preferences& preferences = getPreferences();
  preferences.begin(wirelessNamespaceKey, true);

  if(!preferences.isKey(numWirelessButtonsKey) || !preferences.isKey(wirelessButtonsDataKey))
  {
    preferences.end();
    return;
  }

  int numButtons = preferences.getInt(numWirelessButtonsKey);
  uint8_t buttonsData[numButtons * 6];
  preferences.getBytes(wirelessButtonsDataKey, buttonsData, numButtons * 6);

  link.setButtonsData(numButtons, buttonsData);
  preferences.end();
}

void HalImpl::saveWirelessButtonsData()
{
  WirelessLink& link = getWirelessLink();
  Preferences& preferences = getPreferences();

  preferences.begin(wirelessNamespaceKey, false);

  int numButtons = link.getNumButtons();
  uint8_t buttonsData[numButtons * 6];
  link.getButtonsData(buttonsData);

  preferences.putInt(numWirelessButtonsKey, numButtons);
  preferences.putBytes(wirelessButtonsDataKey, buttonsData, numButtons * 6);

  preferences.end();
}

#endif // #ifdef USE_WIRELESS_LINK

Preferences& HalImpl::getPreferences()
{
  return m_preferences;
}

#ifdef USE_LINKS
void HalImpl::sendLinkCommand(int linkNumber, bool useLink, vgs::link::Command command, unsigned int data)
{
  if(!useLink || linkNumber >= NUM_LINKS)
  {
    return;
  }

  m_links[linkNumber]->send(command, data);
}
#endif // #ifdef USE_LINKS

#ifdef USE_LED_STRIP
void HalImpl::setLedStripColor(const vgs::Color& color)
{
  uint32_t colorEncoded = m_ledStrip->Color(color.r, color.g, color.b);

  for (int i = 0; i < m_ledStrip->numPixels(); i++)
  {
    m_ledStrip->setPixelColor(i, colorEncoded);
  }

  m_ledStrip->show();
}
#endif // #ifdef USE_LED_STRIP
