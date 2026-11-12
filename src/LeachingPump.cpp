#include "LeachingPump.h"
#include <Arduino.h>
#include "Config.h"

const Command LeachingPump::START_COMMAND = Command(START_COMMAND_ID);
const Command LeachingPump::STOP_COMMAND = Command(STOP_COMMAND_ID);

LeachingPump::LeachingPump(int pin, float criticalDsM, CommandHandler *commandHandler)
    : Actuator(pin, commandHandler), criticalDsM(criticalDsM) {}

void LeachingPump::begin() {
  setRelay(false);
}

void LeachingPump::setRelay(bool on) {
#if LEACHING_RELAY_ACTIVE_LOW
  // A 5 V active-low module still switches on with 3.3 V on its input, so the pin floats to turn it off.
  if (on) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  } else {
    pinMode(pin, INPUT);
  }
#else
  pinMode(pin, OUTPUT);
  digitalWrite(pin, on ? HIGH : LOW);
#endif
}

void LeachingPump::enter(State newState) {
  state = newState;
  stateSince = millis();
}

void LeachingPump::handle(Command command) {
  if (command == START_COMMAND) {
    setRelay(true);
    enter(PUMPING);
  } else if (command == STOP_COMMAND) {
    setRelay(false);
    enter(SOAKING);
  }
  Actuator::handle(command);
}

void LeachingPump::update() {
  unsigned long elapsed = millis() - stateSince;
  if (state == PUMPING && elapsed >= LEACHING_PUMP_MS) handle(STOP_COMMAND);
  else if (state == SOAKING && elapsed >= LEACHING_SOAK_MS) enter(IDLE);
}

void LeachingPump::evaluate(float conductivityDsM, float moisturePercent) {
  bool critical = conductivityDsM >= criticalDsM;
  if (state == PUMPING) {
    if (!critical) handle(STOP_COMMAND);
    return;
  }
  if (state == SOAKING) return;
  if (!critical) {  // recovered: a later alert starts over and the lock is released
    criticalStreak = 0;
    cycles = 0;
    enter(IDLE);
    return;
  }
  if (state == LOCKED) return;
  if (moisturePercent >= LEACHING_MAX_MOISTURE_PERCENT) {  // already saturated: more water would not drain
    criticalStreak = 0;
    return;
  }
  if (++criticalStreak < LEACHING_TRIGGER_READINGS) return;
  criticalStreak = 0;
  if (cycles >= LEACHING_MAX_CYCLES) {
    enter(LOCKED);
    return;
  }
  cycles++;
  handle(START_COMMAND);
}

const char *LeachingPump::stateName() const {
  switch (state) {
    case PUMPING: return "PUMPING";
    case SOAKING: return "SOAKING";
    case LOCKED: return "LOCKED";
    default: return "IDLE";
  }
}
