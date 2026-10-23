#ifndef SENSOR_H
#define SENSOR_H

#include "EventHandler.h"

// ModestIoT: a sensor reads a pin and notifies its handler with events.
class Sensor : public EventHandler {
protected:
  int pin;
  EventHandler *handler;

public:
  explicit Sensor(int pin, EventHandler *eventHandler = nullptr);
  void on(Event event) override;
  void setHandler(EventHandler *eventHandler);
  int getPin() const { return pin; }
};

#endif
