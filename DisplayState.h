#ifndef DISPLAY_STATE_H
#define DISPLAY_STATE_H

#include "Core/Hal.h"

enum class DisplayMode
{
  Game,
  Settings
};

typedef struct DisplayState
{
  // ---- INPUT ----
  DisplayMode mode;
  vgs::GameDisplayInfo game;
  vgs::CustomDisplayInfo custom;
  vgs::SettingsDisplayInfo settings;

  // ---- STATE ----
  bool dirty = true;
  bool modeChanged = false;

  // ---- OUTPUT ----
  vgs::ButtonState button_state;

} DisplayState;

#endif
