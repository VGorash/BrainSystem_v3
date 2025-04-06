#include "JeopardyGame.h"

#define JEOPARDY_GAME_TIME 7

using namespace vgs;

JeopardyGame::JeopardyGame(bool falstartEnabled) : Game(falstartEnabled)
{
  m_gameTimer.setPeriodMode(true);
}

void JeopardyGame::processCountdown(Hal* hal, GameDisplayInfo& info)
{
  if(m_gameTimer.tick(hal))
  {
    m_secondsLeft--;
    m_displayDirty = true;
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

void JeopardyGame::start(Hal* hal, GameDisplayInfo& info)
{
  m_secondsLeft = JEOPARDY_GAME_TIME;
  info.gameTime = m_secondsLeft;
  m_gameTimer.start(hal);

  Game::start(hal, info);
}

void JeopardyGame::reset(Hal* hal, GameDisplayInfo& info)
{
  m_gameTimer.stop();

  Game::reset(hal, info);
}

void JeopardyGame::press(Hal* hal, GameDisplayInfo& info, int player)
{
  m_gameTimer.stop();

  Game::press(hal, info, player);
}

const char* JeopardyGame::getName()
{
  return "СВОЯ ИГРА";
}
