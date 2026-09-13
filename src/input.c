#include <gb/gb.h>

#include "input.h"

uint8_t input_held;
uint8_t input_pressed;
uint8_t input_released;
static uint8_t repeat_frames[8];

void input_update(void) {
    uint8_t previous = input_held;
    input_held = joypad();
    input_pressed = input_held & (uint8_t)~previous;
    input_released = (uint8_t)~input_held & previous;
}

uint8_t input_repeat(uint8_t button, uint8_t delay, uint8_t rate) {
    uint8_t index = 0;
    uint8_t bit = button;

    while (bit > 1U) {
        bit >>= 1U;
        index++;
    }

    if (!(input_held & button)) {
        repeat_frames[index] = 0;
        return 0;
    }

    if (input_pressed & button) {
        repeat_frames[index] = 0;
        return 1;
    }

    if (repeat_frames[index] != UINT8_MAX)
        repeat_frames[index]++;

    if (!rate || repeat_frames[index] < delay)
        return 0;

    return (uint8_t)((repeat_frames[index] - delay) % rate == 0);
}
