#pragma once

#include "Hal.h"
#include "Timer.h"

#include "Display.h"

class HalImpl : public Hal
{
public:
  HalImpl();
  ~HalImpl();

  void init();

  //main functions
  void tick() override;

    //buttons
  const ButtonState& getButtonState() override;

  //leds
  void playerLedOn(int player) override;
  void playerLedBlink(int player) override;
  void signalLedOn() override;
  void ledsOff() override;

  //sound
  void sound(int frequency, int duration) override;

  //display
  void updateDisplay(const GameDisplayInfo& info) override;
  void updateDisplay(const SettingsDisplayInfo& info) override;
  void updateDisplay(const CustomDisplayInfo& info) override;

  //time
  unsigned long getTimeMillis() override;

private:
  DisplayState m_displayState;
  Display m_display;

  Button m_playerButtons[NUM_PLAYERS];

  Timer m_blinkTimer;
  bool m_blinkState = 0;
  bool m_blinkingLeds[NUM_PLAYERS];
};
