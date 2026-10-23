#ifndef LED_H
#define LED_H

#include "Actuator.h"

// ModestIoT framework LED on a digital pin.
class Led : public Actuator {
  bool state;

public:
  static const int TURN_ON_COMMAND_ID = 0;
  static const int TURN_OFF_COMMAND_ID = 1;
  static const int TOGGLE_COMMAND_ID = 2;
  static const Command TURN_ON_COMMAND;
  static const Command TURN_OFF_COMMAND;
  static const Command TOGGLE_COMMAND;

  explicit Led(int pin, bool initialState = false, CommandHandler *commandHandler = nullptr);
  void begin();  // call it from setup()
  void handle(Command command) override;
  bool getState() const { return state; }
  void setState(bool newState);
};

#endif
