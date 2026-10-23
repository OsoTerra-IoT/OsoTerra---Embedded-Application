#ifndef CONFIG_H
#define CONFIG_H

// Device identity: the activation code printed on the kit and its key registered in the Edge Service (EDGE_DEVICES).
#define DEVICE_CODE "OS-014"
#define DEVICE_API_KEY "os-014-device-key"

// Wokwi-GUEST is the simulator's open network; in the field use the plot's Wi-Fi.
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

// Edge Service on the local gateway. In Wokwi, host.wokwi.internal reaches the PC running the simulation.
#define EDGE_SERVICE_URL "http://host.wokwi.internal:5000/api/v1/soil-readings"

// 15 min in the field; 10 s in the simulation.
#define SAMPLE_INTERVAL_MS 10000UL

// Pins of the OsoSense circuit (section 5.6 of the report).
#define CONDUCTIVITY_PIN 34
#define MOISTURE_PIN 35
#define TEMPERATURE_PIN 4
#define LED_RED_PIN 25
#define LED_GREEN_PIN 26
#define LED_BLUE_PIN 27
#define BUTTON_PIN 14

// Probe ranges: the ADC (0-4095) is mapped to these values.
#define CONDUCTIVITY_MAX_DSM 5.0f
#define MOISTURE_MAX_PERCENT 100.0f

// Salinity threshold of the plot's crop (avocado 1.1 dS/m); the platform assigns it when the sensor is linked.
#define CROP_THRESHOLD_DSM 1.1f

#endif
