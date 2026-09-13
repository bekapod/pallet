#ifndef PALLET_SPR_H
#define PALLET_SPR_H

#include <stdint.h>

#define SPR_SLOT_COUNT 40U
#define SPR_NONE 0xFFU

/*
 * Allocate the first free contiguous run of count OAM slots, or SPR_NONE.
 * spr_free() accepts only a wholly in-range half-open range and otherwise
 * does nothing. State initializers must call spr_reset(); the game loop must
 * call spr_hide_unused() once per frame.
 *
 * OAM wrapper calls require a currently allocated slot. Invalid or free slots
 * are ignored.
 */
uint8_t spr_alloc(uint8_t count);
void spr_free(uint8_t first, uint8_t count);
void spr_reset(void);
void spr_hide_unused(void);
void spr_tile(uint8_t slot, uint8_t tile);
void spr_move(uint8_t slot, uint8_t x, uint8_t y);
void spr_prop(uint8_t slot, uint8_t prop);

#endif
