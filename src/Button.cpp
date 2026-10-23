#include "Button.h"
#include <Arduino.h>

const Event Button::SHORT_PRESS_EVENT = Event(SHORT_PRESS_EVENT_ID);
const Event Button::LONG_PRESS_EVENT = Event(LONG_PRESS_EVENT_ID);

Button::Button(int pin, EventHandler *eventHandler, unsigned long longPressMs)
    : Sensor(pin, eventHandler), longPressMs(longPressMs) {}

void Button::begin() { pinMode(pin, INPUT_PULLUP); }

void Button::update() {
  bool down = digitalRead(pin) == LOW;
  if (down && !pressed) {
    pressed = true;
    pressedAt = millis();
  } else if (!down && pressed) {
    pressed = false;
    unsigned long held = millis() - pressedAt;
    if (held < 30) return;  // debounce
    on(held >= longPressMs ? LONG_PRESS_EVENT : SHORT_PRESS_EVENT);
  }
}
