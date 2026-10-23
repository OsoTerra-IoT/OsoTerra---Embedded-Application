#include "Led.h"
#include <Arduino.h>

const Command Led::TURN_ON_COMMAND = Command(TURN_ON_COMMAND_ID);
const Command Led::TURN_OFF_COMMAND = Command(TURN_OFF_COMMAND_ID);
const Command Led::TOGGLE_COMMAND = Command(TOGGLE_COMMAND_ID);

Led::Led(int pin, bool initialState, CommandHandler *commandHandler)
    : Actuator(pin, commandHandler), state(initialState) {}

void Led::begin() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, state ? HIGH : LOW);
}

void Led::handle(Command command) {
  if (command == TURN_ON_COMMAND) setState(true);
  else if (command == TURN_OFF_COMMAND) setState(false);
  else if (command == TOGGLE_COMMAND) setState(!state);
  Actuator::handle(command);
}

void Led::setState(bool newState) {
  state = newState;
  digitalWrite(pin, state ? HIGH : LOW);
}
