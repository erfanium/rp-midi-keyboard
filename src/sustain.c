#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "bsp/board.h"

#define MAX_KEYS 37
#define KEY_OFFSET 36

typedef struct {
  uint32_t a_time;
  uint32_t b_time;
  uint8_t note_on_number;
} KeyState;

static KeyState keys[MAX_KEYS];

uint32_t abs_diff(uint32_t a, uint32_t b) {
  return (uint32_t)llabs((int64_t)a - (int64_t)b);
}

// Function to calculate velocity based on time difference
int calculate_velocity(uint32_t time_diff) {
  int velocity =
      127 - (time_diff * 2);  // Assuming 127 max velocity, scale down with time
  if (velocity < 16) velocity = 16;  // Clamp to a minimum value
  return velocity;
}

int calculate_note(int key_idx) {
  return KEY_OFFSET + octave_get_offset() + key_idx;
}

// External functions to be called
void midi_key_on(int key_idx, KeyState *key) {
  if (key->note_on_number) return;

  uint32_t diff = abs_diff(key->a_time, key->b_time);
  uint8_t number = calculate_note(key_idx);
  key->note_on_number = number;
  uint8_t note_on[3] = {0x90 | 0, number, calculate_velocity(diff)};
  tud_midi_stream_write(0, note_on, 3);
}

void midi_key_off(int key_idx, KeyState *key) {
  uint8_t note_off[3] = {0x80 | 0, key->note_on_number, 0};
  tud_midi_stream_write(0, note_off, 3);
  key->note_on_number = 0;
}

void kbd_start_on(int key_idx) {
  printf("kbd_start_on: %d\n", key_idx);

  uint32_t current_time = board_millis();
  KeyState *key = &keys[key_idx];
  key->a_time = current_time;

  if (key->b_time) {
    midi_key_on(key_idx, key);
  }
}

void kbd_end_on(int key_idx) {
  printf("kbd_end_on: %d\n", key_idx);
  uint32_t current_time = board_millis();
  KeyState *key = &keys[key_idx];
  key->b_time = current_time;

  if (key->a_time) {
    midi_key_on(key_idx, key);
  }
}

void kbd_end_off(int key_idx) {
  printf("kbd_end_off: %d\n", key_idx);
  KeyState *key = &keys[key_idx];

  key->b_time = 0;
  if (!key->a_time) {
    midi_key_off(key_idx, key);
  }
}

void kbd_start_off(int key_idx) {
  printf("kbd_start_off: %d\n", key_idx);

  KeyState *key = &keys[key_idx];
  key->a_time = 0;
  if (!key->b_time) {
    midi_key_off(key_idx, key);
  }
}
