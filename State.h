#ifndef STATE_H
#define STATE_H

typedef struct 
{
  struct Game
  {
    bool started;
  };

  Game game;
}State;

#endif