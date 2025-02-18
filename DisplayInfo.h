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
  const char* mode = "";
  GameState state = GameState::IDLE;
  unsigned long time = 0;
  int player = -1;
  const char* custom = "";

  GameDisplayInfo& operator=(const GameDisplayInfo& right)
  {
    if (this == &right) 
    {
      return *this;
    }
    name = right.name;
    mode = right.mode;
    state = right.state;
    time = right.time;
    player = right.player;
    custom = right.custom;
    return *this;
  }
} GameDisplayInfo;

typedef struct SettingsDisplayInfo
{
  const char* setting_name = "";
  const char* setting_value = "";
  const char* custom = "";

  SettingsDisplayInfo& operator=(const SettingsDisplayInfo& right)
  {
    if (this == &right) 
    {
      return *this;
    }
    setting_name = right.setting_name;
    setting_value = right.setting_value;
    custom = right.custom;
    return *this;
  }
} SettingsDisplayInfo;

typedef struct CustomDisplayInfo
{
  const char* name = "";
  unsigned long custom_size = 0;
  const void* custom_data = nullptr;

  CustomDisplayInfo& operator=(CustomDisplayInfo& right)
  {
    if (this == &right) 
    {
      return *this;
    }
    name = right.name;
    custom_size = right.custom_size;
    custom_data = right.custom_data;
    return *this;
  }
} CustomDisplayInfo;
