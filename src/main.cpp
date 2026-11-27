// OsoSense Embedded Application (ESP32): soil EC, moisture and temperature sent to the Edge Service.
// Wokwi has no EC probe or capacitive moisture sensor, so both are simulated with potentiometers.
#include <Arduino.h>
#include <WiFi.h>
#include "Config.h"
#if REAL_BOARD
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"
#endif
#include "OsoSenseDevice.h"

OsoSenseDevice device(EdgeServiceClient(EDGE_SERVICE_URL, DEVICE_CODE, DEVICE_API_KEY),
                      CROP_THRESHOLD_DSM, SAMPLE_INTERVAL_MS);

void connectWifi() {
#if REAL_BOARD
  // The radio's current peaks brown out the board on a weak USB port or cable (a laptop on battery), at start-up
  // and again on every send, so the brownout reset stays off in this build and a lower TX power is enough indoors.
  // Credentials are not written to flash and nothing else writes it, so a dip cannot corrupt it.
  // With a good 5 V supply (charger or power bank) the board does not need this.
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  // Modem sleep lets the radio doze between beacons; behind a phone hotspot the Edge's answer then gets lost.
  WiFi.setSleep(false);
#endif
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to Wi-Fi");
  for (int i = 0; i < 40 && WiFi.status() != WL_CONNECTED; i++) {
    delay(250);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED)
    Serial.printf(" connected, IP %s, signal %d dBm\n", WiFi.localIP().toString().c_str(), WiFi.RSSI());
  else
    Serial.println(" offline");
}

void setup() {
  Serial.begin(115200);
  device.begin();
#if !OFFLINE_MODE
  connectWifi();
#else
  Serial.println("Offline mode: no Wi-Fi, readings only");
#endif
}

void loop() {
#if !OFFLINE_MODE
  // Retry now and then: calling reconnect on every loop restarts the attempt before it can finish.
  static unsigned long lastWifiRetryAt = 0;
  if (WiFi.status() != WL_CONNECTED && millis() - lastWifiRetryAt >= WIFI_RETRY_MS) {
    lastWifiRetryAt = millis();
    Serial.println("Wi-Fi lost, reconnecting");
    WiFi.reconnect();
  }
#endif
  device.update();
  delay(10);
}
