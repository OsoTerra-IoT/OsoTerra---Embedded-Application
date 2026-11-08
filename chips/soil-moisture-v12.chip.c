// Capacitive soil moisture sensor v1.2: analog output that falls as the soil gets wetter.
// Dry air ~2.58 V (ADC 3200) and water up to the line ~1.05 V (ADC 1300), as MOISTURE_RAW_DRY/WET in Config.h.
#include "wokwi-api.h"
#include <stdlib.h>

// Wokwi's ESP32 ADC maps 0-5 V to 0-4095 (its potentiometer also outputs a 0-5 V ratio), while the real board
// reads about 0-3.3 V. The output is scaled so the firmware gets the same raw ADC as with the real sensor.
#define WOKWI_ADC_SCALE (5.0f / 3.3f)

#define VOLTS_DRY 2.58f
#define VOLTS_WET 1.05f

// The PUMP pin listens to the relay signal (GPIO 13): while the pump runs the soil gets wetter (WET_PERCENT_PER_SECOND).
// Moving the slider sets a new value and starts over.
#define WET_PERCENT_PER_SECOND 2.5f
#define TICK_SECONDS 0.01f

typedef struct {
  pin_t aout;
  pin_t pump;
  uint32_t moisture;
  float current;
  float lastSlider;
} chip_state_t;

static void update(void *user_data) {
  chip_state_t *chip = (chip_state_t *)user_data;
  float slider = attr_read_float(chip->moisture);
  if (slider != chip->lastSlider) { chip->lastSlider = slider; chip->current = slider; }
  if (pin_read(chip->pump)) { chip->current += WET_PERCENT_PER_SECOND * TICK_SECONDS; if (chip->current > 100) chip->current = 100; }
  float percent = chip->current;
  pin_dac_write(chip->aout, (VOLTS_DRY + (VOLTS_WET - VOLTS_DRY) * percent / 100.0f) * WOKWI_ADC_SCALE);
}

void chip_init(void) {
  chip_state_t *chip = malloc(sizeof(chip_state_t));
  chip->aout = pin_init("AOUT", ANALOG);
  chip->pump = pin_init("PUMP", INPUT_PULLDOWN);
  chip->moisture = attr_init_float("moisture", 38);
  chip->lastSlider = attr_read_float(chip->moisture);
  chip->current = chip->lastSlider;
  const timer_config_t config = { .callback = update, .user_data = chip };
  timer_start(timer_init(&config), 10000, true);
  update(chip);
}
