#ifndef OSOSENSE_DEVICE_H
#define OSOSENSE_DEVICE_H

#include "Device.h"
#include "AnalogSoilSensor.h"
#include "SoilTemperatureSensor.h"
#include "Button.h"
#include "StatusLight.h"
#include "EdgeServiceClient.h"

// OsoSense field device: samples the soil, sends the reading to the Edge Service and shows the salinity level.
class OsoSenseDevice : public Device {
  AnalogSoilSensor conductivityProbe;
  AnalogSoilSensor moistureSensor;
  SoilTemperatureSensor temperatureSensor;
  Button button;
  StatusLight statusLight;
  EdgeServiceClient edgeClient;
  float cropThresholdDsM;
  unsigned long sampleIntervalMs;
  unsigned long lastSampleAt = 0;
  bool firstSample = true;

  SoilReading takeReading();
  Command levelCommand(float conductivityDsM) const;

public:
  static const int TAKE_READING_COMMAND_ID = 200;
  static const int START_PAIRING_COMMAND_ID = 201;
  static const Command TAKE_READING_COMMAND;
  static const Command START_PAIRING_COMMAND;

  OsoSenseDevice(EdgeServiceClient edgeClient, float cropThresholdDsM, unsigned long sampleIntervalMs);
  void begin();
  void update();  // call it on every loop
  void on(Event event) override;
  void handle(Command command) override;
};

#endif
