#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "keyboard.c"
#include "sustain.c"
#include "tusb.h"

// #define LIB_TINYUSB_DEVICE = 1
#define KEY_SCAN_INTERVAL 2 << 5

void init_gpio_pins() { init_keys_gpio_pin(); }

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

  while (true) {
    tud_task();
    midi_task();
    scan_keys_task(&key_ctx);
  }

  return 0;
}

void scan_keys_task(struct key_ctx_t *key_ctx) {
  static uint32_t start_d = 0;
  if (start_d < KEY_SCAN_INTERVAL) {
    start_d += 1;
    return;
  };  // not enough time
  start_d = 0;

  uint8_t current = read_columns();
  // printf("current_row_state: %d\n", current);
  uint8_t changed_pins = current ^ key_ctx->rows[key_ctx->cr];

  if (changed_pins) {
    // printf("current_row_id: %d\n", key_ctx->cr);
    // printf("changed_pins: %d\n", changed_pins);
    for (int pin = 0; pin < INPUT_PINS; pin += 1) {  // Change from `<=` to `<`
      if (changed_pins & (1 << pin)) {
        bool current_state = (current & (1 << pin)) != 0;
        int key_id = ((key_ctx->cr * 8) + pin) / 2;
        bool is_sustain = (pin % 2) == 0;

        if (current_state) {
          kbd_off(key_id, is_sustain);
        } else {
          kbd_on(key_id, is_sustain);
        }
      }
    }
  }

  key_ctx->rows[key_ctx->cr] = current;

  key_ctx->cr = (key_ctx->cr + 1) % ROWS;
  select_row(key_ctx->cr);
}

// midi task

// Variable that holds the current position in the sequence.
uint32_t note_pos = 0;

void midi_task(void) {
  static uint32_t start_ms = 0;

  uint8_t const cable_num = 0;  // MIDI jack associated with USB endpoint
  uint8_t const channel = 0;    // 0 for channel 1

  // The MIDI interface always creates input and output port/jack descriptors
  // regardless of these being used or not. Therefore incoming traffic should be
  // read (possibly just discarded) to avoid the sender blocking in IO
  uint8_t packet[4];
  while (tud_midi_available()) tud_midi_packet_read(packet);
}
