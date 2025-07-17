#ifndef HAL_IMPL_H
#define HAL_IMPL_H

#define USE_ARDUINO_UART_LINK

#include "src/Framework/Core/Hal.h"
#include "src/Framework/Timer.h"
#include "src/Link/ArduinoUartLink.h"

#include "Display.h"

#define NUM_UART_LINKS 3

enum class HalSoundMode
{
  Normal,
  Disabled
};

enum CustomDisplayInfoType
{
  DisplayInfoSettings = 0
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
  void playerLedOn(int player) override;
  void playerLedBlink(int player) override;
  void signalLedOn() override;
  void ledsOff() override;
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

  //settings
  void saveSettings(const vgs::settings::Settings& settings);
  void loadSettings(vgs::settings::Settings& settings);

  // link
  void setLinkVersion(vgs::link::UartLinkVersion);

private:
  void sendLinkCommand(int linkNumber, bool useLink, vgs::link::Command command, unsigned int data = 0);

private:  
  Display m_display;

  Button m_playerButtons[NUM_PLAYERS];

  vgs::Timer m_blinkTimer;
  vgs::Timer m_soundTimer;
  bool m_blinkState = 0;
  bool m_blinkingLeds[NUM_PLAYERS];

  HalSoundMode m_soundMode;
  bool m_signalLightEnabled;

  vgs::link::UartLink* m_uartLinks[NUM_UART_LINKS];
};

#endif
