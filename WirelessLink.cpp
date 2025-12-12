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
    for(int i=0; i<m_numPlayers; i++)
    {
      m_interface->send(m_players[i], LINK_WIRELESS_HEADER_COMMAND_V2, LINK_CLEAR);
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
      m_interface->send(m_players[data], LINK_WIRELESS_HEADER_COMMAND_V2, (uint8_t)playerCommandCodes[i] | ((uint8_t)data & 0x0F));
    }
  }
}

void WirelessLink::handleEspNowMessage(const uint8_t* address, uint8_t header, uint8_t data)
{
  switch(header)
  {
    case LINK_WIRELESS_HEADER_COMMAND_V2:
      processCommand(address, data);
      break;
    case LINK_WIRELESS_HEADER_PING_REQUEST:
      processPingRequest(address, data);
      break;
    case LINK_WIRELESS_HEADER_PAIRING_REQUEST:
      processPairingRequest(address, data);
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

void WirelessLink::processPingRequest(const uint8_t* address, uint8_t data)
{
  m_interface->send(address, LINK_WIRELESS_HEADER_PING_RESPONSE, data);
}

void WirelessLink::processPairingRequest(const uint8_t* address, uint8_t data)
{

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

