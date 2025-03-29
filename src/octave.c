#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "bsp/board.h"
#include "pico/stdlib.h"

#define INCREASE_PIN 28
#define DECREASE_PIN 29

int8_t offset = 0;

#define DEBOUNCE_DELAY_MS 250

bool last_state_inc = 1;
bool last_state_dec = 1;

uint32_t last_change = 0;

void octave_init_gpio_pins() {
  gpio_init(INCREASE_PIN);
  gpio_set_dir(INCREASE_PIN, GPIO_IN);
  gpio_pull_up(INCREASE_PIN);

  gpio_init(DECREASE_PIN);
  gpio_set_dir(DECREASE_PIN, GPIO_IN);
  gpio_pull_up(DECREASE_PIN);
}

void octave_gpio_scan() {
  uint32_t now = board_millis();

  bool current_state_inc = gpio_get(INCREASE_PIN);

  if (current_state_inc != last_state_inc) {
    last_state_inc = current_state_inc;
    if (offset < 4 && !current_state_inc &&
        now - last_change > DEBOUNCE_DELAY_MS) {
      printf("increase offset\n");
      offset++;
      last_change = now;
    }
  }

  bool current_state_dec = gpio_get(DECREASE_PIN);

  if (current_state_dec != last_state_dec) {
    last_state_dec = current_state_dec;
    if (offset > -4 && !current_state_dec &&
        now - last_change > DEBOUNCE_DELAY_MS) {
      printf("decrease offset\n");
      offset--;
      last_change = now;
    }
  }
}

int8_t octave_get_offset() { return offset * 12; }

void octave_debug() {
  bool current_state_inc = gpio_get(INCREASE_PIN);
  printf("current_state_inc: %d\n", current_state_inc);

  bool current_state_dec = gpio_get(DECREASE_PIN);
  printf("current_state_dec: %d\n", current_state_dec);

  printf("offset: %d\n", offset);
}