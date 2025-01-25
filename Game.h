#ifndef GAME_H
#define GAME_H

#include "State.h"

class Game
{
public:
  Game(){};

  void tick(State& state);
};

#endif