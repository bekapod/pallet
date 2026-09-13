#include <gb/gb.h>
#include <stdint.h>

#include "shake.h"

static uint8_t frames;
static uint8_t amplitude;
static uint8_t phase;

uint8_t shake_active;

void shake(uint8_t duration, uint8_t requested_amplitude) {
    frames = duration;
    amplitude = requested_amplitude;
    phase = 0;
    shake_active = duration != 0;
    if (!shake_active) {
        SCX_REG = 0;
        SCY_REG = 0;
    }
}

void shake_tick(void) {
    if (!shake_active)
        return;

    if (phase) {
        SCX_REG = (uint8_t)(-(int8_t)amplitude);
        SCY_REG = amplitude;
    } else {
        SCX_REG = amplitude;
        SCY_REG = (uint8_t)(-(int8_t)amplitude);
    }
    phase ^= 1;
    if (--frames)
        return;

    shake_active = 0;
    SCX_REG = 0;
    SCY_REG = 0;
}
