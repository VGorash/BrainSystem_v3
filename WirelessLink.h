#ifndef WIRELESS_LINK_H
#define WIRELESS_LINK_H

#include "hardware_config.h"

#ifdef USE_WIRELESS_LINK

#define USE_ESP_NOW_LINK
#include "src/Link/EspNowInterface.h"
#include "src/Link/Link.h"

class WirelessLink : public vgs::link::Link, vgs::link::EspNowHandler
{
public:
  WirelessLink();

  void init();

  // Link methods
  void tick() override;
  vgs::link::Command getCommand() override;
  unsigned int getData() override;
  void send(vgs::link::Command command, unsigned int data = 0) override;

  // Custom methods
  void setPairingEnable(bool enable);
  int getNumButtons();
  void getButtonsData(uint8_t* dest);
  void setButtonsData(int numButtons, uint8_t* data);
  void clearButtons();

private:
  // EspNowHandler methods
  void onCommandV2(const uint8_t* address, uint8_t data) override;
  void onPairingRequest(const uint8_t* address, uint8_t data) override;
  void onBatteryInfo(const uint8_t* address, uint8_t data) override;

  int findButton(const uint8_t* address);
  int addButton(const uint8_t* address);

private:
  vgs::link::EspNowInterface* m_interface = nullptr;
  vgs::link::Command m_command = vgs::link::Command::None;
  unsigned int m_data = 0;
  bool m_dirty = false;
  bool m_pairingEnabled = false;

  uint8_t m_buttons[vgs::link::Link::maxPlayers][6];
  int m_numButtons = 0;
};

#endif // #ifdef USE_WIRELESS_LINK

#endif