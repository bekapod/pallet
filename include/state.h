#ifndef PALLET_STATE_H
#define PALLET_STATE_H

#include <stdint.h>

#include "status.h"

#define STATE_STACK_CAPACITY 4U

typedef struct {
    void (*init)(void);
    void (*update)(void);
    void (*draw)(void);
    void (*exit)(void);
    uint8_t draw_under;
} state_t;

/* State descriptors must have static lifetime; the stack stores their pointers. */

/* Returns PALLET_FULL when the stack already holds STATE_STACK_CAPACITY
 * states. The state's init runs on push. */
pallet_status_t state_push(const state_t *state);

/* Returns PALLET_EMPTY when the stack is empty. */
pallet_status_t state_pop(void);

/* Swap the whole stack for one state. When called from an update callback
 * the change is deferred until that update returns, so the old state
 * always finishes its frame. A null state is PALLET_BAD_ARGUMENT. */
pallet_status_t state_replace(const state_t *state);

/* Like state_replace(), wrapped in a fade: the palettes fade out over
 * frames_per_step, then the stack swaps, then the palettes fade back in.
 * Returns PALLET_BUSY while another palette effect owns the palettes. */
pallet_status_t state_replace_faded(const state_t *state,
                                    uint8_t frames_per_step);

void state_tick(void);

/*
 * Only an update callback may call a state transition API. Init, draw, and
 * exit callbacks must only establish, render, or release their own state.
 * Transitions requested there take effect after the update returns.
 */

#endif