#ifndef CONFIG_H
#define CONFIG_H

// Device identity: the activation code printed on the kit and its key registered in the Edge Service (EDGE_DEVICES).
#define DEVICE_CODE "OS-014"
#define DEVICE_API_KEY "os-014-device-key"

// Wokwi-GUEST is the simulator's open network; in the field use the plot's 2.4 GHz Wi-Fi.
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
// 0 scans every channel, so it works with any router and in Wokwi (6 only skips the scan in the simulator).
#define WIFI_CHANNEL 0

// Edge Service on the local gateway. In Wokwi, host.wokwi.internal reaches the PC running the simulation.
#define EDGE_SERVICE_URL "http://host.wokwi.internal:5000/api/v1/soil-readings"

// 0 = Edge Service flow and Wokwi (env esp32dev).
// 1 = bench demo over USB (env bench): no Wi-Fi nor Edge, fast sampling and the LED level computed on the device.
#ifndef OFFLINE_MODE
#define OFFLINE_MODE 0
#endif

// 15 min in the field; 10 s in the simulation; 100 ms on the bench.
#if OFFLINE_MODE
#define SAMPLE_INTERVAL_MS 100UL
#define STATUS_BLINK_MS 15
#else
#define SAMPLE_INTERVAL_MS 10000UL
#define STATUS_BLINK_MS 150
#endif

// Pins of the OsoSense circuit (section 5.6 of the report).
#define CONDUCTIVITY_PIN 34
#define MOISTURE_PIN 35
#define TEMPERATURE_PIN 4
#define LED_RED_PIN 25
#define LED_GREEN_PIN 26
#define LED_BLUE_PIN 27
#define BUTTON_PIN 14
#define LEACHING_PUMP_PIN 18

// Probe ranges: the ADC (0-4095) is mapped to these values.
// Generic TDS / EC module (0-2.3 V output) on the 1:2 soil-water extract: 3.3 V would be 3.2 dS/m.
#define CONDUCTIVITY_MAX_DSM 3.2f
#define MOISTURE_MAX_PERCENT 100.0f

// Moisture calibration: raw ADC of the capacitive sensor in dry air (0 %) and in water up to its line (100 %).
// The v1.2 sensor is inverse (more water, lower reading); read both values from the serial monitor and set them here.
// In Wokwi the potentiometer then reads 100 % at the left end and 0 % at the right end.
// The bench values belong to the sensor mounted on the prototype; the others match the Wokwi chip.
#if OFFLINE_MODE
#define MOISTURE_RAW_DRY 3749
#define MOISTURE_RAW_WET 1534
#else
#define MOISTURE_RAW_DRY 3200
#define MOISTURE_RAW_WET 1300
#endif

// Salinity threshold of the plot's crop (avocado 1.1 dS/m); the platform assigns it when the sensor is linked.
#define CROP_THRESHOLD_DSM 1.1f

// Salt leaching: a relay on GPIO 18 drives a mini pump. It starts at the CRITICAL level (1.5x the crop threshold)
// after a few consecutive critical readings, and is limited in time and cycles.
// The relay module is active-low: LOW turns it on and the pin floats to turn it off (3.3 V does not release a 5 V module).
#define LEACHING_CRITICAL_RATIO 1.5f
#define LEACHING_RELAY_ACTIVE_LOW 1
#if OFFLINE_MODE
#define LEACHING_TRIGGER_READINGS 5
#else
#define LEACHING_TRIGGER_READINGS 2
#endif
#define LEACHING_MAX_MOISTURE_PERCENT 90.0f
#define LEACHING_PUMP_MS 5000UL
#define LEACHING_SOAK_MS 30000UL
#define LEACHING_MAX_CYCLES 3

#endif
