#ifndef HAL_IMPL_H
#define HAL_IMPL_H

#include "Hal.h"
#include "Timer.h"

#include "Display.h"

enum HalSoundMode
{
  Normal,
  Disabled
};

class HalImpl : public Hal
{
public:
  HalImpl();
  ~HalImpl();

  void init();
  void tick() override;

  //buttons
  ButtonState getButtonState() override;

  //leds
  void playerLedOn(int player) override;
  void playerLedBlink(int player) override;
  void signalLedOn() override;
  void ledsOff() override;
  void setSignalLightEnabled(bool enabled);

  //sound
  void sound(HalSound soundType) override;
  void sound(unsigned int frequency, unsigned int duration) override;
  void setSoundMode(HalSoundMode mode);

  //display
  void updateDisplay(const GameDisplayInfo& info) override;
  void updateDisplay(const SettingsDisplayInfo& info) override;
  void updateDisplay(const CustomDisplayInfo& info) override;

  //time
  unsigned long getTimeMillis() override;

  //settings
  void saveSettings(const Settings& settings) override;
  void loadSettings(Settings& settings) override;

private:  
  Display m_display;

  Button m_playerButtons[NUM_PLAYERS];

  Timer m_blinkTimer;
  Timer m_soundTimer;
  bool m_blinkState = 0;
  bool m_blinkingLeds[NUM_PLAYERS];

  HalSoundMode m_soundMode;
  bool m_signalLightEnabled;
};

#endif
