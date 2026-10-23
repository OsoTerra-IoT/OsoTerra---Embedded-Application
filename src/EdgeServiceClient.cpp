#include "EdgeServiceClient.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <math.h>

EdgeServiceClient::EdgeServiceClient(const char *url, const char *deviceCode, const char *apiKey)
    : url(url), deviceCode(deviceCode), apiKey(apiKey) {}

String EdgeServiceClient::toJson(const SoilReading &reading) const {
  String body = "{\"deviceId\":\"" + String(deviceCode) + "\"";
  body += ",\"conductivityDsM\":" + String(reading.conductivityDsM, 2);
  body += ",\"moisturePercent\":" + String(reading.moisturePercent, 1);
  // Without the DS18B20 the Edge stores the reading raw and does not send it (TS08).
  if (reading.hasTemperature()) body += ",\"temperatureCelsius\":" + String(reading.temperatureCelsius, 1);
  body += "}";
  return body;
}

EdgeResponse EdgeServiceClient::send(const SoilReading &reading) {
  EdgeResponse response{-1, NAN, ""};
  if (WiFi.status() != WL_CONNECTED) return response;

  HTTPClient http;
  http.begin(url);
  http.setTimeout(5000);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", apiKey);
  response.httpStatus = http.POST(toJson(reading));
  if (response.reached()) {
    String payload = http.getString();
    response.compensatedConductivityDsM = numberField(payload, "compensatedConductivityDsM");
    response.status = textField(payload, "status");
  }
  http.end();
  return response;
}

// The Edge answers a small flat JSON object, so a field lookup is enough (no JSON library on the device).
float EdgeServiceClient::numberField(const String &json, const char *field) {
  String key = String("\"") + field + "\":";
  int at = json.indexOf(key);
  if (at < 0) return NAN;
  int start = at + key.length();
  while (start < (int)json.length() && json[start] == ' ') start++;
  if (json.startsWith("null", start)) return NAN;
  return json.substring(start).toFloat();
}

String EdgeServiceClient::textField(const String &json, const char *field) {
  String key = String("\"") + field + "\":";
  int at = json.indexOf(key);
  if (at < 0) return "";
  int start = json.indexOf('"', at + key.length());
  if (start < 0) return "";
  int end = json.indexOf('"', start + 1);
  return end < 0 ? "" : json.substring(start + 1, end);
}
