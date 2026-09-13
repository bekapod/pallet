#include <stdint.h>

#include "shake.h"

static uint8_t frames;
static uint8_t amplitude;
static uint8_t phase;

uint8_t shake_active;
int8_t shake_offset_x;
int8_t shake_offset_y;

void shake(uint8_t duration, uint8_t requested_amplitude) {
    frames = duration;
    amplitude = requested_amplitude;
    phase = 0;
    shake_active = duration != 0;
    if (!shake_active) {
        shake_offset_x = 0;
        shake_offset_y = 0;
    }
}

void shake_tick(void) {
    if (!shake_active)
        return;

    if (phase) {
        shake_offset_x = (int8_t)-amplitude;
        shake_offset_y = (int8_t)amplitude;
    } else {
        shake_offset_x = (int8_t)amplitude;
        shake_offset_y = (int8_t)-amplitude;
    }
    phase ^= 1;
    if (--frames)
        return;

    shake_active = 0;
    shake_offset_x = 0;
    shake_offset_y = 0;
}