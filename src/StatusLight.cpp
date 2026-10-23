#include "StatusLight.h"
#include <Arduino.h>

StatusLight::StatusLight(int redPin, int greenPin, int bluePin, CommandHandler *commandHandler)
    : Actuator(redPin, commandHandler), red(redPin), green(greenPin), blue(bluePin) {}

void StatusLight::begin() {
  red.begin();
  green.begin();
  blue.begin();
}

void StatusLight::setColor(bool r, bool g, bool b) {
  red.handle(r ? Led::TURN_ON_COMMAND : Led::TURN_OFF_COMMAND);
  green.handle(g ? Led::TURN_ON_COMMAND : Led::TURN_OFF_COMMAND);
  blue.handle(b ? Led::TURN_ON_COMMAND : Led::TURN_OFF_COMMAND);
}

void StatusLight::blink(int times, bool r, bool g, bool b, int onMs, int offMs) {
  for (int i = 0; i < times; i++) {
    setColor(r, g, b);
    delay(onMs);
    setColor(false, false, false);
    delay(offMs);
  }
}

void StatusLight::handle(Command command) {
  switch (command.id) {
    case SHOW_NORMAL_COMMAND_ID: blink(1, false, true, false); break;             // green, 1 blink
    case SHOW_WATCH_COMMAND_ID: blink(1, true, true, false); break;               // amber, 1 blink
    case SHOW_WARNING_COMMAND_ID: blink(2, true, true, false); break;             // orange, 2 blinks
    case SHOW_CRITICAL_COMMAND_ID: blink(3, true, false, false); break;           // red, 3 blinks
    case SHOW_OFFLINE_COMMAND_ID: blink(1, true, true, true, 600, 200); break;    // white, long blink
    case SHOW_PAIRING_COMMAND_ID: blink(10, false, false, true, 100, 100); break; // blue, fast
    case SHOW_STARTUP_COMMAND_ID: blink(1, true, true, true); break;              // white, 1 blink
    case SHOW_ERROR_COMMAND_ID: blink(5, true, false, true, 80, 80); break;       // magenta, fast
  }
  Actuator::handle(command);
}
