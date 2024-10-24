#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "bsp/board.h"

#define MAX_KEYS 37
#define KEY_OFFSET 36

typedef struct {
  uint32_t a_time;
  uint32_t b_time;
} KeyState;

static KeyState keys[MAX_KEYS];

uint32_t abs_diff(uint32_t a, uint32_t b) {
  return (uint32_t)llabs((int64_t)a - (int64_t)b);
}

// External functions to be called
void midi_key_on(int key_id, int velocity) {
  uint8_t note_on[3] = {0x90 | 0, KEY_OFFSET + key_id, velocity};
  tud_midi_stream_write(0, note_on, 3);
}

void midi_key_off(int key_id) {
  uint8_t note_off[3] = {0x80 | 0, KEY_OFFSET + key_id, 0};
  tud_midi_stream_write(0, note_off, 3);
}

// Function to calculate velocity based on time difference (mock function, can
// be adjusted)
int calculate_velocity(uint32_t time_diff) {
  printf("calculate_velocity: %d\n", time_diff);
  int velocity =
      127 - (time_diff);  // Assuming 127 max velocity, scale down with time
  if (velocity < 16) velocity = 16;  // Clamp to a minimum value
  return velocity;
}

void kbd_on(int key_id, bool is_a) {
  uint32_t current_time = board_millis();
  KeyState *key = &keys[key_id];
  bool should_flush = false;

  // is it a new A press?
  if (is_a && !key->a_time) {
    key->a_time = current_time;
    should_flush = true;
  }

  if (!is_a && !key->b_time) {
    key->b_time = current_time;
    should_flush = true;
  }

  if (should_flush && key->a_time && key->b_time) {
    uint32_t diff = abs_diff(key->a_time, key->b_time);
    midi_key_on(key_id, calculate_velocity(diff));
  }

  // log
  printf("kbd_on: key_id: %d, is_a: %d\n", key_id, is_a);
}

// MIDI button off handler
void kbd_off(int key_id, bool is_a) {
  KeyState *key = &keys[key_id];
  bool should_flush = false;

  if (is_a && key->a_time) {
    key->a_time = 0;
    should_flush = true;
  }

  if (!is_a && key->b_time) {
    key->b_time = 0;
    should_flush = true;
  }

  if (should_flush && !key->a_time && !key->b_time) {
    midi_key_off(key_id);
  }

  printf("kbd_off: key_id: %d, is_a: %d\n", key_id, is_a);
}