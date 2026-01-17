#ifndef WIRELESS_PAIRING_APP_H
#define WIRELESS_PAIRING_APP_H

#include "src/Framework/Core/App.h"
#include "src/Framework/Timer.h"

namespace vgs
{

class WirelessPairingApp : public IApp
{
public:
  WirelessPairingApp();

  void init(IHal& hal) override;
  void tick(IHal& hal) override;

  AppChangeType appChangeNeeded() override;
  IApp* createCustomApp() override;

private:
  bool m_displayDirty = true;
  bool m_shouldClose = false;

  Timer m_updateTimer;
};

} // namespace vgs

#endif