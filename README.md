# OsoSense Embedded Application

Firmware of the **OsoSense** field device (Oso Terra, 1ASI0572 Desarrollo de Soluciones IoT, UPC 202620). The ESP32 measures soil electrical conductivity, moisture and temperature, sends each reading to the [Edge Service](https://github.com/OsoTerra-IoT/OsoTerra---Edge-Service) and shows the salinity level of the plot with an RGB LED.

## Stack
ESP32 DevKit C v4 · C++ (Arduino framework) · PlatformIO · OneWire + DallasTemperature · Wokwi

## Circuit (section 5.6 of the report)
| Part | Pin | Note |
|---|---|---|
| EC probe | GPIO 34 (ADC) | Simulated with a potentiometer in Wokwi, 0–5 dS/m |
| Capacitive moisture sensor | GPIO 35 (ADC) | Simulated with a potentiometer in Wokwi, 0–100 % |
| DS18B20 soil temperature | GPIO 4 | 4.7 kΩ pull-up |
| RGB LED (common cathode) | GPIO 25 / 26 / 27 | 220 Ω per channel |
| Push button | GPIO 14 | `INPUT_PULLUP` |

## Design (ModestIoT)
The firmware follows the course's ModestIoT style: sensors notify the device with **events** (`on`) and the device gives **commands** to its actuators (`handle`).

| Class | Kind | Responsibility |
|---|---|---|
| `Event`, `Command`, `EventHandler`, `CommandHandler`, `Sensor`, `Actuator`, `Device`, `Led`, `Button` | Framework | ModestIoT base classes. |
| `AnalogSoilSensor` | Sensor | EC probe and moisture sensor: averages 8 ADC samples and scales them. |
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

## Build and simulate
```bash
pio run
```
Then open the project with the Wokwi extension for VS Code (`wokwi.toml` points to the firmware and `diagram.json` is the circuit). Run the Edge Service on port 5000 of the same PC: the simulator reaches it at `host.wokwi.internal`.
