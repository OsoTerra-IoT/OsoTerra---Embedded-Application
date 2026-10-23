# Custom Wokwi chips

Wokwi has no capacitive soil moisture sensor, TDS / EC module or power parts, so the circuit uses these custom chips.

| Chip | Part bought | Pins | Behavior in the simulation |
|---|---|---|---|
| `soil-moisture-v12` | Capacitive soil moisture sensor v1.2 | VCC, GND, AOUT | Slider 0–100 %. Inverse output: dry ≈ 2.58 V (ADC 3200), wet ≈ 1.05 V (ADC 1300), as `MOISTURE_RAW_DRY/WET` in `Config.h`. |
| `tds-meter` | Generic TDS / EC module with BNC probe (1:2 soil-water extract) | VCC, GND, A | Slider 0–2.2 dS/m. Output 0–2.3 V like the real module; the firmware maps 3.3 V to `CONDUCTIVITY_MAX_DSM` (3.2 dS/m). |
| `solar-panel-6v` | 6 V solar panel | +, − | Drawing only. |
| `tp4056` | TP4056 charger with DW01 protection | IN+, IN−, B+, B−, OUT+, OUT− | Drawing only. |
| `battery-18650` | 18650 Li-ion cell and holder | +, − | Drawing only. |
| `ht7333` | HT7333 3.3 V LDO regulator | VIN, GND, VOUT | Drawing only: Wokwi always powers the board at 3.3 V. |

Wokwi's ESP32 ADC maps 0–5 V to 0–4095, while the real board reads about 0–3.3 V. The two analog chips scale their output by 5 / 3.3 (`WOKWI_ADC_SCALE`), so the firmware reads the same raw ADC values as with the real sensors. Checked on wokwi.com: 0.80 dS/m and 38 % give `Raw ADC: conductivity 1024, moisture 2480` and `0.80 dS/m, 37.9 %`.

## Build

wokwi.com compiles the `.chip.c` files by itself. The Wokwi extension for VS Code loads the `.chip.wasm` files listed in `wokwi.toml`. After changing a chip, rebuild them with zig (`pip install ziglang`):

```bash
for c in chips/*.chip.c; do python -m ziglang cc --target=wasm32-wasi -mexec-model=reactor -Oz -s -Wl,--import-memory -Wl,--export-table -Wl,--no-entry -o "${c%.c}.wasm" "$c"; done
```

`wokwi-api.h` is the chip API header from [wokwi/inverter-chip](https://github.com/wokwi/inverter-chip/blob/main/src/wokwi-api.h).
