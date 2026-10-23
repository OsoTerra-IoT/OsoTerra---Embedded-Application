#ifndef SOIL_TEMPERATURE_SENSOR_H
#define SOIL_TEMPERATURE_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include "Sensor.h"

// DS18B20 soil temperature probe on a OneWire pin.
class SoilTemperatureSensor : public Sensor {
  OneWire oneWire;
  DallasTemperature probe;

public:
  static const int PROBE_DISCONNECTED_EVENT_ID = 20;
  static const Event PROBE_DISCONNECTED_EVENT;

  explicit SoilTemperatureSensor(int pin, EventHandler *eventHandler = nullptr);
  void begin();
  float read();  // NAN when the probe does not answer
};

#endif
