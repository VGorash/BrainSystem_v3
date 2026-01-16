#ifndef WIRELESS_LINK_H
#define WIRELESS_LINK_H

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

private:
  // EspNowHandler methods
  void onCommandV2(const uint8_t* address, uint8_t data) override;
  void onPingRequest(const uint8_t* address, uint8_t data) override;

  int findPlayer(const uint8_t* address);
  void addPlayer(const uint8_t* address);

private:
  vgs::link::EspNowInterface* m_interface = nullptr;
  vgs::link::Command m_command = vgs::link::Command::None;
  unsigned int m_data = 0;
  bool m_dirty;

  uint8_t m_players[6][vgs::link::Link::maxPlayers];
  int m_numPlayers = 0;
};

#endif