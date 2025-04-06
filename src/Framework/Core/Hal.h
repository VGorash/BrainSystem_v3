#ifndef HAL_H
#define HAL_H

#include "Settings.h"

namespace vgs
{

enum class HalSound
{
  Start,
  Press,
  Falstart,
  Tick,
  Signal,
  End,
  None
};

enum class GameState
{
  Idle,
  Countdown,
  Press,
  Falstart
};

typedef struct GameDisplayInfo
{
  const char* name = "";
  bool falstart_enabled = false;
  GameState state = GameState::Idle;
  int gameTime = -1;
  int pressTime = -1;
  int player = -1;
  const char* custom = "";
} GameDisplayInfo;

typedef struct SettingsDisplayInfo
{
  const Settings* settings = nullptr;
  bool edit_mode = true;
} SettingsDisplayInfo;

typedef struct CustomDisplayInfo
{
  const char* name = "";
  const void* data = nullptr;
} CustomDisplayInfo;

typedef struct ButtonState
{
  bool start = false;
  bool stop = false;
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

  //settings
  virtual void saveSettings(const Settings& settings) = 0;
  virtual void loadSettings(Settings& settings) = 0;

};

} // namespace vgs

#endif
