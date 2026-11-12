#include "FlushPump.h"
#include <Arduino.h>

const Command FlushPump::START_FLUSH_COMMAND = Command(START_FLUSH_COMMAND_ID);
const Command FlushPump::STOP_FLUSH_COMMAND = Command(STOP_FLUSH_COMMAND_ID);

FlushPump::FlushPump(int pin, bool activeLow, unsigned long durationMs, CommandHandler *commandHandler)
    : Actuator(pin, commandHandler), activeLow(activeLow), durationMs(durationMs) {}

// A 5 V active-low module still switches on with 3.3 V on IN, so the pin floats to turn it off and goes LOW to turn it on.
void FlushPump::drive(bool on) {
  if (activeLow) {
    if (on) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    } else {
      pinMode(pin, INPUT);
    }
    return;
  }
  pinMode(pin, OUTPUT);
  digitalWrite(pin, on ? HIGH : LOW);
}

void FlushPump::begin() { drive(false); }

void FlushPump::handle(Command command) {
  if (command == START_FLUSH_COMMAND && !running) {
    running = true;
    startedAt = millis();
    drive(true);
  } else if (command == STOP_FLUSH_COMMAND && running) {
    running = false;
    drive(false);
  }
  Actuator::handle(command);
}

bool FlushPump::update(unsigned long now) {
  if (!running || now - startedAt < durationMs) return false;
  handle(STOP_FLUSH_COMMAND);
  return true;
}
