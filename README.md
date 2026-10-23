# OsoSense Embedded Application

Firmware of the **OsoSense** field device (Oso Terra, 1ASI0572 Desarrollo de Soluciones IoT, UPC 202620). The ESP32 measures soil electrical conductivity, moisture and temperature, sends each reading to the [Edge Service](https://github.com/OsoTerra-IoT/OsoTerra---Edge-Service) and shows the salinity level of the plot with an RGB LED.

## Stack
ESP32 DevKit V1 (WROOM-32) · C++ (Arduino framework) · PlatformIO · OneWire + DallasTemperature · Wokwi

## Circuit (section 5.6 of the report)
The Wokwi circuit (`diagram.json`) uses the parts bought for the prototype on a half breadboard. Wokwi does not ship the sensors or the power parts, so they are custom chips (see [chips/README.md](https://github.com/OsoTerra-IoT/OsoTerra---Embedded-Application/blob/develop/chips/README.md)).

| Part | Pin | Note |
|---|---|---|
| Generic TDS / EC module with BNC probe | GPIO 34 (ADC) | 0–2.3 V, read on the 1:2 soil-water extract; 3.3 V = `CONDUCTIVITY_MAX_DSM` (3.2 dS/m) |
| Capacitive moisture sensor v1.2 | GPIO 35 (ADC) | Calibrated between `MOISTURE_RAW_DRY` and `MOISTURE_RAW_WET` (inverse sensor) |
| DS18B20 waterproof probe | GPIO 4 | 4.7 kΩ pull-up to 3.3 V |
| RGB LED (common cathode) | GPIO 25 / 26 / 27 | 220 Ω per channel |
| Push button | GPIO 14 | `INPUT_PULLUP`, to GND |
| Power | 3V3 / GND | 6 V solar panel → TP4056 with DW01 protection (18650 cell) → HT7333 3.3 V regulator → breadboard rails |

All the sensors run on 3.3 V from the rails, never 5 V: the ESP32 inputs are 3.3 V.

## Design (ModestIoT)
The firmware follows the course's ModestIoT style: sensors notify the device with **events** (`on`) and the device gives **commands** to its actuators (`handle`).

| Class | Kind | Responsibility |
|---|---|---|
| `Event`, `Command`, `EventHandler`, `CommandHandler`, `Sensor`, `Actuator`, `Device`, `Led`, `Button` | Framework | ModestIoT base classes. |
| `AnalogSoilSensor` | Sensor | EC probe and moisture sensor: averages 8 ADC samples and scales them between two calibration points (inverse sensors allowed). |
| `SoilTemperatureSensor` | Sensor | DS18B20; emits `PROBE_DISCONNECTED_EVENT` and returns `NAN` when it does not answer. |
| `StatusLight` | Actuator | RGB LED made of three framework `Led`s; one command per pattern of the IoT style guide. |
| `EdgeServiceClient` | Support | `POST /api/v1/soil-readings` with `X-API-Key`; reads `compensatedConductivityDsM` and `status`. |
| `OsoSenseDevice` | Device | Sampling cycle, button events and salinity level. |

## Behavior
- Takes a reading every `SAMPLE_INTERVAL_MS` (15 min in the field, 10 s in Wokwi) and on a **short press** of the button.
- **Long press (3 s):** pairing mode, blue fast blinks, to register the activation code in the OsoSense app.
- Without the DS18B20 the reading goes without temperature; the Edge stores it raw and does not send it (TS08).
- The level uses the EC compensated to 25 °C returned by the Edge, with the same severities as the platform's `SalinityAlert`:

| EC / crop threshold | Level | LED |
|---|---|---|
| < 1.0 | Normal | green, 1 blink |
| < 1.2 | Watch | amber, 1 blink |
| < 1.5 | Warning | orange, 2 blinks |
| ≥ 1.5 | Critical | red, 3 blinks |
| — | Edge not reachable | white, long blink |
| — | Rejected by the Edge (4xx) | magenta, fast blinks |

## Configuration
Edit `src/Config.h`: `DEVICE_CODE`, `DEVICE_API_KEY` (must match `EDGE_DEVICES` in the Edge Service), Wi-Fi, `EDGE_SERVICE_URL`, `SAMPLE_INTERVAL_MS` and `CROP_THRESHOLD_DSM`.

### Real board
- `WIFI_CHANNEL 0` scans every channel; the ESP32 only joins 2.4 GHz networks.
- **Moisture calibration:** each reading prints `Raw ADC: conductivity …, moisture …` on the serial monitor (115200). Note the moisture value with the sensor in dry air and in a glass of water up to its line, and set them as `MOISTURE_RAW_DRY` and `MOISTURE_RAW_WET` (defaults 3200 and 1300 are typical for the v1.2 sensor at 3.3 V).

## Build and simulate
```bash
pio run
```
Then open the project with the Wokwi extension for VS Code (`wokwi.toml` points to the firmware, the custom chips and `diagram.json`). Move the sliders of the moisture and TDS chips and the DS18B20 temperature to change the readings. Run the Edge Service on port 5000 of the same PC: the simulator reaches it at `host.wokwi.internal`.
