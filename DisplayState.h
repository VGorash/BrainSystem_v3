#ifndef DISPLAY_STATE_H
#define DISPLAY_STATE_H

#include "src/Framework/Core/Hal.h"
#include "src/Settings/Settings.h"

enum class DisplayMode
{
  Game,
  Settings,
  Wireless
};

struct SettingsDisplayInfo
{
  bool edit_mode;
  const vgs::settings::Settings* settings;
};

struct WirelessDisplayInfo
{
  int num_buttons;
};

typedef struct DisplayState
{
  // ---- INPUT ----
  DisplayMode mode;
  vgs::GameDisplayInfo game;
  SettingsDisplayInfo settings;
  WirelessDisplayInfo wireless;

  // ---- STATE ----
  bool dirty = true;
  bool modeChanged = false;

  // ---- OUTPUT ----
  vgs::ButtonState button_state;

} DisplayState;

#endif
