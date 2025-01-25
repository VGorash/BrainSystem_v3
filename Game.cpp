#include "Game.h"

void Game::tick(State &state)
{
  if(state.currentCommand == GameCommand::START && state.game.status == GameStatus::IDLE)
  {
    state.game.status = GameStatus::COUNTDOWN;
  }
  if(state.currentCommand == GameCommand::STOP && state.game.status != GameStatus::IDLE)
  {
    state.game.status = GameStatus::IDLE;
  }
  state.currentCommand = GameCommand::NONE;
}