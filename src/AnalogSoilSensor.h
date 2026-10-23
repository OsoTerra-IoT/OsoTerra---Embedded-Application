#ifndef ANALOG_SOIL_SENSOR_H
#define ANALOG_SOIL_SENSOR_H

#include "Sensor.h"

// Analog probe on an ADC pin, scaled linearly from [rawAtZero, rawAtMax] to [0, maxValue].
// Used for the EC probe and the capacitive moisture sensor; rawAtZero > rawAtMax handles inverse sensors.
class AnalogSoilSensor : public Sensor {
  float maxValue;
  int rawAtZero;
  int rawAtMax;
  int samples;
  int lastRaw = 0;

public:
  static const int READING_TAKEN_EVENT_ID = 10;
  static const Event READING_TAKEN_EVENT;

  AnalogSoilSensor(int pin, float maxValue, int rawAtZero = 0, int rawAtMax = 4095, int samples = 8,
                   EventHandler *eventHandler = nullptr);
  float read();  // average of several samples to remove noise, clamped to [0, maxValue]
  int raw() const { return lastRaw; }  // last averaged ADC value, for calibration
};

#endif
