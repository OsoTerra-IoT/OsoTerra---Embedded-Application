#ifndef FLUSH_PUMP_H
#define FLUSH_PUMP_H

#include "Actuator.h"

// Contingency actuator: a mini water pump switched by a 1-channel relay module. It sends fresh water to the pot to wash
// the salts out (leaching) when the salinity stays Critical. It never runs longer than the maximum duration it is given.
class FlushPump : public Actuator {
  bool activeLow;
  bool running = false;
  unsigned long startedAt = 0;
  unsigned long durationMs = 0;
  void drive(bool on);

public:
  static const int START_FLUSH_COMMAND_ID = 300;
  static const int STOP_FLUSH_COMMAND_ID = 301;
  static const Command START_FLUSH_COMMAND;
  static const Command STOP_FLUSH_COMMAND;

  FlushPump(int pin, bool activeLow, unsigned long durationMs, CommandHandler *commandHandler = nullptr);
  void begin();  // call it from setup(): the pump starts off
  void handle(Command command) override;
  // Call it on every loop: stops the pump when its time is over. Returns true on the loop where it stopped.
  bool update(unsigned long now);
  bool isRunning() const { return running; }
};

#endif
