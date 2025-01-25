#include "Display.h"
#include "State.h"
#include "Game.h"

#include <EncButton.h>

State state;
Display display;

Game* game;

void setup() {
  Serial.begin(9600);
  delay(100);

  display.init();

  game = new Game();
}

void loop() {
  display.syncTouchscreen(state);
  game->tick(state);
  display.sync(state);
}
