#ifndef DISPLAY_STATE_H
#define DISPLAY_STATE_H

#include "src/Framework/Core/Hal.h"
#include "src/Settings/Settings.h"

#include "hardware_config.h"

enum class DisplayMode
{
  Game,
  Settings,
#ifdef USE_WIRELESS_LINK
  Wireless
#endif
};

struct SettingsDisplayInfo
{
  bool edit_mode;
  const vgs::settings::Settings* settings;
};

#ifdef USE_WIRELESS_LINK
struct WirelessDisplayInfo
{
  int num_buttons;
};
#endif

typedef struct DisplayState
{
  // ---- INPUT ----
  DisplayMode mode;
  vgs::GameDisplayInfo game;
  SettingsDisplayInfo settings;
#ifdef USE_WIRELESS_LINK
  WirelessDisplayInfo wireless;
#endif
#ifdef USE_BATTERY
  int batteryPercent;
#endif

  // ---- STATE ----
  bool dirty = true;
  bool modeChanged = false;

  // ---- OUTPUT ----
  vgs::ButtonState button_state;

} DisplayState;

#endif
