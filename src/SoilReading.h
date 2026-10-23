#ifndef SOIL_READING_H
#define SOIL_READING_H

#include <math.h>

struct SoilReading {
  float conductivityDsM;
  float moisturePercent;
  float temperatureCelsius;  // NAN when the probe is disconnected
  bool hasTemperature() const { return !isnan(temperatureCelsius); }
};

#endif
