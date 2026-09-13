#ifndef PALLET_INPUT_H
#define PALLET_INPUT_H

#include <stdint.h>

#include <gb/gb.h>

/* Query the sampled input state. All three take a button mask such as
 * J_A or J_START and return nonzero while every masked button is in the
 * given edge state for this frame. Call input_update() once per frame
 * (Pallet's runtime does this) before querying. */

uint8_t input_update(void);

/* Buttons currently held down. */
uint8_t input_held(uint8_t buttons);

/* Buttons that went down this frame. */
uint8_t input_pressed(uint8_t buttons);

/* Buttons that came up this frame. */
uint8_t input_released(uint8_t buttons);

/* Return consecutive sampled down frames for exactly J_A or J_B, or zero. */
uint8_t input_held_frames(uint8_t button);

/*
 * For an exact D-pad direction, return true on press, after `delay` held
 * frames, and then every `rate` frames. A zero rate disables held repeats.
 * Other buttons and combined masks return zero.
 */
uint8_t input_repeat(uint8_t button, uint8_t delay, uint8_t rate);

#endif