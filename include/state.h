#ifndef PALLET_STATE_H
#define PALLET_STATE_H

#include <stdint.h>

#define STATE_STACK_CAPACITY 4U

typedef struct {
    void (*init)(void);
    void (*update)(void);
    void (*draw)(void);
    void (*exit)(void);
    uint8_t draw_under;
} state_t;

/* State descriptors must have static lifetime; the stack stores their pointers. */
void state_push(const state_t *state);
void state_pop(void);
void state_replace(const state_t *state);
void state_tick(void);

/*
 * Only an update callback may call a state transition API. Init, draw, and
 * exit callbacks must only establish, render, or release their own state.
 * This keeps lifecycle callbacks from changing the stack during state_tick.
 */

#endif
