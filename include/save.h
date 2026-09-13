#ifndef PALLET_SAVE_H
#define PALLET_SAVE_H

#include <stdint.h>

#define SAVE_MAGIC 0xA5U
#define SAVE_HEADER_BYTES 2U

/*
 * Nonempty save buffers begin with the caller's expected schema version.
 * save_load validates it against SRAM, zeroes the whole buffer on failure,
 * and callers must restore the version before saving after a failed load.
 */
uint8_t save_load(void *dst, uint8_t len);
void save_write(const void *src, uint8_t len);
void save_wipe(void);

#endif
