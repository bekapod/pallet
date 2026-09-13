#ifndef PALLET_FLASH_H
#define PALLET_FLASH_H

#include <stdint.h>

#include "status.h"

extern uint8_t flash_active;

/* Invert the palettes for the given number of frames, then restore
 * them. Fails with PALLET_BUSY while a fade is running, since only one
 * effect may own the palettes. */
pallet_status_t flash(uint8_t frames);

void flash_tick(void);

#endif
