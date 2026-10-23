#include "OsoSenseDevice.h"
#include <Arduino.h>
#include "Config.h"

const Command OsoSenseDevice::TAKE_READING_COMMAND = Command(TAKE_READING_COMMAND_ID);
const Command OsoSenseDevice::START_PAIRING_COMMAND = Command(START_PAIRING_COMMAND_ID);

OsoSenseDevice::OsoSenseDevice(EdgeServiceClient edgeClient, float cropThresholdDsM, unsigned long sampleIntervalMs)
    : conductivityProbe(CONDUCTIVITY_PIN, CONDUCTIVITY_MAX_DSM),
      moistureSensor(MOISTURE_PIN, MOISTURE_MAX_PERCENT),
      temperatureSensor(TEMPERATURE_PIN),
      button(BUTTON_PIN),
      statusLight(LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN),
      edgeClient(edgeClient),
      cropThresholdDsM(cropThresholdDsM),
      sampleIntervalMs(sampleIntervalMs) {}

void OsoSenseDevice::begin() {
  button.setHandler(this);
  button.begin();
  temperatureSensor.setHandler(this);
  temperatureSensor.begin();
  statusLight.begin();
  statusLight.handle(Command(StatusLight::SHOW_STARTUP_COMMAND_ID));
}

void OsoSenseDevice::update() {
  button.update();
  if (firstSample || millis() - lastSampleAt >= sampleIntervalMs) handle(TAKE_READING_COMMAND);
}

void OsoSenseDevice::on(Event event) {
  if (event == Button::SHORT_PRESS_EVENT) handle(TAKE_READING_COMMAND);
  else if (event == Button::LONG_PRESS_EVENT) handle(START_PAIRING_COMMAND);
  else if (event == SoilTemperatureSensor::PROBE_DISCONNECTED_EVENT)
    Serial.println("DS18B20 not answering: the reading goes without temperature");
}

void OsoSenseDevice::handle(Command command) {
  if (command == TAKE_READING_COMMAND) {
    firstSample = false;
    lastSampleAt = millis();
    SoilReading reading = takeReading();
    Serial.println("Reading: " + edgeClient.toJson(reading));

    EdgeResponse response = edgeClient.send(reading);
    if (!response.reached()) {
      Serial.println("Edge Service not reachable");
      statusLight.handle(Command(StatusLight::SHOW_OFFLINE_COMMAND_ID));
      return;
    }
    Serial.printf("Edge Service answered %d (%s)\n", response.httpStatus, response.status.c_str());
    if (response.httpStatus >= 400) {
      statusLight.handle(Command(StatusLight::SHOW_ERROR_COMMAND_ID));
      return;
    }
    // The level uses the EC compensated to 25 °C by the Edge; the raw value only if it could not compensate.
    float conductivity = isnan(response.compensatedConductivityDsM) ? reading.conductivityDsM
                                                                    : response.compensatedConductivityDsM;
    statusLight.handle(levelCommand(conductivity));
  } else if (command == START_PAIRING_COMMAND) {
    Serial.printf("Pairing mode: register %s in the OsoSense app\n", DEVICE_CODE);
    statusLight.handle(Command(StatusLight::SHOW_PAIRING_COMMAND_ID));
  }
}

SoilReading OsoSenseDevice::takeReading() {
  float conductivity = conductivityProbe.read();
  float moisture = moistureSensor.read();
  return SoilReading{conductivity, moisture, temperatureSensor.read()};
}

// Same severities as the platform's SalinityAlert: WATCH < 1.2x, WARNING < 1.5x, CRITICAL >= 1.5x the crop threshold.
Command OsoSenseDevice::levelCommand(float conductivityDsM) const {
  float ratio = conductivityDsM / cropThresholdDsM;
  if (ratio < 1.0f) return Command(StatusLight::SHOW_NORMAL_COMMAND_ID);
  if (ratio < 1.2f) return Command(StatusLight::SHOW_WATCH_COMMAND_ID);
  if (ratio < 1.5f) return Command(StatusLight::SHOW_WARNING_COMMAND_ID);
  return Command(StatusLight::SHOW_CRITICAL_COMMAND_ID);
}
