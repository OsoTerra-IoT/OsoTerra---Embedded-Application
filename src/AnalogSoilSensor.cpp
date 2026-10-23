#include "AnalogSoilSensor.h"
#include <Arduino.h>

const Event AnalogSoilSensor::READING_TAKEN_EVENT = Event(READING_TAKEN_EVENT_ID);

AnalogSoilSensor::AnalogSoilSensor(int pin, float maxValue, int samples, EventHandler *eventHandler)
    : Sensor(pin, eventHandler), maxValue(maxValue), samples(samples) {}

float AnalogSoilSensor::read() {
  long total = 0;
  for (int i = 0; i < samples; i++) {
    total += analogRead(pin);
    delay(2);
  }
  float value = (total / (float)samples) / 4095.0f * maxValue;
  on(READING_TAKEN_EVENT);
  return value;
}
