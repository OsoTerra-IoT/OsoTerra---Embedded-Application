// Generic TDS / EC module with BNC probe, used on the 1:2 soil-water extract.
// Analog output 0-2.3 V like the real board; the firmware maps 3.3 V to CONDUCTIVITY_MAX_DSM (3.2 dS/m).
#include "wokwi-api.h"
#include <stdlib.h>

// Wokwi's ESP32 ADC maps 0-5 V to 0-4095 (its potentiometer also outputs a 0-5 V ratio), while the real board
// reads about 0-3.3 V. The output is scaled so the firmware gets the same raw ADC as with the real sensor.
#define WOKWI_ADC_SCALE (5.0f / 3.3f)

#define VOLTS_PER_DSM (3.3f / 3.2f)
#define VOLTS_MAX 2.3f

// The PUMP pin listens to the relay signal (GPIO 13). While the pump runs, the washed soil loses salt: the simulated
// extract drops WASH_DSM_PER_SECOND each second, so the contingency can be seen closing the loop. Moving the slider
// sets a new value and starts over.
#define WASH_DSM_PER_SECOND 0.03f
#define TICK_SECONDS 0.01f

typedef struct {
  pin_t out;
  pin_t pump;
  uint32_t conductivity;
  float current;
  float lastSlider;
} chip_state_t;

static void update(void *user_data) {
  chip_state_t *chip = (chip_state_t *)user_data;
  float slider = attr_read_float(chip->conductivity);
  if (slider != chip->lastSlider) { chip->lastSlider = slider; chip->current = slider; }
  if (pin_read(chip->pump)) { chip->current -= WASH_DSM_PER_SECOND * TICK_SECONDS; if (chip->current < 0) chip->current = 0; }
  float volts = chip->current * VOLTS_PER_DSM;
  pin_dac_write(chip->out, (volts > VOLTS_MAX ? VOLTS_MAX : volts) * WOKWI_ADC_SCALE);
}

void chip_init(void) {
  chip_state_t *chip = malloc(sizeof(chip_state_t));
  chip->out = pin_init("A", ANALOG);
  chip->pump = pin_init("PUMP", INPUT_PULLDOWN);
  chip->conductivity = attr_init_float("conductivity", 0.8f);
  chip->lastSlider = attr_read_float(chip->conductivity);
  chip->current = chip->lastSlider;
  const timer_config_t config = { .callback = update, .user_data = chip };
  timer_start(timer_init(&config), 10000, true);
  update(chip);
}
