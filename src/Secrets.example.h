// Copy this file to src/Secrets.h (git ignores it) and fill in the plot's network for the board build.
// The ESP32 only joins 2.4 GHz networks. EDGE_SERVICE_URL uses the IP of the PC or Raspberry Pi running the Edge.
#define WIFI_SSID "network-name"
#define WIFI_PASSWORD "network-password"
#define EDGE_SERVICE_URL "http://192.168.1.10:5000/api/v1/soil-readings"
