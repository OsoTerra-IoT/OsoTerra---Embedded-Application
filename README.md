# OsoSense Embedded Application

Firmware of the **OsoSense** field device (Oso Terra, 1ASI0572 Desarrollo de Soluciones IoT, UPC 202620). The ESP32 measures soil electrical conductivity, moisture and temperature, sends each reading to the [Edge Service](https://github.com/OsoTerra-IoT/OsoTerra---Edge-Service) and shows the salinity level of the plot with an RGB LED.

## Stack
ESP32 DevKit V1 (WROOM-32) · C++ (Arduino framework) · PlatformIO · OneWire + DallasTemperature · Wokwi

## Circuit (section 5.6 of the report)
The Wokwi circuit (`diagram.json`) uses the parts bought for the prototype on an 830-point breadboard. Every wire joins two points; the nets meet in the breadboard strips and rails. Wokwi does not ship the sensors or the power parts, so they are custom chips (see [chips/README.md](https://github.com/OsoTerra-IoT/OsoTerra---Embedded-Application/blob/develop/chips/README.md)).

| Part | Pin | Note |
|---|---|---|
| TDS Meter V1.0 with its probe | GPIO 34 (ADC) | 0–2.3 V, read on the 1:2 soil-water extract; 3.3 V = `CONDUCTIVITY_MAX_DSM` (3.2 dS/m) |
| Capacitive Soil Moisture Sensor v2.0 | GPIO 35 (ADC) | Calibrated between `MOISTURE_RAW_DRY` and `MOISTURE_RAW_WET` (inverse sensor) |
| DS18B20 waterproof probe | GPIO 4 | 4.7 kΩ pull-up to 3.3 V |
| RGB LED (common cathode) | GPIO 25 / 26 / 27 | 220 Ω per channel |
| 2-pin push button | GPIO 14 | `INPUT_PULLUP`, to GND |
| 1-channel relay module + mini water pump (contingency) | GPIO 18 | Relay IN to D18, VCC to 5 V (VIN with USB), GND to GND. The pump has its own 2 × AA pack on COM / NO, so the ESP32 never powers it. In Wokwi a blue LED stands for the pump |
| Power | VIN / GND | 6 V solar panel → TP4056 with protection (USB-C) charging the 18650 → OUT+/OUT− to VIN/GND; the board's regulator gives 3.3 V to the rails |

All the sensors run on 3.3 V from the rails, never 5 V: the ESP32 inputs are 3.3 V. On VIN the 18650 (3.0–4.2 V) is close to the dropout of the board's regulator, so below about 3.8 V the 3.3 V line sags; a low-dropout regulator such as the HT7333 between OUT+ and 3V3 fixes it.

## Design (ModestIoT)
The firmware follows the course's ModestIoT style: sensors notify the device with **events** (`on`) and the device gives **commands** to its actuators (`handle`).

| Class | Kind | Responsibility |
|---|---|---|
| `Event`, `Command`, `EventHandler`, `CommandHandler`, `Sensor`, `Actuator`, `Device`, `Led`, `Button` | Framework | ModestIoT base classes. |
| `AnalogSoilSensor` | Sensor | EC probe and moisture sensor: averages 8 ADC samples and scales them between two calibration points (inverse sensors allowed). |
| `SoilTemperatureSensor` | Sensor | DS18B20; emits `PROBE_DISCONNECTED_EVENT` and returns `NAN` when it does not answer. |
| `StatusLight` | Actuator | RGB LED made of three framework `Led`s; one command per pattern of the IoT style guide. |
| `FlushPump` | Actuator | Relay + mini pump of the contingency. `START_FLUSH` / `STOP_FLUSH` commands; it stops by itself after `FLUSH_DURATION_MS`. |
| `FlushPolicy` | Support | Pure logic (no hardware) that decides when a wash may start. |
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

## Contingency: washing the soil when the salinity stays Critical
If the EC reaches **Critical** (≥ 1.5 × the crop threshold) the prototype does not only warn: it lowers the salinity with parts that cost a few soles. A relay switches a mini pump that sends fresh water from a bottle to the pot (leaching). `FlushPolicy` keeps it safe:

| Rule | Value in `Config.h` | Why |
|---|---|---|
| Critical in a row before the first wash | `FLUSH_CONFIRM_READINGS` = 2 | One noisy reading does not start the pump |
| Pump time per wash | `FLUSH_DURATION_MS` (60 s in the field, 8 s in Wokwi) | Fixed dose; `FlushPump` also stops itself |
| Wait between washes | `FLUSH_COOLDOWN_MS` (30 min in the field, 20 s in Wokwi) | The water drains and the next reading is real |
| Washes before asking for a person | `FLUSH_MAX_CYCLES` = 3 | The water itself may be salty or the substrate bad |
| No wash if the soil is wetter than | `FLUSH_SATURATED_PERCENT` = 90 % | Avoids waterlogging |
| Back to Normal | — | The wash counter restarts |

It works with or without the Edge Service (it uses the raw EC when the Edge does not answer) and without the relay connected it only logs the decision. Serial log: `Contingency: EC 2.00 dS/m is Critical -> wash 1/3, pump on for 8 s`. The LED blinks cyan twice when the pump starts. Set `CONTINGENCY_ENABLED 0` to turn it off and `PUMP_RELAY_ACTIVE_LOW` to match the relay module (most blue modules are active-low; Wokwi's is active-high). With `PUMP_RELAY_ACTIVE_LOW 1` the pin floats to release the relay: a 5 V module still switches on with 3.3 V on IN, as the prototype showed.

`variants/diagram.sin-contingencia.json` is the previous circuit (without the relay) and `libraries.txt` lists the libraries wokwi.com needs.

## Configuration
Edit `src/Config.h`: `DEVICE_CODE`, `DEVICE_API_KEY` (must match `EDGE_DEVICES` in the Edge Service), Wi-Fi, `EDGE_SERVICE_URL`, `SAMPLE_INTERVAL_MS`, `CROP_THRESHOLD_DSM` and the contingency values. `OFFLINE_MODE 1` is a bench test over USB (no Wi-Fi, no Edge, readings and contingency on the serial monitor).

### Bench demo on the real prototype
```bash
pio run -e bench -t upload
```
The `bench` environment sets `OFFLINE_MODE 1` and `PUMP_RELAY_ACTIVE_LOW 1`: it samples every 100 ms, uses the moisture calibration of the sensor mounted on the prototype, washes for 5 s with 30 s to mix and prints `Pump: IDLE | PUMPING | SOAKING | LOCKED | SATURATED | cycles n/3` on each reading.

### Real board
- `WIFI_CHANNEL 0` scans every channel; the ESP32 only joins 2.4 GHz networks.
- **Moisture calibration:** each reading prints `Raw ADC: conductivity …, moisture …` on the serial monitor (115200). Note the moisture value with the sensor in dry air and in a glass of water up to its line, and set them as `MOISTURE_RAW_DRY` and `MOISTURE_RAW_WET` (defaults 3200 and 1300 are typical for the v1.2 sensor at 3.3 V).

## Build and simulate
```bash
pio run
```
Then open the project with the Wokwi extension for VS Code (`wokwi.toml` points to the firmware, the custom chips and `diagram.json`). Move the sliders of the moisture and TDS chips and the DS18B20 temperature to change the readings. Run the Edge Service on port 5000 of the same PC: the simulator reaches it at `host.wokwi.internal`.
