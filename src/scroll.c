#include <gb/gb.h>
#include <stdint.h>

#include "scroll.h"

uint16_t scroll_x;

static uint16_t speed;
static uint8_t paused;
static uint8_t left_world_col;
static uint32_t absolute_x;
static scroll_column_callback_t on_column;

static void request_column(uint16_t world_col) {
    if (on_column)
        on_column((uint8_t)(world_col & 31U), (uint8_t)world_col);
}

void scroll_set_on_column(scroll_column_callback_t callback) {
    on_column = callback;
}

void scroll_set_speed(uint16_t new_speed) {
    speed = new_speed;
}

void scroll_reset(uint8_t world_col) {
    uint8_t index;

    left_world_col = world_col;
    absolute_x = (uint32_t)world_col * 8U * 256U;
    scroll_x = (uint16_t)absolute_x;
    paused = 0;

    if (!on_column)
        return;
    for (index = 0; index <= SCROLL_VIEW_COLUMNS; index++)
        request_column((uint16_t)world_col + index);
}

void scroll_tick(void) {
    uint32_t old_column;
    uint32_t new_column;

    if (paused)
        return;

    old_column = absolute_x / (8U * 256U);
    absolute_x += speed;
    scroll_x = (uint16_t)absolute_x;
    new_column = absolute_x / (8U * 256U);
    while (old_column < new_column) {
        left_world_col++;
        request_column((uint16_t)left_world_col + SCROLL_LOOKAHEAD_COLUMNS);
        old_column++;
    }
}

void scroll_apply(void) {
    SCX_REG = scroll_px();
}

uint8_t scroll_px(void) {
    return (uint8_t)(scroll_x >> 8);
}

uint8_t scroll_col(void) {
    return (uint8_t)(scroll_px() >> 3);
}

void scroll_pause(void) {
    paused = 1;
}

void scroll_resume(void) {
    paused = 0;
}

uint8_t scroll_world_to_screen(uint32_t world_x_fx) {
    uint32_t offset;

    if (world_x_fx < absolute_x)
        return SCROLL_OFFSCREEN;
    offset = world_x_fx - absolute_x;
    if (offset >= 160U * 256U)
        return SCROLL_OFFSCREEN;
    return (uint8_t)(offset >> 8);
}
