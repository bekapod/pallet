#ifndef PALLET_INPUT_H
#define PALLET_INPUT_H

#include <stdint.h>

extern uint8_t input_held;
extern uint8_t input_pressed;
extern uint8_t input_released;

void input_update(void);

/* Return consecutive sampled down frames for exactly J_A or J_B, or zero. */
uint8_t input_held_frames(uint8_t button);

/*
 * For an exact D-pad direction, return true on press, after `delay` held
 * frames, and then every `rate` frames. A zero rate disables held repeats.
 * Other buttons and combined masks return zero.
 */
uint8_t input_repeat(uint8_t button, uint8_t delay, uint8_t rate);

#endif
