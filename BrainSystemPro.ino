#include "Display.h"

Display display;

void setup() {
  Serial.begin(9600);

  display.init();
  display.showGreeting();
}

void loop() {
  display.processTouchScreen();
}
