#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "Command.h"

class CommandHandler {
public:
  virtual void handle(Command command) = 0;
  virtual ~CommandHandler() = default;
};

#endif
