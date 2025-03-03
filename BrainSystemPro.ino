#include "Engine.h"
#include "HalImpl.h"
#include "BrainRingGame.h"

#include <EncButton.h>
 
HalImpl* hal;
Engine* engine;

bool taskInit = false;

void setup() {
  Serial.begin(9600);
  delay(100);

  hal = new HalImpl();
  hal->init();

  engine = new Engine(hal, BrainRingGame::initStatic, nullptr);

}

void loop() {
  engine->tick();
}
