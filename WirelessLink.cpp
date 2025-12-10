#include "WirelessLink.h"

#include "src/Link/Codes.h"

#include <WiFi.h>
#include <esp_wifi.h>

using namespace vgs::link;

static WirelessLink* currentInstance;

WirelessLink::WirelessLink()
{
  currentInstance = this;
}

void WirelessLink::init()
{
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(1);
  WiFi.setTxPower(WIFI_POWER_21dBm);
  WiFi.setSleep(false);
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);
  esp_now_init();
  esp_now_register_recv_cb([](const esp_now_recv_info_t *info, const uint8_t *data, int len){currentInstance->onDataRecv(info, data, len);});
}

void WirelessLink::tick()
{
  if(!m_dirty)
  {
    m_command = Command::None;
    m_data = 0;
  }
}

Command WirelessLink::getCommand()
{
  m_dirty = false;
  return m_command;
}

unsigned int WirelessLink::getData()
{
  return m_data;
}

constexpr int numPlayerCommands = 3;
constexpr char playerCommandCodes[numPlayerCommands] = 
{
  LINK_CORRECT_PRESS_SIGNAL,
  LINK_FALSTART_PRESS_SIGNAL,
  LINK_PENDING_PRESS_SIGNAL
};
constexpr Command playerCommands[numPlayerCommands] = 
{
  Command::CorrectPressSignal,
  Command::FalstartPressSignal, 
  Command::PendingPressSignal
};

void WirelessLink::send(Command command, unsigned int data)
{
  if(command == Command::Clear)
  {
    uint8_t sendData[2];
    sendData[0] = LINK_WIRELESS_HEADER_COMMAND_V2;
    sendData[1] = LINK_CLEAR;

    for(int i=0; i<m_numPlayers; i++)
    {
      esp_now_send(m_players[i], sendData, 2);
    }

    return;
  }

  for(int i=0; i<numPlayerCommands; i++)
  {
    if(command == playerCommands[i])
    {
      if(data >= m_numPlayers)
      {
        return;
      }

      uint8_t sendData[2];
      sendData[0] = LINK_WIRELESS_HEADER_COMMAND_V2;
      sendData[1] = (uint8_t)playerCommandCodes[i] | ((uint8_t)data & 0x0F);
      esp_now_send(m_players[data], sendData, 2);
    }
  }
}

void WirelessLink::onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
  if(len != 2)
  {
    return; // all correct Link packages have size 2
  }

  switch(data[0])
  {
    case LINK_WIRELESS_HEADER_COMMAND_V2:
      processCommand(info->src_addr, data[1]);
      break;
  }
}

void WirelessLink::processCommand(const uint8_t* address, uint8_t data)
{
  if(m_dirty)
  {
    return;
  }

  if(findPlayer(address) == -1)
  {
    addPlayer(address);
  }

  m_dirty = true;

  unsigned char command = data & 0xF0;
  unsigned char payload = data & 0x0F;

  if(command == LINK_BUTTON_PRESSED)
  {
    m_command = Command::ButtonPressed;
    m_data = (unsigned int)findPlayer(address);
  }
}

bool checkAddressEqual(const uint8_t* a, const uint8_t* b)
{
  bool equal = true;
  for(int i=0; i<6; i++)
  {
    if(a[i] != b[i])
    {
      equal = false;
      break;
    }
  }

  return equal;
}

int WirelessLink::findPlayer(const uint8_t* address)
{
  int player = -1;

  for(int i=0; i<m_numPlayers; i++)
  {
    if(checkAddressEqual(address, m_players[i]))
    {
      player = i;
      break;
    }
  }

  return player;
}

void WirelessLink::addPlayer(const uint8_t* address)
{
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, address, 6);
  peer.channel = 1;
  peer.encrypt = false;
  esp_now_add_peer(&peer);

  esp_now_rate_config_t config = {};
  config.phymode = WIFI_PHY_MODE_LR;
  config.rate = WIFI_PHY_RATE_LORA_250K;
  config.ersu = false;  
  config.dcm = false;
  esp_now_set_peer_rate_config(address, &config);

  memcpy(m_players[m_numPlayers++], address, 6);
}

