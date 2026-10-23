// Generic TDS / EC module with BNC probe, used on the 1:2 soil-water extract.
// Analog output 0-2.3 V like the real board; the firmware maps 3.3 V to CONDUCTIVITY_MAX_DSM (3.2 dS/m).
#include "wokwi-api.h"
#include <stdlib.h>

// Wokwi's ESP32 ADC maps 0-5 V to 0-4095 (its potentiometer also outputs a 0-5 V ratio), while the real board
// reads about 0-3.3 V. The output is scaled so the firmware gets the same raw ADC as with the real sensor.
#define WOKWI_ADC_SCALE (5.0f / 3.3f)

#define VOLTS_PER_DSM (3.3f / 3.2f)
#define VOLTS_MAX 2.3f

typedef struct {
  pin_t out;
  uint32_t conductivity;
} chip_state_t;

static void update(void *user_data) {
  chip_state_t *chip = (chip_state_t *)user_data;
  float volts = attr_read_float(chip->conductivity) * VOLTS_PER_DSM;
  pin_dac_write(chip->out, (volts > VOLTS_MAX ? VOLTS_MAX : volts) * WOKWI_ADC_SCALE);
}

void chip_init(void) {
  chip_state_t *chip = malloc(sizeof(chip_state_t));
  chip->out = pin_init("A", ANALOG);
  chip->conductivity = attr_init_float("conductivity", 0.8f);
  const timer_config_t config = { .callback = update, .user_data = chip };
  timer_start(timer_init(&config), 10000, true);
  update(chip);
}
