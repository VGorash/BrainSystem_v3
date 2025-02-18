#include "Game.h"

Game::Game()
{

}

void Game::tick(Hal* hal)
{
  GameDisplayInfo info;
  info.name = "";
  info.mode = "";

  switch(m_state)
  {
    case GameState::IDLE:
      processIdle(hal, info);
      break;
    case GameState::COUNTDOWN:
      processCountdown(hal, info);
      break;
    case GameState::PRESS:
      processPress(hal, info);
      break;
    case GameState::FALSTART:
      processFalstart(hal, info);
      break;
  }

  info.state = m_state;

  if(m_displayDirty)
  {
    hal->updateDisplay(info);
    m_displayDirty = false;
  }
}

void Game::processIdle(Hal* hal, GameDisplayInfo& info)
{
  const ButtonState& buttonState = hal->getButtonState();

  if(buttonState.start)
  {
    m_state = GameState::COUNTDOWN;
    m_displayDirty = true;
    hal->signalLedOn();
    return;
  }
  if(buttonState.player >= 0)
  {
    m_state = GameState::FALSTART;
    m_displayDirty = true;
    hal->ledsOff();
    hal->playerLedBlink(buttonState.player);
    info.player = buttonState.player;
    return;
  }
}

void Game::processCountdown(Hal* hal, GameDisplayInfo& info)
{
  const ButtonState& buttonState = hal->getButtonState();

  if(buttonState.stop)
  {
    m_state = GameState::IDLE;
    m_displayDirty = true;
    hal->ledsOff();
    return;
  }
  if(buttonState.player >= 0)
  {
    m_state = GameState::PRESS;
    m_displayDirty = true;
    hal->ledsOff();
    hal->playerLedOn(buttonState.player);
    info.player = buttonState.player;
    return;
  }
}

void Game::processPress(Hal* hal, GameDisplayInfo& info)
{
  const ButtonState& buttonState = hal->getButtonState();

  if(buttonState.stop)
  {
    m_state = GameState::IDLE;
    m_displayDirty = true;
    hal->ledsOff();
    return;
  }
}

void Game::processFalstart(Hal* hal, GameDisplayInfo& info)
{
  processPress(hal, info);
}

AppChangeType Game::appChangeNeeded()
{
  return AppChangeType::NONE;
}

App* Game::getCustomApp()
{
  return new Game();
}

App* Game::initStatic()
{
  return new Game();
}