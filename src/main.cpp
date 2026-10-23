// OsoSense Embedded Application (ESP32): soil EC, moisture and temperature sent to the Edge Service.
// Wokwi has no EC probe or capacitive moisture sensor, so both are simulated with potentiometers.
#include <Arduino.h>
#include <WiFi.h>
#include "Config.h"
#include "OsoSenseDevice.h"

OsoSenseDevice device(EdgeServiceClient(EDGE_SERVICE_URL, DEVICE_CODE, DEVICE_API_KEY),
                      CROP_THRESHOLD_DSM, SAMPLE_INTERVAL_MS);

void connectWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to Wi-Fi");
  for (int i = 0; i < 40 && WiFi.status() != WL_CONNECTED; i++) {
    delay(250);
    Serial.print(".");
  }
  Serial.println(WiFi.status() == WL_CONNECTED ? " connected" : " offline");
}

void setup() {
  Serial.begin(115200);
  device.begin();
  connectWifi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) WiFi.reconnect();
  device.update();
  delay(10);
}
