#ifndef HAL_H
#define HAL_H

#include "DisplayInfo.h"

enum HalSound
{
  Start,
  Press,
  Falstart,
  Tick,
  End,
  None
};

typedef struct ButtonState
{
  bool start = false;
  bool stop = false;
  bool left = false;
  bool right = false;
  bool up = false;
  bool down = false;
  bool enter = false;
  bool menu = false;
  int custom = -1;
  int player = -1;
} ButtonState;

class Hal
{
public:
  //main functions
  virtual void tick() = 0;

  //buttons
  virtual ButtonState getButtonState() = 0;

  //leds
  virtual void playerLedOn(int player) = 0;
  virtual void playerLedBlink(int player) = 0;
  virtual void signalLedOn() = 0;
  virtual void ledsOff();

  //sound
  virtual void sound(HalSound soundType) = 0;
  virtual void sound(unsigned int frequency, unsigned int duration) = 0;

  //display
  virtual void updateDisplay(const GameDisplayInfo& info) = 0;
  virtual void updateDisplay(const SettingsDisplayInfo& info) = 0;
  virtual void updateDisplay(const CustomDisplayInfo& info) = 0;

  //time
  virtual unsigned long getTimeMillis() = 0;

};

#endif