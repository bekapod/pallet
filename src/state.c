#include "state.h"

static const state_t *stack[STATE_STACK_CAPACITY];
static uint8_t depth;

void state_push(const state_t *state) {
    if (!state || depth >= STATE_STACK_CAPACITY)
        return;

    stack[depth++] = state;
    if (state->init)
        state->init();
}

void state_pop(void) {
    const state_t *state;

    if (depth == 0)
        return;

    state = stack[depth - 1];
    if (state->exit)
        state->exit();
    depth--;
}

void state_replace(const state_t *state) {
    if (!state)
        return;

    if (depth != 0)
        state_pop();
    state_push(state);
}

void state_tick(void) {
    const state_t *updating;
    uint8_t first_draw;

    if (depth == 0)
        return;

    updating = stack[depth - 1];
    if (updating->update)
        updating->update();

    first_draw = depth - 1;
    while (first_draw != 0 && stack[first_draw]->draw_under)
        first_draw--;

    for (; first_draw < depth; first_draw++) {
        if (stack[first_draw]->draw)
            stack[first_draw]->draw();
    }
}
