#pragma once

#include "Hal.h"

enum DisplayMode
{
  GAME,
  SETTINGS
};

typedef struct DisplayState
{
  // ---- INPUT ----
  DisplayMode mode;
  GameDisplayInfo game;
  CustomDisplayInfo custom;
  SettingsDisplayInfo settings;

  // ---- STATE ----
  bool dirty = true;
  bool modeChanged = false;

  // ---- OUTPUT ----
  ButtonState button_state;

} DisplayState;
