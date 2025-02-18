#ifndef GAME_H
#define GAME_H

#include "App.h"

class Game : public App
{
public:
  Game();

  void tick(Hal* hal);

  AppChangeType appChangeNeeded();
  App* getCustomApp();

  static App* initStatic();

private:
  void processIdle(Hal* hal, GameDisplayInfo& info);
  void processCountdown(Hal* hal, GameDisplayInfo& info);
  void processPress(Hal* hal, GameDisplayInfo& info);
  void processFalstart(Hal* hal, GameDisplayInfo& info);

private:
  GameState m_state = GameState::IDLE;
  bool m_displayDirty = true;
};

#endif