#ifndef LEACHING_PUMP_H
#define LEACHING_PUMP_H

#include "Actuator.h"

// Relay-driven mini pump for the salt leaching demo. Waters only after sustained critical readings, in short timed
// cycles with a soak pause and a cycle limit, so a stuck reading cannot flood the container.
class LeachingPump : public Actuator {
  enum State { IDLE, PUMPING, SOAKING, LOCKED };

  float criticalDsM;
  State state = IDLE;
  int criticalStreak = 0;
  int cycles = 0;
  unsigned long stateSince = 0;

  void enter(State newState);
  void setRelay(bool on);

public:
  static const int START_COMMAND_ID = 300;
  static const int STOP_COMMAND_ID = 301;
  static const Command START_COMMAND;
  static const Command STOP_COMMAND;

  LeachingPump(int pin, float criticalDsM, CommandHandler *commandHandler = nullptr);
  void begin();                                                  // call it from setup()
  void update();                                                 // call it on every loop
  void evaluate(float conductivityDsM, float moisturePercent);  // call it on every reading
  void handle(Command command) override;
  bool isLocked() const { return state == LOCKED; }
  int getCycles() const { return cycles; }
  const char *stateName() const;
};

#endif
