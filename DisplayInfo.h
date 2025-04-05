#ifndef DISPLAY_INFO_H
#define DISPLAY_INFO_H

#include "Settings.h"

enum GameState
{
  IDLE,
  COUNTDOWN,
  PRESS,
  FALSTART
};

typedef struct GameDisplayInfo
{
  const char* name = "";
  bool falstart_enabled = false;
  GameState state = GameState::IDLE;
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

#endif
