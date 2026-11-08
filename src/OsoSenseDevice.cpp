#include "OsoSenseDevice.h"
#include <Arduino.h>
#include "Config.h"

const Command OsoSenseDevice::TAKE_READING_COMMAND = Command(TAKE_READING_COMMAND_ID);
const Command OsoSenseDevice::START_PAIRING_COMMAND = Command(START_PAIRING_COMMAND_ID);

OsoSenseDevice::OsoSenseDevice(EdgeServiceClient edgeClient, float cropThresholdDsM, unsigned long sampleIntervalMs)
    : conductivityProbe(CONDUCTIVITY_PIN, CONDUCTIVITY_MAX_DSM),
      moistureSensor(MOISTURE_PIN, MOISTURE_MAX_PERCENT, MOISTURE_RAW_DRY, MOISTURE_RAW_WET),
      temperatureSensor(TEMPERATURE_PIN),
      button(BUTTON_PIN),
      statusLight(LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN),
      pump(PUMP_RELAY_PIN, PUMP_RELAY_ACTIVE_LOW, FLUSH_DURATION_MS),
      flushPolicy(FLUSH_CONFIRM_READINGS, FLUSH_COOLDOWN_MS, FLUSH_MAX_CYCLES, FLUSH_SATURATED_PERCENT),
      edgeClient(edgeClient),
      cropThresholdDsM(cropThresholdDsM),
      sampleIntervalMs(sampleIntervalMs) {}

void OsoSenseDevice::begin() {
  button.setHandler(this);
  button.begin();
  temperatureSensor.setHandler(this);
  temperatureSensor.begin();
  statusLight.begin();
  pump.begin();
  statusLight.handle(Command(StatusLight::SHOW_STARTUP_COMMAND_ID));
}

void OsoSenseDevice::update() {
  button.update();
  if (pump.update(millis())) {
    flushPolicy.flushFinished(millis());
    Serial.println("Contingency: wash finished, pump off. Waiting for the soil to drain before measuring again");
  }
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
#if OFFLINE_MODE
    if (isnan(reading.temperatureCelsius)) Serial.printf("Humedad: %.1f %% | Conductividad: %.2f dS/m | Temp: --\n", reading.moisturePercent, reading.conductivityDsM);
    else Serial.printf("Humedad: %.1f %% | Conductividad: %.2f dS/m | Temp: %.1f C\n", reading.moisturePercent, reading.conductivityDsM, reading.temperatureCelsius);
    statusLight.flash(levelCommand(reading.conductivityDsM));
    applyContingency(reading.conductivityDsM, reading.moisturePercent);
    return;
#endif
    Serial.println("Reading: " + edgeClient.toJson(reading));

    EdgeResponse response = edgeClient.send(reading);
    if (!response.reached()) {
      Serial.println("Edge Service not reachable");
      statusLight.handle(Command(StatusLight::SHOW_OFFLINE_COMMAND_ID));
      applyContingency(reading.conductivityDsM, reading.moisturePercent);  // the washing does not depend on the Edge
      return;
    }
    Serial.printf("Edge Service answered %d (%s)\n", response.httpStatus, response.status.c_str());
    if (response.httpStatus >= 400) {
      statusLight.handle(Command(StatusLight::SHOW_ERROR_COMMAND_ID));
      applyContingency(reading.conductivityDsM, reading.moisturePercent);
      return;
    }
    // The level uses the EC compensated to 25 °C by the Edge; the raw value only if it could not compensate.
    float conductivity = isnan(response.compensatedConductivityDsM) ? reading.conductivityDsM
                                                                    : response.compensatedConductivityDsM;
    statusLight.handle(levelCommand(conductivity));
    applyContingency(conductivity, reading.moisturePercent);
  } else if (command == START_PAIRING_COMMAND) {
    Serial.printf("Pairing mode: register %s in the OsoSense app\n", DEVICE_CODE);
    statusLight.handle(Command(StatusLight::SHOW_PAIRING_COMMAND_ID));
  }
}

SoilReading OsoSenseDevice::takeReading() {
  float conductivity = conductivityProbe.read();
  float moisture = moistureSensor.read();
  Serial.printf("Raw ADC: conductivity %d, moisture %d\n", conductivityProbe.raw(), moistureSensor.raw());
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

// Contingency: if the salinity stays Critical, wash the soil with fresh water. The relay and the pump are cheap parts;
// the decision (when to wash, how often, when to stop) lives in FlushPolicy so it can be tested without hardware.
void OsoSenseDevice::applyContingency(float conductivityDsM, float moisturePercent) {
#if CONTINGENCY_ENABLED
  switch (flushPolicy.evaluate(conductivityDsM, cropThresholdDsM, moisturePercent, millis())) {
    case FlushPolicy::START:
      flushPolicy.flushStarted();
      Serial.printf("Contingency: EC %.2f dS/m is Critical -> wash %d/%d, pump on for %lu s\n", conductivityDsM,
                    flushPolicy.getCycles(), flushPolicy.getMaxCycles(), FLUSH_DURATION_MS / 1000UL);
      statusLight.handle(Command(StatusLight::SHOW_FLUSHING_COMMAND_ID));
      pump.handle(FlushPump::START_FLUSH_COMMAND);
      break;
    case FlushPolicy::WAIT_COOLDOWN: Serial.println("Contingency: still Critical, waiting for the soil to drain before the next wash"); break;
    case FlushPolicy::SOIL_SATURATED: Serial.println("Contingency: soil is saturated, no wash. Let it drain and check the water source"); break;
    case FlushPolicy::MAX_CYCLES_REACHED: Serial.println("Contingency: washes used up and EC is still Critical. A person must act (check the water, change the substrate)"); break;
    case FlushPolicy::NONE: break;
  }
#endif
}
