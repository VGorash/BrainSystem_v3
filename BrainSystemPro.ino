#include "Engine.h"
#include "HalImpl.h"
#include "JeopardyGame.h"

#include <EncButton.h>
 
HalImpl* hal;
Engine* engine;

bool taskInit = false;

void setup() {
  Serial.begin(9600);
  delay(100);

  hal = new HalImpl();
  hal->init();

  engine = new Engine(hal, JeopardyGame::initStatic, nullptr);

}

void loop() {
  engine->tick();
}
