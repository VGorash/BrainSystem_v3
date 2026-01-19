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
    for(int i=0; i<m_numButtons; i++)
    {
      m_interface->send(m_buttons[i], LINK_WIRELESS_HEADER_COMMAND_V2, code);
    }
    return;
  }

  if(data < m_numButtons)
  {
    m_interface->send(m_buttons[data], LINK_WIRELESS_HEADER_COMMAND_V2, code);
  }
}

void WirelessLink::setPairingEnable(bool enable)
{
  m_pairingEnabled = enable;
}

int WirelessLink::getNumButtons()
{
  return m_numButtons;
}

void WirelessLink::getButtonsData(uint8_t* dest)
{
  for(int i=0; i<m_numButtons; i++)
  {
    memcpy(dest + i * 6, m_buttons[i], 6);
  }
}

void WirelessLink::setButtonsData(int numButtons, uint8_t* data)
{
  if(numButtons > Link::maxPlayers)
  {
    numButtons = Link::maxPlayers;
  }

  for(int i=0; i<numButtons; i++)
  {
    memcpy(m_buttons[i], data + i * 6, 6);
  }

  m_numButtons = numButtons;
}

void WirelessLink::clearButtons()
{
  m_numButtons = 0;
}

void WirelessLink::onCommandV2(const uint8_t* address, uint8_t data)
{
  if(m_dirty || findButton(address) == -1)
  {
    return;
  }

  m_command = Command::None;
  m_data = 0;

  m_dirty = codeToCommand(data, m_command, m_data);
}

void WirelessLink::onPairingRequest(const uint8_t* address, uint8_t data)
{
  if(!m_pairingEnabled)
  {
    return;
  }

  if(data == LINK_WIRELESS_DEVICE_BUTTON)
  {
    if(findButton(address) == -1)
    {
      addButton(address);
    }

    m_interface->send(address, LINK_WIRELESS_HEADER_PAIRING_RESPONSE, findButton(address));
  }
}

void WirelessLink::onBatteryInfo(const uint8_t* address, uint8_t data)
{
  if(findButton(address) != -1) // info from button
  {
    //Serial.println(String("Button ") + String(findButton(address)) + ": battery is " + String(data) + String("%"));
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

int WirelessLink::findButton(const uint8_t* address)
{
  int button = -1;

  for(int i=0; i<m_numButtons; i++)
  {
    if(checkAddressEqual(address, m_buttons[i]))
    {
      button = i;
      break;
    }
  }

  return button;
}

void WirelessLink::addButton(const uint8_t* address)
{
  memcpy(m_buttons[m_numButtons++], address, 6);
}

