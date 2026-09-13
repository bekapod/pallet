#ifndef PALLET_INPUT_H
#define PALLET_INPUT_H

#include <stdint.h>

extern uint8_t input_held;
extern uint8_t input_pressed;
extern uint8_t input_released;

void input_update(void);

/*
 * For a held button, return true immediately on press, after `delay` frames,
 * and then every `rate` frames. Use this for repeated UI actions.
 */
uint8_t input_repeat(uint8_t button, uint8_t delay, uint8_t rate);

#endif
