#include <stdio.h>

#include "pico/stdlib.h"

#define INPUT_PINS 8   // 0-7
#define OUTPUT_PINS 4  // 8-11
#define ROWS 10

const uint8_t row_pin_state[ROWS] = {12, 2, 10, 6, 14, 1, 9, 5, 13, 3};

struct key_ctx_t {
  uint8_t cr;          // number of current row to scan
  uint8_t rows[ROWS];  // state of rows
};

void init_keys_gpio_pin() {
  for (int pin = 0; pin < INPUT_PINS; pin++) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
  }

  for (int pin = INPUT_PINS; pin < INPUT_PINS + OUTPUT_PINS; pin++) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
  }
}

void select_row(uint8_t row) { gpio_put_all(row_pin_state[row] << INPUT_PINS); }

uint8_t read_columns() { return gpio_get_all() & ((1 << INPUT_PINS) - 1); }

struct key_ctx_t init_key_ctx() {
  struct key_ctx_t ctx;

  ctx.cr = 0;  // Initialize the row number to 0

  // Initialize all rows to 0 (or any default state)
  for (int i = 0; i < ROWS; i++) {
    ctx.rows[i] = 0;
  }

  return ctx;
}
