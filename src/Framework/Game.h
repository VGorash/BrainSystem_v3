#ifndef GAME_H
#define GAME_H

#include "Core/App.h"

#include "Timer.h"

namespace vgs
{

class Game : public App
{
public:
  Game(bool falstartEnabled);

  void init(Hal* hal) override;
  void tick(Hal* hal) override;

  AppChangeType appChangeNeeded() override;
  App* getCustomApp() override;

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
  GameState m_state = GameState::Idle;
  bool m_displayDirty = true;

  bool m_falstartEnabled;

  Timer m_delayTimer;

  AppChangeType m_changeNeeded = AppChangeType::None;
};

}

#endif
