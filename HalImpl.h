#ifndef HAL_IMPL_H
#define HAL_IMPL_H

#include "hardware_config.h"
#include "Display.h"

#include <Preferences.h>

#include "src/Framework/Core/Hal.h"
#include "src/Framework/Timer.h"
#include "src/Framework/colors.h"

#ifdef USE_UART_LINKS
  #define USE_ARDUINO_UART_LINK
  #include "src/Link/ArduinoUartLink.h"
#endif

#ifdef USE_WIRELESS_LINK
  #include "WirelessLink.h"
#endif

enum class HalSoundMode
{
  Normal,
  Disabled
};

enum CustomDisplayInfoType
{
  DisplayInfoSettings = 0,
#ifdef USE_WIRELESS_LINK
  DisplayInfoWireless = 1
#endif
};

class HalImpl : public vgs::IHal
{
public:
  HalImpl();
  ~HalImpl();

  void init() override;
  void tick() override;

  //buttons
  vgs::ButtonState getButtonState() override;

  //leds
  void correctPressSignal(int player) override;
  void falstartPressSignal(int player) override;
  void pendingPressSignal(int player) override;
  void gameStartSignal() override;
  void clearSignals() override;
  void setSignalLightEnabled(bool enabled);

  //sound
  void sound(vgs::HalSound soundType) override;
  void sound(unsigned int frequency, unsigned int duration) override;
  void setSoundMode(HalSoundMode mode);

  //display
  void updateDisplay(const vgs::GameDisplayInfo& info) override;
  void updateDisplay(const vgs::CustomDisplayInfo& info) override;

  //time
  unsigned long getTimeMillis() override;

  //preferences
  Preferences& getPreferences();

  // link
#ifdef USE_UART_LINKS
  void setUartLinkVersion(vgs::link::UartLinkVersion);
#endif

#ifdef USE_WIRELESS_LINK
  WirelessLink& getWirelessLink();
  void loadWirelessButtonsData();
  void saveWirelessButtonsData();
#endif

  static void convertPlayerNumber(int player, int& outPlayerNumber, int& outLinkNumber);

private:
  void startBlinkTimer();

#ifdef USE_BUTTON_LEDS
  void blinkLed(int player);
#endif

#ifdef USE_LED_STRIP
  void blinkLedStrip(int player);
  void setLedStripColor(const vgs::Color& color);
#endif

#ifdef USE_LINKS
  void sendLinkCommand(int linkNumber, bool useLink, vgs::link::Command command, unsigned int data = 0);
#endif

private:  
  Display m_display;
  vgs::Timer m_blinkTimer;
  vgs::Timer m_soundTimer;
  bool m_blinkState = 0;
  bool m_signalLightEnabled;
  HalSoundMode m_soundMode;
  Preferences m_preferences;

#ifdef USE_WIRED_BUTTONS
  Button m_wiredButtons[NUM_WIRED_BUTTONS];
#endif

#ifdef USE_LED_STRIP
  Adafruit_NeoPixel* m_ledStrip;
  vgs::Color m_blinkingStripColor;
#endif

#ifdef USE_BUTTON_LEDS
  bool m_blinkingLeds[NUM_BUTTON_LEDS];
#endif

#ifdef USE_LINKS
  vgs::link::Link* m_links[NUM_LINKS];
#endif
};

#endif
