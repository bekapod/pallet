#include "blink.h"

void blink_init(blink_state_t *state) {
    state->frames = BLINK_FRAMES;
    state->visible = 1;
    state->dirty = 0;
}

void blink_tick(blink_state_t *state) {
    if (--state->frames)
        return;

    state->frames = BLINK_FRAMES;
    state->visible ^= 1;
    state->dirty = 1;
}
