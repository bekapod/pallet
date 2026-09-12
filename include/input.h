#ifndef PALLET_INPUT_H
#define PALLET_INPUT_H

#include <stdint.h>

extern uint8_t input_held;
extern uint8_t input_pressed;
extern uint8_t input_released;

void input_update(void);

#endif
