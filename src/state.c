#include "state.h"

#include "fade.h"

static const state_t *stack[PALLET_STATE_STACK_CAPACITY];
static uint8_t depth;
static uint8_t in_update;
static const state_t *pending_state;
static uint8_t pending_faded_frames; /* 0 = plain replace. */

static void apply_pending(void) {
    uint8_t fade_frames = pending_faded_frames;

    state_pop();
    if (state_push(pending_state) == PALLET_OK && fade_frames)
        fade_in(fade_frames);
    pending_state = 0;
    pending_faded_frames = 0;
}

pallet_status_t state_push(const state_t *state) {
    if (!state)
        return PALLET_BAD_ARGUMENT;
    if (depth >= PALLET_STATE_STACK_CAPACITY)
        return PALLET_FULL;

    stack[depth++] = state;
    if (state->init)
        state->init();
    return PALLET_OK;
}

pallet_status_t state_pop(void) {
    const state_t *state;

    if (depth == 0)
        return PALLET_EMPTY;

    state = stack[depth - 1];
    if (state->exit)
        state->exit();
    depth--;
    return PALLET_OK;
}

pallet_status_t state_replace(const state_t *state) {
    if (!state)
        return PALLET_BAD_ARGUMENT;
    if (in_update) {
        pending_state = state;
        pending_faded_frames = 0;
        return PALLET_OK;
    }

    state_pop();
    return state_push(state);
}

pallet_status_t state_replace_faded(const state_t *state,
                                    uint8_t frames_per_step) {
    if (!state)
        return PALLET_BAD_ARGUMENT;
    if (fade_out(frames_per_step) != PALLET_OK)
        return PALLET_BUSY;
    pending_state = state;
    pending_faded_frames = frames_per_step ? frames_per_step : 1;
    return PALLET_OK;
}

void state_tick(void) {
    const state_t *updating;
    uint8_t first_draw;

    if (depth == 0)
        return;

    /* While a faded replace waits for its fade-out, gameplay pauses. */
    if (pending_state && pending_faded_frames && fade_active)
        return;

    updating = stack[depth - 1];
    in_update = 1;
    if (updating->update)
        updating->update();
    in_update = 0;

    if (pending_state)
        apply_pending();

    first_draw = depth - 1;
    while (first_draw != 0 && stack[first_draw]->draw_under)
        first_draw--;

    for (; first_draw < depth; first_draw++) {
        if (stack[first_draw]->draw)
            stack[first_draw]->draw();
    }
}