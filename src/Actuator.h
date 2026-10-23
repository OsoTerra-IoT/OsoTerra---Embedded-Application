#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "CommandHandler.h"

// ModestIoT: an actuator drives a pin when it receives commands.
class Actuator : public CommandHandler {
protected:
  int pin;
  CommandHandler *handler;

public:
  explicit Actuator(int pin, CommandHandler *commandHandler = nullptr);
  void handle(Command command) override;
  void setHandler(CommandHandler *commandHandler);
  int getPin() const { return pin; }
};

#endif
