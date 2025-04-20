#include "src/Framework/Core/Engine.h"

#include "HalImpl.h"
#include "SettingsApp.h"

#include <EEPROM.h>

using namespace vgs;
 
HalImpl* hal;
Engine* engine;

bool taskInit = false;

App* initSettings()
{
  return new SettingsApp(true);
}

void setup()
{
  hal = new HalImpl();
  hal->init();

  engine = new Engine(hal, nullptr, initSettings);
}

void loop() {
  engine->tick();
}
