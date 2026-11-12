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

// 1 = bench test over USB on the real prototype (env bench): no Wi-Fi and no Edge Service, fast sampling,
// readings and contingency on the serial monitor. 0 = Edge Service flow and Wokwi (env esp32dev).
#ifndef OFFLINE_MODE
#define OFFLINE_MODE 0
#endif

// 15 min in the field; 10 s in the simulation; 100 ms on the bench.
#if OFFLINE_MODE
#define SAMPLE_INTERVAL_MS 100UL
#else
#define SAMPLE_INTERVAL_MS 10000UL
#endif

// Pins of the OsoSense circuit (section 5.6 of the report).
#define CONDUCTIVITY_PIN 34
#define MOISTURE_PIN 35
#define TEMPERATURE_PIN 4
#define LED_RED_PIN 25
#define LED_GREEN_PIN 26
#define LED_BLUE_PIN 27
#define BUTTON_PIN 14
#define PUMP_RELAY_PIN 18  // contingency: relay module that switches the mini water pump

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

// Contingency: when the salinity stays Critical the device washes the soil with fresh water (pump on a relay).
// 1 = enabled. Without the relay connected the device works the same and only logs the decision.
#define CONTINGENCY_ENABLED 1
// 1 for the usual blue 1-channel relay modules (they switch when IN goes LOW, like the prototype's); 0 for active-high
// modules and Wokwi's (its sensor chips read a HIGH relay signal as the pump running). The bench build sets 1.
#ifndef PUMP_RELAY_ACTIVE_LOW
#define PUMP_RELAY_ACTIVE_LOW 0
#endif
#if OFFLINE_MODE
// Bench demo with the prototype: 5 s washes, 30 s to mix the water, 5 readings (0.5 s) to confirm.
#define FLUSH_DURATION_MS 5000UL
#define FLUSH_COOLDOWN_MS 30000UL
#define FLUSH_CONFIRM_READINGS 5
#else
// Pump time per wash: 60 s in the field (about 150 ml through a 3 V mini pump); 8 s in the simulation.
#define FLUSH_DURATION_MS 8000UL
// Wait between washes so the water drains and the next reading is real: 30 min in the field; 20 s in the simulation.
#define FLUSH_COOLDOWN_MS 20000UL
// Critical readings in a row before the first wash (filters one noisy reading).
#define FLUSH_CONFIRM_READINGS 2
#endif
// Washes before giving up and asking for a person (the pot may be saturated or the water itself may be salty).
#define FLUSH_MAX_CYCLES 3
// Never wash a soil that is already this wet (%): it would waterlog the plant.
#define FLUSH_SATURATED_PERCENT 90.0f

#endif
