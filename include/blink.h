#ifndef PALLET_BLINK_H
#define PALLET_BLINK_H

#include <stdint.h>

#define BLINK_FRAMES 30

typedef struct {
    uint8_t frames;
    uint8_t visible;
    uint8_t dirty;
} blink_state_t;

void blink_init(blink_state_t *state);
void blink_tick(blink_state_t *state);

#endif
