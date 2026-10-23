#ifndef DEVICE_H
#define DEVICE_H

#include "EventHandler.h"
#include "CommandHandler.h"

// ModestIoT: a device receives events from its sensors and gives commands to its actuators.
class Device : public EventHandler, public CommandHandler {
public:
  void on(Event event) override = 0;
  void handle(Command command) override = 0;
};

#endif
