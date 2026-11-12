#include "SoilTemperatureSensor.h"
#include <Arduino.h>
#include <math.h>
#include "Config.h"

// The DS18B20 needs up to 750 ms per conversion at 12 bits.
static const unsigned long CONVERSION_MS = 800;

const Event SoilTemperatureSensor::PROBE_DISCONNECTED_EVENT = Event(PROBE_DISCONNECTED_EVENT_ID);

SoilTemperatureSensor::SoilTemperatureSensor(int pin, EventHandler *eventHandler)
    : Sensor(pin, eventHandler), oneWire(pin), probe(&oneWire) {}

void SoilTemperatureSensor::begin() {
  probe.begin();
#if OFFLINE_MODE
  probe.setWaitForConversion(false);
  probe.requestTemperatures();
  requestedAt = millis();
#endif
}

float SoilTemperatureSensor::read() {
#if OFFLINE_MODE
  // Fast bench sampling: keep the last value while the next conversion runs instead of blocking.
  if (millis() - requestedAt < CONVERSION_MS) return lastCelsius;
#else
  probe.requestTemperatures();
#endif
  float celsius = probe.getTempCByIndex(0);
#if OFFLINE_MODE
  probe.requestTemperatures();
  requestedAt = millis();
#endif
  if (celsius == DEVICE_DISCONNECTED_C) {
    on(PROBE_DISCONNECTED_EVENT);
    lastCelsius = NAN;
    return NAN;
  }
  lastCelsius = celsius;
  return celsius;
}
