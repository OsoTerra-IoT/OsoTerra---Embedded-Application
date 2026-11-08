#include "SoilTemperatureSensor.h"
#include <math.h>

const Event SoilTemperatureSensor::PROBE_DISCONNECTED_EVENT = Event(PROBE_DISCONNECTED_EVENT_ID);

SoilTemperatureSensor::SoilTemperatureSensor(int pin, EventHandler *eventHandler)
    : Sensor(pin, eventHandler), oneWire(pin), probe(&oneWire) {}

// Non-blocking: each read returns the last finished conversion and starts the next one (the DS18B20 takes 750 ms).
void SoilTemperatureSensor::begin() {
  probe.begin();
  probe.setWaitForConversion(false);
  probe.requestTemperatures();
}

float SoilTemperatureSensor::read() {
  float celsius = probe.getTempCByIndex(0);
  probe.requestTemperatures();
  if (celsius == DEVICE_DISCONNECTED_C) {
    on(PROBE_DISCONNECTED_EVENT);
    return NAN;
  }
  return celsius;
}
