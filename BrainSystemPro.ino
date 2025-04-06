#include "Engine.h"
#include "HalImpl.h"
#include "BrainRingGame.h"
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

void setup() {
  Serial.begin(9600);
  delay(100);

  EEPROM.begin(512);

  hal = new HalImpl();
  hal->init();

  engine = new Engine(hal, nullptr, initSettings);

}

void loop() {
  engine->tick();
}
