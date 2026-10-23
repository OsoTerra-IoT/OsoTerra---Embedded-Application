#ifndef BUTTON_H
#define BUTTON_H

#include "Sensor.h"

// ModestIoT framework push button (INPUT_PULLUP). Emits a short or long press event on release.
class Button : public Sensor {
  unsigned long longPressMs;
  unsigned long pressedAt = 0;
  bool pressed = false;

public:
  static const int SHORT_PRESS_EVENT_ID = 0;
  static const int LONG_PRESS_EVENT_ID = 1;
  static const Event SHORT_PRESS_EVENT;
  static const Event LONG_PRESS_EVENT;

  Button(int pin, EventHandler *eventHandler = nullptr, unsigned long longPressMs = 3000);
  void begin();   // call it from setup()
  void update();  // call it on every loop
};

#endif
