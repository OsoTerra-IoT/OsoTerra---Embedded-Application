#ifndef EDGE_SERVICE_CLIENT_H
#define EDGE_SERVICE_CLIENT_H

#include <Arduino.h>
#include "SoilReading.h"

struct EdgeResponse {
  int httpStatus;                    // < 0 when the gateway could not be reached
  float compensatedConductivityDsM;  // NAN when the Edge did not compensate it
  String status;                     // PENDING, SYNCHRONIZED, UNCOMPENSATED or DISCARDED
  bool reached() const { return httpStatus > 0; }
};

// Sends readings to the Edge Service: POST /api/v1/soil-readings with X-API-Key.
class EdgeServiceClient {
  const char *url;
  const char *deviceCode;
  const char *apiKey;
  static float numberField(const String &json, const char *field);
  static String textField(const String &json, const char *field);

public:
  EdgeServiceClient(const char *url, const char *deviceCode, const char *apiKey);
  String toJson(const SoilReading &reading) const;
  EdgeResponse send(const SoilReading &reading);
};

#endif
