#pragma once

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
  const char* setting_name = "";
  const char* setting_value = "";
  const char* custom = "";
} SettingsDisplayInfo;

typedef struct CustomDisplayInfo
{
  const char* name = "";
  unsigned long custom_size = 0;
  const void* custom_data = nullptr;
} CustomDisplayInfo;
