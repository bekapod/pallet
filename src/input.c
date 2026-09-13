#include <gb/gb.h>
#include <stdint.h>

#include "input.h"

uint8_t input_held;
uint8_t input_pressed;
uint8_t input_released;
static uint8_t repeat_right;
static uint8_t repeat_left;
static uint8_t repeat_up;
static uint8_t repeat_down;
static uint8_t held_a;
static uint8_t held_b;

static uint8_t *repeat_counter(uint8_t button) {
    switch (button) {
    case J_RIGHT:
        return &repeat_right;
    case J_LEFT:
        return &repeat_left;
    case J_UP:
        return &repeat_up;
    case J_DOWN:
        return &repeat_down;
    default:
        return 0;
    }
}

void input_update(void) {
    uint8_t previous = input_held;

    input_held = joypad();
    input_pressed = input_held & (uint8_t)~previous;
    input_released = (uint8_t)~input_held & previous;

    if (input_held & J_A) {
        if (held_a != UINT8_MAX)
            held_a++;
    } else
        held_a = 0;
    if (input_held & J_B) {
        if (held_b != UINT8_MAX)
            held_b++;
    } else
        held_b = 0;
}

uint8_t input_held_frames(uint8_t button) {
    if (button == J_A)
        return held_a;
    if (button == J_B)
        return held_b;
    return 0;
}

uint8_t input_repeat(uint8_t button, uint8_t delay, uint8_t rate) {
    uint8_t *counter = repeat_counter(button);

    if (!counter)
        return 0;
    if (!(input_held & button)) {
        *counter = 0;
        return 0;
    }
    if (input_pressed & button) {
        *counter = 0;
        return 1;
    }

    if (*counter != UINT8_MAX)
        (*counter)++;
    if (!rate || *counter < delay)
        return 0;
    return (uint8_t)((*counter - delay) % rate == 0U);
}
