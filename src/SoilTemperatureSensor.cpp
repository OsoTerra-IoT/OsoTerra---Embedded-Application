#include "SoilTemperatureSensor.h"
#include <math.h>

const Event SoilTemperatureSensor::PROBE_DISCONNECTED_EVENT = Event(PROBE_DISCONNECTED_EVENT_ID);

SoilTemperatureSensor::SoilTemperatureSensor(int pin, EventHandler *eventHandler)
    : Sensor(pin, eventHandler), oneWire(pin), probe(&oneWire) {}

void SoilTemperatureSensor::begin() { probe.begin(); }

float SoilTemperatureSensor::read() {
  probe.requestTemperatures();
  float celsius = probe.getTempCByIndex(0);
  if (celsius == DEVICE_DISCONNECTED_C) {
    on(PROBE_DISCONNECTED_EVENT);
    return NAN;
  }
  return celsius;
}
