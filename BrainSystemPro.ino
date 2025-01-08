#include "Display.h"
#include "State.h"

State state;
Display display;

void setup() {
  Serial.begin(9600);
  delay(2000);

  display.init();
  display.showGreeting();
}

void loop() {
  display.sync(state);
}
