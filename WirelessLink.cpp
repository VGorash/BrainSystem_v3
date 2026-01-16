#include "WirelessLink.h"

#include "src/Link/Codes.h"

using namespace vgs::link;

WirelessLink::WirelessLink()
{

}

void WirelessLink::init()
{
  m_interface = EspNowInterface::getInstance();
  m_interface->setHandler(this);
  WiFi.setSleep(false);
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

void WirelessLink::send(Command command, unsigned int data)
{
  unsigned char code;

  if(!commandToCode(command, data, code))
  {
    return;
  }

  if(command == Command::Clear) // broadcast
  {
    for(int i=0; i<m_numPlayers; i++)
    {
      m_interface->send(m_players[i], LINK_WIRELESS_HEADER_COMMAND_V2, code);
    }
    return;
  }

  if(data >= m_numPlayers)
  {
    return;
  }

  m_interface->send(m_players[data], LINK_WIRELESS_HEADER_COMMAND_V2, code);
}

void WirelessLink::onCommandV2(const uint8_t* address, uint8_t data)
{
  if(m_dirty)
  {
    return;
  }

  if(findPlayer(address) == -1)
  {
    addPlayer(address);
  }

  m_command = Command::None;
  m_data = 0;

  m_dirty = codeToCommand(data, m_command, m_data);
}

void WirelessLink::onPingRequest(const uint8_t* address, uint8_t data)
{
  m_interface->send(address, LINK_WIRELESS_HEADER_PING_RESPONSE, data);
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
  memcpy(m_players[m_numPlayers++], address, 6);
}

