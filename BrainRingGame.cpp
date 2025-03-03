#include "BrainRingGame.h"

BrainRingGame::BrainRingGame(bool falstartEnabled) : JeopardyGame(falstartEnabled)
{

}

void BrainRingGame::processPress(Hal* hal, GameDisplayInfo& info)
{
  ButtonState buttonState = hal->getButtonState();

  if(buttonState.start)
  {
    hal->ledsOff();
    start(hal, info);
    return;
  }

  Game::processPress(hal, info);
}

void BrainRingGame::start(Hal* hal, GameDisplayInfo& info)
{
  m_secondsLeft = m_state == GameState::PRESS ? BRAIN_RING_WRONG_ANSWER_TIME : BRAIN_RING_TIME;

  info.gameTime = m_secondsLeft;
  m_gameTimer.start(hal);

  Game::start(hal, info);
}

const char* BrainRingGame::getName()
{
  return "БРЕЙН-РИНГ";
}

App* BrainRingGame::initStatic()
{
  return new BrainRingGame(true);
}
