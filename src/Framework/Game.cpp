#include "Game.h"

using namespace vgs;

Game::Game(bool falstartEnabled) : m_falstartEnabled(falstartEnabled)
{
  // do nothing
}

void Game::init(Hal* hal)
{
  // do nothing
}

void Game::tick(Hal* hal)
{
  m_delayTimer.tick(hal);

  if(m_delayTimer.isStarted())
  {
    return;
  }

  if(hal->getButtonState().menu)
  {
    m_changeNeeded = AppChangeType::Menu;
    return;
  }

  GameDisplayInfo info;

  switch(m_state)
  {
    case GameState::Idle:
      processIdle(hal, info);
      break;
    case GameState::Countdown:
      processCountdown(hal, info);
      break;
    case GameState::Press:
      processPress(hal, info);
      break;
    case GameState::Falstart:
      processFalstart(hal, info);
      break;
  }

  if(m_displayDirty)
  {
    info.name = getName();
    info.falstart_enabled = m_falstartEnabled;
    info.state = m_state;

    hal->updateDisplay(info);
    m_displayDirty = false;
  }
}

void Game::processIdle(Hal* hal, GameDisplayInfo& info)
{
  ButtonState buttonState = hal->getButtonState();

  if(buttonState.player >= 0)
  {
    if(m_falstartEnabled)
    {
      falstart(hal, info, buttonState.player);
    }
    else
    {
      press(hal, info, buttonState.player);
    }
    return;
  }
  if(buttonState.start)
  {
    start(hal, info);
    return;
  }
}

void Game::processCountdown(Hal* hal, GameDisplayInfo& info)
{
  ButtonState buttonState = hal->getButtonState();
  
  if(buttonState.player >= 0)
  {
    press(hal, info, buttonState.player);
    return;
  }
  if(buttonState.stop)
  {
    reset(hal, info);
    hal->sound(HalSound::None);
    return;
  }
}

void Game::processPress(Hal* hal, GameDisplayInfo& info)
{
  ButtonState buttonState = hal->getButtonState();

  if(buttonState.stop)
  {
    reset(hal, info);
    return;
  }
}

void Game::processFalstart(Hal* hal, GameDisplayInfo& info)
{
  processPress(hal, info);
}

void Game::start(Hal* hal, GameDisplayInfo& info)
{
  m_state = GameState::Countdown;
  m_displayDirty = true;
  hal->signalLedOn();
  hal->sound(HalSound::Start);
}

void Game::reset(Hal* hal, GameDisplayInfo& info)
{
  m_state = GameState::Idle;
  m_displayDirty = true;
  hal->ledsOff();
}

void Game::press(Hal* hal, GameDisplayInfo& info, int player)
{
  m_state = GameState::Press;
  m_displayDirty = true;
  hal->ledsOff();
  hal->playerLedOn(player);
  hal->sound(HalSound::Press);
  info.player = player;
  m_delayTimer.start(hal);
}

void Game::falstart(Hal* hal, GameDisplayInfo& info, int player)
{
  m_state = GameState::Falstart;
  m_displayDirty = true;
  hal->ledsOff();
  hal->playerLedBlink(player);
  hal->sound(HalSound::Falstart);
  info.player = player;
  m_delayTimer.start(hal);
}

const char* Game::getName()
{
  return "БЕЗ ОТСЧЕТА";
}

AppChangeType Game::appChangeNeeded()
{
  return m_changeNeeded;
}

App* Game::getCustomApp()
{
  return nullptr;
}
