#include <stdio.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"

#define INPUT_PINS 8   // 0-7
#define OUTPUT_PINS 4  // 8-11
#define ROWS 10

const uint8_t row_pin_state[ROWS] = {12, 2, 10, 6, 14, 1, 9, 5, 13, 3};

void init_gpio_pins() {
  for (int pin = 0; pin < INPUT_PINS; pin++) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
  }

  for (int pin = INPUT_PINS; pin < INPUT_PINS + OUTPUT_PINS; pin++) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
  }

  // gpio_put(8, 1);
  // gpio_put(9, 1);
  // gpio_put(10, 0);
  // gpio_put(11, 0);
  // gpio_put(12, 0);
}

void set_row(uint8_t row) {
  // for (int pin = 0; pin < OUTPUT_PINS; pin++) {
  // gpio_put(INPUT_PINS + pin, (row_pin_state[row] & (1 << pin)) != 0);
  // }
  gpio_put_all(row_pin_state[row] << INPUT_PINS);
}

uint8_t read_input_pins() { return gpio_get_all() & ((1 << INPUT_PINS) - 1); }

int main() {
  stdio_init_all();
  init_gpio_pins();

  printf("Hello, world!\n");
  sleep_ms(1000);
  printf("Hello, world!\n");
  sleep_ms(1000);
  printf("Hello, world!\n");
  sleep_ms(1000);
  printf("Hello, world!\n");

  uint8_t current_row = 0;  // min=0, max=31
  uint8_t rows_state[ROWS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  while (true) {
    uint8_t current_gpio_states = read_input_pins();
    uint8_t changed_pins = current_gpio_states ^ rows_state[current_row];

    if (changed_pins) {
      for (int pin = 0; pin < INPUT_PINS; pin++) {
        if (changed_pins & (1 << pin)) {
          bool current_state = (current_gpio_states & (1 << pin)) != 0;
          printf("Pin %d: current_state: %d, current_row: %d\n", pin,
                 current_state, current_row);
        }
      }
      rows_state[current_row] = current_gpio_states;
    }

    current_row = (current_row + 1) % ROWS;
    set_row(current_row);
    sleep_ms(1);
  }

  return 0;
}

// used states: 12, 2, 10, 6, 14, 1, 9, 5, 13, 3
