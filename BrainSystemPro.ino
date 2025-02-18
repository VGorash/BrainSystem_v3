#include "Engine.h"
#include "HalImpl.h"
#include "Game.h"

#include <EncButton.h>
 
HalImpl* hal;
Engine* engine;

void setup() {
  Serial.begin(9600);
  delay(100);
  //tone(18, 1200, 1000);

  hal = new HalImpl();
  hal->init();

  engine = new Engine(hal, Game::initStatic, nullptr);
}

void loop() {
  engine->tick();
}
