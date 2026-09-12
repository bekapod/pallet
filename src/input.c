#include <gb/gb.h>

#include "input.h"

uint8_t input_held;
uint8_t input_pressed;
uint8_t input_released;

void input_update(void) {
    uint8_t previous = input_held;
    input_held = joypad();
    input_pressed = input_held & (uint8_t)~previous;
    input_released = (uint8_t)~input_held & previous;
}
