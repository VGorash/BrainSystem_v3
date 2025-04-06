#ifndef JEOPARDY_GAME_H
#define JEOPARDY_GAME_H

#include "Game.h"

namespace vgs
{

class JeopardyGame : public Game
{
public:
  JeopardyGame(bool falstartEnabled);

protected:
  void processCountdown(Hal* hal, GameDisplayInfo& info) override;

  void start(Hal* hal, GameDisplayInfo& info) override;
  void reset(Hal* hal, GameDisplayInfo& info) override;
  void press(Hal* hal, GameDisplayInfo& info, int player) override;

  const char* getName() override;

protected:
  Timer m_gameTimer;
  int m_secondsLeft = 0;
};

} //namespace vgs

#endif
