#include "FlushPump.h"
#include <Arduino.h>

const Command FlushPump::START_FLUSH_COMMAND = Command(START_FLUSH_COMMAND_ID);
const Command FlushPump::STOP_FLUSH_COMMAND = Command(STOP_FLUSH_COMMAND_ID);

FlushPump::FlushPump(int pin, bool activeLow, unsigned long durationMs, CommandHandler *commandHandler)
    : Actuator(pin, commandHandler), activeLow(activeLow), durationMs(durationMs) {}

void FlushPump::drive(bool on) { digitalWrite(pin, (on != activeLow) ? HIGH : LOW); }

void FlushPump::begin() {
  // Write the OFF level before turning the pin into an output so the relay does not click on boot.
  digitalWrite(pin, activeLow ? HIGH : LOW);
  pinMode(pin, OUTPUT);
  drive(false);
}

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
