#ifndef BRAIN_RING_GAME_H
#define BRAIN_RING_GAME_H

#include "JeopardyGame.h"

#define BRAIN_RING_TIME 60
#define BRAIN_RING_WRONG_ANSWER_TIME 20

class BrainRingGame : public JeopardyGame
{
public:
  BrainRingGame(bool falstartEnabled);

  App* getCustomApp() override;

protected:
  void processPress(Hal* hal, GameDisplayInfo& info) override;

  void start(Hal* hal, GameDisplayInfo& info) override;

  const char* getName() override;
  
};

#endif
