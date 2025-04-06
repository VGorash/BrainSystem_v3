#ifndef BRAIN_RING_GAME_H
#define BRAIN_RING_GAME_H

#include "JeopardyGame.h"

namespace vgs
{

class BrainRingGame : public JeopardyGame
{
public:
  BrainRingGame(bool falstartEnabled);

protected:
  void processPress(Hal* hal, GameDisplayInfo& info) override;
  void processCountdown(Hal* hal, GameDisplayInfo& info) override;

  void start(Hal* hal, GameDisplayInfo& info) override;

  const char* getName() override;
  
};

} // namespace vgs

#endif
