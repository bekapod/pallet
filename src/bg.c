#include <gb/gb.h>
#include <stdint.h>

#include "bg.h"

/* A single command needs x, y, and tile; spans retain a source pointer,
 * length, and consumed count. Reuse the latter fields to keep the 512-entry
 * queue within the Game Boy's 8 KiB WRAM. */
typedef struct {
    uint8_t x;
    uint8_t y;
    const uint8_t *tiles;
    uint8_t kind_and_len;
    uint8_t value_or_consumed;
} bg_command_t;

#define BG_SINGLE 0U
#define BG_ROW 0U
#define BG_COL 0x40U
#define BG_LEN_MASK 0x3FU

static uint8_t is_single(const bg_command_t *command) {
    return command->kind_and_len == BG_SINGLE;
}

static uint8_t is_column(const bg_command_t *command) {
    return command->kind_and_len & BG_COL;
}

static uint8_t command_len(const bg_command_t *command) {
    return command->kind_and_len & BG_LEN_MASK;
}

static bg_command_t commands[BG_QUEUE_CAPACITY];
static uint16_t head;
static uint16_t tail;
static uint16_t command_count;
static uint16_t pending_tiles;

static uint16_t next_index(uint16_t index) {
    index++;
    if (index == BG_QUEUE_CAPACITY)
        index = 0;
    return index;
}

static bg_command_t *reserve_command(void) {
    bg_command_t *command;

    if (command_count == BG_QUEUE_CAPACITY)
        return 0;

    command = &commands[tail];
    tail = next_index(tail);
    command_count++;
    return command;
}

void bg_put(uint8_t x, uint8_t y, uint8_t tile) {
    bg_command_t *command = reserve_command();

    if (!command)
        return;

    command->x = x & 31U;
    command->y = y & 31U;
    command->kind_and_len = BG_SINGLE;
    command->value_or_consumed = tile;
    pending_tiles++;
}

static void bg_put_span(uint8_t kind, uint8_t x, uint8_t y,
                        const uint8_t *tiles, uint8_t len) {
    bg_command_t *command;

    if (!len || len > 32U)
        return;

    command = reserve_command();
    if (!command)
        return;

    command->x = x & 31U;
    command->y = y & 31U;
    command->tiles = tiles;
    command->kind_and_len = kind | len;
    command->value_or_consumed = 0;
    pending_tiles += len;
}

void bg_put_row(uint8_t x, uint8_t y, const uint8_t *tiles, uint8_t len) {
    bg_put_span(BG_ROW, x, y, tiles, len);
}

void bg_put_col(uint8_t x, uint8_t y, const uint8_t *tiles, uint8_t len) {
    bg_put_span(BG_COL, x, y, tiles, len);
}

void bg_fill(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t tile) {
    uint8_t row;
    uint8_t column;

    for (row = 0; row < h; row++) {
        for (column = 0; column < w; column++)
            bg_put((uint8_t)(x + column), (uint8_t)(y + row), tile);
    }
}

void bg_flush(void) {
    uint16_t budget = BG_FLUSH_TILE_BUDGET;

    while (command_count && budget) {
        bg_command_t *command = &commands[head];

        if (is_single(command)) {
            set_bkg_tile_xy(command->x, command->y,
                            command->value_or_consumed);
            pending_tiles--;
            budget--;
            head = next_index(head);
            command_count--;
            continue;
        }

        {
            uint8_t remaining = command_len(command) -
                                command->value_or_consumed;
            uint8_t chunk = remaining;
            uint8_t edge;

            if (chunk > budget)
                chunk = (uint8_t)budget;
            if (!is_column(command))
                edge = 32U - command->x;
            else
                edge = 32U - command->y;
            if (chunk > edge)
                chunk = edge;

            if (!is_column(command)) {
                set_bkg_tiles(command->x, command->y, chunk, 1,
                              command->tiles + command->value_or_consumed);
            } else {
                set_bkg_tiles(command->x, command->y, 1, chunk,
                              command->tiles + command->value_or_consumed);
            }

            command->value_or_consumed += chunk;
            pending_tiles -= chunk;
            budget -= chunk;
            if (command->value_or_consumed == command_len(command)) {
                head = next_index(head);
                command_count--;
            } else if (!is_column(command)) {
                command->x = (uint8_t)((command->x + chunk) & 31U);
            } else {
                command->y = (uint8_t)((command->y + chunk) & 31U);
            }
        }
    }
}

uint16_t bg_pending(void) {
    return pending_tiles;
}
