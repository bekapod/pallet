#include <gb/gb.h>
#include <stdint.h>

#include "spr.h"

static uint8_t allocated[PALLET_SPR_SLOTS];

static uint8_t valid_slot(uint8_t slot) {
    return slot < PALLET_SPR_SLOTS && allocated[slot];
}

uint8_t spr_alloc(uint8_t count) {
    uint8_t first;
    uint8_t offset;

    if (!count || count > PALLET_SPR_SLOTS)
        return SPR_NONE;
    for (first = 0; first <= PALLET_SPR_SLOTS - count; first++) {
        for (offset = 0; offset < count; offset++)
            if (allocated[first + offset])
                break;
        if (offset == count) {
            for (offset = 0; offset < count; offset++)
                allocated[first + offset] = 1;
            return first;
        }
    }
    return SPR_NONE;
}

void spr_free(uint8_t first, uint8_t count) {
    uint8_t offset;

    if (first == SPR_NONE || !count || first >= PALLET_SPR_SLOTS ||
        count > PALLET_SPR_SLOTS - first)
        return;
    for (offset = 0; offset < count; offset++)
        allocated[first + offset] = 0;
}

void spr_reset(void) {
    uint8_t slot;

    for (slot = 0; slot < PALLET_SPR_SLOTS; slot++) {
        allocated[slot] = 0;
        move_sprite(slot, 0U, 0U);
    }
}

void spr_hide_unused(void) {
    uint8_t slot;

    for (slot = 0; slot < PALLET_SPR_SLOTS; slot++)
        if (!allocated[slot])
            move_sprite(slot, 0U, 0U);
}

void spr_tile(uint8_t slot, uint8_t tile) {
    if (valid_slot(slot))
        set_sprite_tile(slot, tile);
}

void spr_move(uint8_t slot, uint8_t x, uint8_t y) {
    if (valid_slot(slot))
        move_sprite(slot, x, y);
}

void spr_prop(uint8_t slot, uint8_t prop) {
    if (valid_slot(slot))
        set_sprite_prop(slot, prop);
}
