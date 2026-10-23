// Two-pin momentary push button: while pressed it closes pin 1 to pin 2 (GND in the OsoSense circuit).
// Wokwi's own push buttons have four legs; this one matches the button bought for the prototype.
#include "wokwi-api.h"
#include <stdlib.h>

typedef struct {
  pin_t pin1;
  uint32_t pressed;
  bool closed;
} chip_state_t;

static void update(void *user_data) {
  chip_state_t *chip = (chip_state_t *)user_data;
  bool closed = attr_read_float(chip->pressed) >= 0.5f;
  if (closed != chip->closed) pin_mode(chip->pin1, closed ? OUTPUT_LOW : INPUT);
  chip->closed = closed;
}

void chip_init(void) {
  chip_state_t *chip = malloc(sizeof(chip_state_t));
  chip->pin1 = pin_init("1", INPUT);
  pin_init("2", INPUT);
  chip->pressed = attr_init_float("pressed", 0);
  chip->closed = false;
  const timer_config_t config = { .callback = update, .user_data = chip };
  timer_start(timer_init(&config), 5000, true);
}
