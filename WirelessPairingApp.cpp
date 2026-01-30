#include "WirelessPairingApp.h"

#ifdef USE_WIRELESS_LINK

#include "HalImpl.h"


using namespace vgs;

WirelessPairingApp::WirelessPairingApp()
{
  
}

void WirelessPairingApp::init(IHal& hal)
{
  HalImpl* halImpl = (HalImpl*) &hal;

  halImpl->getWirelessLink().setPairingEnable(true);
  m_displayDirty = true;
  m_shouldClose = false;

  m_updateTimer.setTime(1000);
  m_updateTimer.setPeriodMode(true);
  m_updateTimer.start(hal);
}

void WirelessPairingApp::tick(IHal& hal)
{
  if(m_shouldClose)
  {
    return;
  }

  HalImpl* halImpl = (HalImpl*) &hal;
  WirelessLink& link = halImpl->getWirelessLink();
  
  ButtonState buttonState = hal.getButtonState();

  if(buttonState.menu)
  {
    link.setPairingEnable(false);
    halImpl->saveWirelessButtonsData();
    m_shouldClose = true;
    return;
  }

  if(buttonState.stop)
  {
    link.clearButtons();
    halImpl->saveWirelessButtonsData();
    m_displayDirty = true;
  }

  if(m_updateTimer.tick(hal))
  {
    m_displayDirty = true;
  }

  if(m_displayDirty)
  { 
    WirelessDisplayInfo info;
    info.num_buttons = link.getNumButtons();

    CustomDisplayInfo customInfo;
    customInfo.type = DisplayInfoWireless;
    customInfo.data = (void*)&info;
    
    hal.updateDisplay(customInfo);
    m_displayDirty = false;
  }
}

AppChangeType WirelessPairingApp::appChangeNeeded()
{
  if(m_shouldClose)
  {
    return AppChangeType::Menu;
  }

  return AppChangeType::None;
}

IApp* WirelessPairingApp::createCustomApp()
{
  return nullptr;
}

#endif // #ifdef USE_WIRELESS_LINK
