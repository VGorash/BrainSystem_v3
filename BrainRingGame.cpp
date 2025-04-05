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

void BrainRingGame::processCountdown(Hal* hal, GameDisplayInfo& info)
{
  if(m_gameTimer.tick(hal))
  {
    m_secondsLeft--;
    m_displayDirty = true;

    if(m_secondsLeft == BRAIN_RING_SIGNAL_TIME)
    {
      hal->sound(HalSound::Signal);
    }

    if(m_secondsLeft <= BRAIN_RING_TICKS_TIME)
    {
      hal->sound(HalSound::Tick);
    }
  }
  info.gameTime = m_secondsLeft;

  if(m_secondsLeft <= 0)
  {
    hal->sound(HalSound::End);
    reset(hal, info);
    m_delayTimer.start(hal);
    return;
  }

  Game::processCountdown(hal, info);
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

App* BrainRingGame::getCustomApp()
{
  return new Game(false);
}

