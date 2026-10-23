#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "Event.h"

class EventHandler {
public:
  virtual void on(Event event) = 0;
  virtual ~EventHandler() = default;
};

#endif
