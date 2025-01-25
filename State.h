#ifndef STATE_H
#define STATE_H

enum GameStatus
{
  IDLE,
  COUNTDOWN,
  PRESSED,
  FALSTART
};

enum GameCommand
{
  NONE,
  START,
  STOP
};

typedef struct 
{
  struct GameState
  {
    GameStatus status = GameStatus::IDLE;
    int secondsLeft = -1;
  };

  GameState game;
  GameCommand currentCommand = GameCommand::NONE;

}State;

#endif