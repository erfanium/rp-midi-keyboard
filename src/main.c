#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "keyboard.c"
#include "sustain.c"
#include "tusb.h"

#define DEBOUNCE_SAMPLES 10   // Number of samples for debouncing
#define DEBOUNCE_DELAY_US 10  // Delay between samples in microseconds

int scans = 0;

// Helper function to read debounced column state
uint8_t debounced_read_columns(void) {
  uint8_t debounced = 0xFF;  // Start with all bits high
  for (int i = 0; i < DEBOUNCE_SAMPLES; i++) {
    uint8_t sample = read_columns();
    debounced &= sample;  // Bitwise AND to keep only stable high bits
    sleep_us(DEBOUNCE_DELAY_US);
  }
  return debounced;
}

void init_gpio_pins() { init_keys_gpio_pin(); }

bool timer_callback(struct repeating_timer *t) {
  printf("Stats: %d scans per second\n", scans);
  scans = 0;
  return true;  // Returning true keeps the timer running
}

int main() {
  board_init();
  tud_init(0);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  init_gpio_pins();
  stdio_usb_init();

  struct key_ctx_t key_ctx = init_key_ctx();
  select_row(key_ctx.cr);

  // struct repeating_timer timer;
  // add_repeating_timer_us(-1000000, timer_callback, NULL, &timer);

  while (true) {
    tud_task();
    midi_task();
    scan_keys_task(&key_ctx);
  }

  return 0;
}

void scan_keys_task(struct key_ctx_t *key_ctx) {
  // Instead of reading once, take multiple samples to debounce the input
  uint8_t current = debounced_read_columns();
  uint8_t cr = key_ctx->cr;
  uint8_t changed_pins = current ^ key_ctx->rows[cr];

  if (changed_pins) {
    for (int pin = 0; pin < INPUT_PINS; pin++) {
      if (changed_pins & (1 << pin)) {
        bool current_state = (current & (1 << pin)) != 0;
        int key_id = ((cr * 8) + pin) / 2;
        bool is_sustain = (pin % 2) == 0;

        if (key_id > 36) {
          printf("invalid key_id %d. skipping", key_id);
          continue;
        }

        if (current_state) {
          if (is_sustain) {
            kbd_start_off(key_id);
          } else {
            kbd_end_off(key_id);
          }
        } else {
          if (is_sustain) {
            kbd_start_on(key_id);
          } else {
            kbd_end_on(key_id);
          }
        }
      }
    }
  }

  key_ctx->rows[cr] = current;
  if (cr == 0) {
    scans++;
  }

  uint8_t new_row = (cr + 1) % ROWS;
  select_row(new_row);
  key_ctx->cr = new_row;
  sleep_ms(1);
}

void midi_task(void) {
  uint8_t packet[4];
  while (tud_midi_available()) {
    tud_midi_packet_read(packet);
  }
}
