#ifndef ANALOG_SOIL_SENSOR_H
#define ANALOG_SOIL_SENSOR_H

#include "Sensor.h"

// Analog probe on an ADC pin, scaled to [0, maxValue]. Used for the EC probe and the capacitive moisture sensor.
class AnalogSoilSensor : public Sensor {
  float maxValue;
  int samples;

public:
  static const int READING_TAKEN_EVENT_ID = 10;
  static const Event READING_TAKEN_EVENT;

  AnalogSoilSensor(int pin, float maxValue, int samples = 8, EventHandler *eventHandler = nullptr);
  float read();  // average of several samples to remove noise
};

#endif
