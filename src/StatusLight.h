#ifndef STATUS_LIGHT_H
#define STATUS_LIGHT_H

#include "Actuator.h"
#include "Led.h"

// RGB status LED built from three framework Leds. Each command plays a pattern of the IoT style guide (5.1.2).
class StatusLight : public Actuator {
  Led red;
  Led green;
  Led blue;
  void setColor(bool r, bool g, bool b);
  void blink(int times, bool r, bool g, bool b, int onMs = 150, int offMs = 150);

public:
  static const int SHOW_NORMAL_COMMAND_ID = 100;
  static const int SHOW_WATCH_COMMAND_ID = 101;
  static const int SHOW_WARNING_COMMAND_ID = 102;
  static const int SHOW_CRITICAL_COMMAND_ID = 103;
  static const int SHOW_OFFLINE_COMMAND_ID = 104;
  static const int SHOW_PAIRING_COMMAND_ID = 105;
  static const int SHOW_STARTUP_COMMAND_ID = 106;
  static const int SHOW_ERROR_COMMAND_ID = 107;

  StatusLight(int redPin, int greenPin, int bluePin, CommandHandler *commandHandler = nullptr);
  void begin();
  void handle(Command command) override;
};

#endif
