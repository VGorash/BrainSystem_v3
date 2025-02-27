#ifndef GAME_H
#define GAME_H

#include "App.h"
#include "Timer.h"

class Game : public App
{
public:
  Game(bool falstartEnabled);

  virtual void tick(Hal* hal);

  AppChangeType appChangeNeeded();
  App* getCustomApp();

  static App* initStatic();

protected:
  virtual void processIdle(Hal* hal, GameDisplayInfo& info);
  virtual void processCountdown(Hal* hal, GameDisplayInfo& info);
  virtual void processPress(Hal* hal, GameDisplayInfo& info);
  virtual void processFalstart(Hal* hal, GameDisplayInfo& info);

  virtual void start(Hal* hal, GameDisplayInfo& info);
  virtual void reset(Hal* hal, GameDisplayInfo& info);
  virtual void press(Hal* hal, GameDisplayInfo& info, int player);
  virtual void falstart(Hal* hal, GameDisplayInfo& info, int player);

  virtual const char* getName();

protected:
  GameState m_state = GameState::IDLE;
  bool m_displayDirty = true;

  bool m_falstartEnabled;

  Timer m_delayTimer;
};

#endif