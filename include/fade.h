#ifndef PALLET_FADE_H
#define PALLET_FADE_H

#include <stdint.h>

#include "status.h"

extern uint8_t fade_active;

/* Start fading the palettes to black. Fails with PALLET_BUSY while a
 * flash is running, since only one effect may own the palettes. */
pallet_status_t fade_out(uint8_t frames_per_step);

/* Start fading the palettes back from black. Same exclusivity rule. */
pallet_status_t fade_in(uint8_t frames_per_step);

void fade_tick(void);

#endif
