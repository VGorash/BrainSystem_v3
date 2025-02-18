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
  SettingsDisplayInfo settings;

  // ---- OUTPUT ----
  ButtonState button_state;

} DisplayState;
